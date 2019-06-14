#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/system.h>
#include <string>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;
using std::string;

const uint64_t ANNUALIZED_RETURN_M = 100; //年化收益率分子
const uint64_t ANNUALIZED_RETURN_D = 100; //年化收益率分母
const asset MIN_QUANTITY = asset(1000'0000, symbol("EOC", 4));
const uint64_t FULLYEAR_SECONDS = 365*24*3600;
const uint64_t SEVEN_DAY_SECONDS = 3600;


class [[eosio::contract("mining")]] mining : public contract {
public:
    using contract::contract;

    mining(name n , name code, datastream<const char*> ds)
            : contract(n,code, ds){}
    ~mining(){}

    [[eosio::action]]
    void transfer(name from ,name to, asset quantity, string memo);

    [[eosio::action]]
    void claimreward(name account);

    [[eosio::action]]
    void stop(uint32_t count);

    private:

    asset mining_revenue(asset quantity, uint64_t duration)
    {
        return quantity*(duration/FULLYEAR_SECONDS) * (ANNUALIZED_RETURN_M/ANNUALIZED_RETURN_D);
    }

    void transfer_(name from, name to, asset quantity);
    

    struct [[eosio::table]] member {
        name account;
        asset quantity;
        uint32_t starttime;
        uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<"members"_n, member >members;
};
