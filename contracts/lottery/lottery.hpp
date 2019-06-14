#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

namespace lottery {

    using namespace std;
    using namespace eosio;

    const asset REWARD_ASSET = asset(20000'0000, symbol("EOC", 4));

    class [[eosio::contract("lottery")]] lottery : public contract {
    public:
        using contract::contract;

        lottery(name s, name code, datastream<const char*> ds);
        ~lottery();

        [[eosio::action]]
        void reward(name account);

        [[eosio::action]]
        void officialdraw(uint32_t num);

        [[eosio::action]]
        void clear(uint32_t max_num);

        [[eosio::action]]
        void transfer(name from, name to, asset quantity, string memo);

    private:
        void transfer_send(name from, name to, asset quantity, string memo);

        struct [[eosio::table]] member {
            uint64_t number;
            name account;
            int64_t dice;

            uint64_t primary_key() const { return number; }
            uint64_t by_account() const { return account.value; }
        };

        struct [[eosio::table]] winner {
            uint64_t stage;
            name account;

            uint64_t primary_key() const { return stage; }
            uint64_t by_account() const { return account.value; }
        };

        typedef eosio::multi_index<"members"_n, member,
                indexed_by<"account"_n, const_mem_fun<member, uint64_t, &member::by_account>>
                > members;
        typedef eosio::multi_index<"winners"_n, winner,
                indexed_by<"account"_n, const_mem_fun<winner, uint64_t, &winner::by_account>>> winners;
    };

}
