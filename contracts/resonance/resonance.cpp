#include "resonance.hpp"
#include <cmath>
#include <map>
#include <set>

namespace resonance {

const int64_t SOURCE_SYMBOL_PRECISION = std::pow(10, SOURCE_SYMBOL.precision());
const int64_t TARGET_SYMBOL_PRECISION = std::pow(10, TARGET_SYMBOL.precision());

const map<uint32_t, uint32_t> GROUP_DISCOUNT = { // {group capacity, discount percentage}
    {3, 3}, {5, 5}, {7, 7}, {-1, 2} // -1 means unlimited group capacity
};

resonance::resonance(name s, name code, datastream<const char*> ds) : contract(s, code, ds) {
    stats_singleton st(_self, _self.value);
    _stats = st.get_or_default(stats{});
}

resonance::~resonance() {
    if (_cleared) return;
    stats_singleton st(_self, _self.value);
    st.set(_stats, _self);
}

int64_t targetAmountAtStage(float n) {
   auto amount = 0.945 * (40000*20 - n*n - (float)(n/3)*(n/3) - (float)(n/4)*(n/4) - (float)(n/5)*(n/5) -
                          (n/8)*(n/8) - (250000-(n-499)*(n-499))*2.2);
   return (int64_t)(amount);
}

int64_t sourceAmountAtStage(int64_t n) {
   if (n <= 50 )
      return 2345;
   else
      return 2680;
//   float avg = MAX_SOURCE_AMOUNT/float(MAX_STAGE);
//   float first = 2100.0;
//   float step = (avg - first)*2/MAX_STAGE;
//
//   return int64_t(first+step* float(n));
}

void resonance::next_stage(stages& s, uint64_t current_stage) {
   eosio::check(current_stage < MAX_STAGE, "max stage");

    s.emplace(_self, [&](auto& s) {
        s.stage = current_stage + 1; // first stage is 1
        auto targetAmount = targetAmountAtStage(s.stage);
        s.target_total = asset(targetAmount * TARGET_SYMBOL_PRECISION, TARGET_SYMBOL);
        s.source_price = asset(static_cast<double>(targetAmount) / sourceAmountAtStage(s.stage) * TARGET_SYMBOL_PRECISION, TARGET_SYMBOL);
        s.target_released = asset(0, TARGET_SYMBOL);
        s.source_locked = asset(0, SOURCE_SYMBOL);
    });
}

void resonance::transfer(name from, name to, asset quantity, string memo) {
    if (from == _self or to != _self) {
        return;
    }

    eosio::check(quantity.symbol == SOURCE_SYMBOL, "invalid source symbol");
    eosio::check(quantity >= SOURCE_MINIMUM, "too little source amount");
    eosio::check(quantity.amount <= MAX_SOURCE_AMOUNT_PER_ORDER, "too much quantity");

    /* memo format:
     * <receiver> [initiate_group:<group_capacity>|join_group:<group_id>]
     */

    name receiver;
    auto receiver_end = memo.find(' ');
    if (receiver_end != std::string::npos) receiver = name(memo.substr(0, receiver_end));
    else receiver = name(memo);
    if (!receiver) return; // not resonance transfer

    groups group(_self, _self.value);
    groups::const_iterator group_iter = group.end();
    if (receiver_end != std::string::npos) {
        string group_str = memo.substr(receiver_end + 1);
        if (group_str.find("initiate_group:") == 0) {
            auto group_capacity = std::stol(group_str.substr(string("initiate_group:").size()));
            eosio::check(GROUP_DISCOUNT.find(group_capacity) != GROUP_DISCOUNT.cend(), "invalid group capacity");
            group_iter = group.emplace(_self, [&](auto& g) {
                g.id = group.available_primary_key();
                g.capacity = group_capacity;
                g.joined = 1;
            });

            if (group_capacity == -1) {
                group_stats groupstats(_self, _self.value);
                groupstats.emplace(_self, [&](auto& g) {
                    g.id = group_iter->id;
                });
            }
        } else if (group_str.find("join_group:") == 0) {
            auto group_id = std::stoull(group_str.substr(string("join_group:").size()));
            group_iter = group.find(group_id);
            eosio::check(group_iter != group.end(), "unable to find group");
            eosio::check(group_iter->joined < group_iter->capacity or group_iter->capacity == -1, "group is completed");

            group.modify(group_iter, same_payer, [&](auto& g) {
                g.joined += 1;
            });
            group_iter = group.find(group_id);
        }

        if (group_iter->capacity == -1) {
            group_joiners groupjoiners(_self, _self.value);
            groupjoiners.emplace(_self, [&](auto& j) {
                j.joiner = receiver;
                j.group_id = group_iter->id;
            });
        }
    }

    group_joiners groupjoiners(_self, _self.value);
    auto groupjoiners_iter = groupjoiners.find(receiver.value);
    if (groupjoiners_iter != groupjoiners.end()) {
        group_stats groupstats(_self, _self.value);
        auto groupstats_iter = groupstats.find(groupjoiners_iter->group_id);
        groupstats.modify(groupstats_iter, same_payer, [&](auto& g) {
            g.source_total += quantity;
        });
    }

    stages s(_self, _self.value);

    if (s.rbegin() == s.rend()) {
        next_stage(s, 0); // first stage
    }
    auto current_stage = s.rbegin();

    ++_stats.integral_orders;

    auto source_locked = quantity;
    bool to_next_stage = false;

    auto resonate = [&]() {
        auto target_amount = source_locked.amount * current_stage->source_price.amount / SOURCE_SYMBOL_PRECISION;
        auto target = current_stage->target_total - current_stage->target_released;
        if (target.amount < target_amount) {
            source_locked.amount = target.amount / current_stage->source_price.amount * SOURCE_SYMBOL_PRECISION;
            to_next_stage = true;
        } else {
            target.amount = target_amount;
        }

        if (target.amount > 0) {
            orders order(_self, _self.value);
            order.emplace(_self, [&](auto& o) {
                o.id = order.available_primary_key();
                o.integral_id = _stats.integral_orders - 1;
                o.time = now();
                o.buyer = from;
                o.receiver = receiver;
                o.source = source_locked;
                o.target = target;
                if (group_iter != group.end()) {
                    o.group_id = group_iter->id;
                    auto discount_percentage = GROUP_DISCOUNT.at(group_iter->capacity);
                    o.target_from_group = asset(o.target.amount * discount_percentage / 100, o.target.symbol);
                } else {
                    o.group_id = -1; // no group
                }
            });

            s.modify(*current_stage, same_payer, [&](auto& s) {
                s.target_released += target;
                s.source_locked += source_locked;
            });

            _stats.source_total += source_locked;
            _stats.target_total += target;
        }
    };

    resonate();

    if (to_next_stage) {
        next_stage(s, current_stage->stage);
        auto current_stage = s.rbegin();

        source_locked = quantity - source_locked;
        eosio::check(source_locked.amount <= sourceAmountAtStage(current_stage->stage) * SOURCE_SYMBOL_PRECISION, "exceeds stage source amount limit");

        resonate();
    }

    if (group_iter == group.end()) return; // not group purchase

    std::map<name, uint64_t> buyers; // map buyer to integral_id
    orders order(_self, _self.value);
    auto by_group_id = order.get_index<"groupid"_n>();
    auto group_order_iter = by_group_id.find(group_iter->id);
    while (group_order_iter != by_group_id.end() and group_order_iter->group_id == group_iter->id) {
        if (buyers.count(group_order_iter->buyer)) {
            eosio::check(group_order_iter->integral_id == buyers.at(group_order_iter->buyer), "one buyer can only join one time per group");
        } else {
            buyers[group_order_iter->buyer] = group_order_iter->integral_id;
        }

        if (group_iter->joined == group_iter->capacity or group_iter->capacity == -1) { // group completed or unlimited
            _stats.target_total += group_order_iter->target_from_group;
        }
        ++group_order_iter;
    }
    if (group_iter->joined == group_iter->capacity) { // group completed
        eosio::check(buyers.size() == group_iter->capacity, "invalid group joined");
    }
}

void resonance::redeem(uint64_t order_id) {
}

void resonance::clear(uint32_t max_num) {
    require_auth(_self);

    stages stage(_self, _self.value);
    for (auto it = stage.begin(); it != stage.end();) {
        if (max_num <= 0) break; --max_num;
        it = stage.erase(it);
    }
    orders order(_self, _self.value);
    for (auto it = order.begin(); it != order.end();) {
        if (max_num <= 0) break; --max_num;
        it = order.erase(it);
    }
    groups group(_self, _self.value);
    for (auto it = group.begin(); it != group.end();) {
        if (max_num <= 0) break; --max_num;
        it = group.erase(it);
    }
   group_joiners groupjoiners(_self, _self.value);
   for (auto it = groupjoiners.begin(); it != groupjoiners.end();) {
      if (max_num <= 0) break; --max_num;
      it = groupjoiners.erase(it);
   }

   group_stats groupstats(_self, _self.value);
   for (auto it = groupstats.begin(); it != groupstats.end();) {
      if (max_num <= 0) break; --max_num;
      it = groupstats.erase(it);
   }

    stats_singleton(_self, _self.value).remove();
    _cleared = true;
}

}

extern "C" {

void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    if (code == "eosio.token"_n.value and action == "transfer"_n.value) {
        switch (action) {
            EOSIO_DISPATCH_HELPER(resonance::resonance, (transfer))
        }
        return;
    }

    if (code == receiver) {
        switch (action) {
            EOSIO_DISPATCH_HELPER(resonance::resonance, (redeem)(clear))
        }
    }
}

}
