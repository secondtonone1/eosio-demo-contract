#include "lottery.hpp"
#include"eosio.token.hpp"
#include <eosiolib/random.hpp>
#include <eosiolib/random.h>
#include"eosio.token.hpp"
namespace lottery {

    lottery::lottery(name s, name code, datastream<const char*> ds) : contract(s, code, ds) {
    }

    lottery::~lottery() {
    }

    seed_seq_ptr get_seed()
    {
       char buf[64]; // 12 * sizeof(uint32_t)
       size_t size = random_seed(buf, sizeof(buf));
       eosio_assert( size > 0 && size <= sizeof(buf), "buffer is too small" );
       uint32_t* seq = reinterpret_cast<uint32_t*>(buf);
       return std::make_unique<std::seed_seq>(seq, seq + size/4);
    }

    uint64_t get_dice(uint64_t s, uint64_t e) {
       auto seed= eosio::minstd_rand0(get_seed());
       std::uniform_int_distribution<uint64_t> distribution(s,e);
       uint64_t dice = distribution(seed);
       print("dice is :", dice );
       return dice;
    }

    void lottery::reward(name account) {
       require_auth(account);

       const auto dice = get_dice(0,99);

       members members_(_self, _self.value);
       auto member_idx = members_.get_index<"account"_n>();

       auto iter_find = member_idx.find(account.value);
       eosio_assert(iter_find != member_idx.end() , "You are not participating flash exchange");
       eosio_assert(iter_find->dice == -1 , "Cannot repeat the lucky draw");

       member_idx.modify(iter_find, account, [&](auto& t){
           t.account = account;
           t.dice = dice;
       });

       if(dice % 10 == 8)
       {
          winners winners_(_self, _self.value);
          winners_.emplace(account, [&](auto& t){
              t.stage = winners_.available_primary_key();
              t.account = account;
          });

          transfer_send(_self, account, REWARD_ASSET, "Congratulations on winning");
       }
    }

    void lottery::officialdraw(uint32_t num) {
       require_auth(_self);

       members members_(_self, _self.value);
       winners winners_(_self, _self.value);
       eosio_assert(members_.begin() != members_.end(), "no member");

       auto dice = get_dice(0, members_.rbegin()->number);
       auto iter = members_.lower_bound(dice);

       bool loop = false;
       for (int i = 0; i < num;) {
          if (iter == members_.end()) {
             if (!loop) {
                loop = true;
                iter = members_.begin();
                continue;
             }
             eosio_assert(false, "Not enough members");
          }
          if (iter->dice %10 == 8 || iter->dice == -1) {iter++;continue;}

          members_.modify(iter, _self, [&](auto& t){
              t.dice = 88888;
          });

          winners_.emplace(_self, [&](auto &t) {
              t.stage = winners_.available_primary_key();
              t.account = iter->account;
          });

          transfer_send(_self, iter->account, REWARD_ASSET, "Thanks for participating");
          ++i;
          iter++;
       }
    }

    void lottery::transfer(name from, name to, asset quantity, string memo) {
       if (from != _self or to == _self or quantity.symbol != REWARD_ASSET.symbol) return;

       members members_(_self, _self.value);
       auto member_idx = members_.get_index<"account"_n>();
       if (member_idx.find(to.value) == member_idx.end()) {
          members_.emplace(_self, [&](auto& t){
              t.number = members_.available_primary_key();
              t.account = to;
              t.dice = -1;
          });
       }
    }


    void lottery::transfer_send(name from, name to, asset quantity, string memo) {
       eosio::print_f("transfer_send: %, %, %, %", from, to, quantity, memo);

       static constexpr eosio::name token_account{"eosio.token"_n};
       static constexpr eosio::name active_permission{"active"_n};

       INLINE_ACTION_SENDER(eosio::token, transfer)(
               token_account, { {from, active_permission} },
               { from, to, quantity, memo }
      );
    }

    void lottery::clear(uint32_t max_num) {
       require_auth(_self);

       members members_(_self, _self.value);
       for (auto it = members_.begin(); it != members_.end();) {
          if (max_num <= 0) break; --max_num;
          it = members_.erase(it);
       }
       winners winners_(_self, _self.value);
       for (auto it = winners_.begin(); it != winners_.end();) {
          if (max_num <= 0) break; --max_num;
          it = winners_.erase(it);
       }
    }

}

extern "C" {

void apply(uint64_t receiver, uint64_t code, uint64_t action) {
   if (code == "eosio.token"_n.value and action == "transfer"_n.value) {
      switch (action) {
         EOSIO_DISPATCH_HELPER(lottery::lottery, (transfer))
      }
      return;
   }

   if (code == receiver) {
      switch (action) {
         EOSIO_DISPATCH_HELPER(lottery::lottery, (reward)(officialdraw)(clear))
      }
   }
}

}
