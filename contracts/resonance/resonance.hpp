#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>

namespace resonance {

using namespace std;
using namespace eosio;

constexpr int MAX_STAGE = 350;
constexpr int MAX_SOURCE_AMOUNT = 100'0000;
constexpr symbol SOURCE_SYMBOL = symbol("EOS", 4);
constexpr symbol TARGET_SYMBOL = symbol("EOC", 4);
const asset SOURCE_MINIMUM = asset(1000, SOURCE_SYMBOL); // 0.1 EOS
constexpr int64_t MAX_SOURCE_AMOUNT_PER_ORDER = 1000 * 10000; // 1000 EOS

class [[eosio::contract("resonance")]] resonance : public contract {
public:
    using contract::contract;

    resonance(name s, name code, datastream<const char*> ds);
    ~resonance();

    [[eosio::action]]
    void transfer(name from, name to, asset quantity, string memo);

    [[eosio::action]]
    void redeem(uint64_t order_id);

    [[eosio::action]]
    void clear(uint32_t max_num);

private:
    struct [[eosio::table]] stats {
        asset source_total = asset(0, SOURCE_SYMBOL);
        asset target_total = asset(0, TARGET_SYMBOL);
        uint64_t integral_orders = 0;
    };

    struct [[eosio::table]] stage {
        uint64_t stage;
        asset source_price; // unit: EOC/EOS
        asset target_total;
        asset target_released;
        asset source_locked;

        uint64_t primary_key() const { return stage; }
    };

    struct [[eosio::table]] order {
        uint64_t id;
        uint64_t integral_id; // orders of same integral_id belong to an integral order
        uint32_t time; // time in seconds from 1970 of the current block
        name buyer;
        name receiver;
        asset source;
        asset target;
        int64_t group_id;
        asset target_from_group = asset(0, TARGET_SYMBOL);

        uint64_t primary_key() const { return id; }
        uint64_t by_group_id() const { return group_id; }
        uint128_t by_receiver() const { return (uint128_t(receiver.value) << 64) + id; }
    };

    struct [[eosio::table]] group {
        uint64_t id;
        int32_t capacity;
        int32_t joined;

        uint64_t primary_key() const { return id; }
    };

    struct [[eosio::table]] unlimited_group_stats { // only used for unlimited groups
        uint64_t id;
        asset source_total = asset(0, SOURCE_SYMBOL);

        uint64_t primary_key() const { return id; }
    };

    struct [[eosio::table]] unlimited_group_joiner { // only used for unlimited groups
        name joiner;
        uint64_t group_id;

        uint64_t primary_key() const { return joiner.value; }
    };

    typedef eosio::singleton<"stats"_n, stats> stats_singleton;
    typedef eosio::multi_index<"stages"_n, stage> stages;
    typedef eosio::multi_index<"orders"_n, order,
        indexed_by<"groupid"_n, const_mem_fun<order, uint64_t, &order::by_group_id>>,
        indexed_by<"receiver"_n, const_mem_fun<order, uint128_t, &order::by_receiver>>
    > orders;
    typedef eosio::multi_index<"groups"_n, group> groups;
    typedef eosio::multi_index<"groupstats"_n, unlimited_group_stats> group_stats;
    typedef eosio::multi_index<"groupjoiners"_n, unlimited_group_joiner> group_joiners;

    void next_stage(stages& s, uint64_t current_stage = 0);

    stats _stats;
    bool _cleared = false;
};

}
