#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/system.h>
#include <string>
using namespace eosio;
using std::string;

class [[eosio::contract("mining")]] mining : public contract {
public:
    using contract::contract;

    mining(name n , name code, datastream<const char*> ds)
            : contract(n,code, ds), miningdatas(_self,_self.value),benefit(0.01) {}

    [[eosio::action]]
    void buycount(name account, uint64_t count);

    [[eosio::action]]
    void regaccount(name account);

    [[eosio::action]]
    void getearnings(name account);

    [[eosio::action]]
    void destroyuser(name account);
    
    private:

    double calc_earnings(uint64_t lasttime, uint64_t count, double earnings);

    struct [[eosio::table, eosio::contract("mining")]] mining_data{
        uint64_t pk;
        name    account;
        //时间戳
        uint64_t time_stamp;
        //从上个时间戳开始投入的数量
        uint64_t count;
        //截止上个时间戳累计收益
        double earnings;
        //是否购买过
        uint16_t ifbuy;
        //是否取出收益
        uint16_t ifearnings;

        uint64_t primary_key() const { return pk; }
        uint64_t     by_time() const { return time_stamp; }
        uint64_t     by_count() const {return count;}
        double     by_earnings() const {return earnings;}

        name     by_account() const {return account;}
    };

    typedef eosio::multi_index<"miningdatas"_n, mining_data,
    indexed_by<"earnings"_n, const_mem_fun<mining_data, double, &mining_data::by_earnings>>
    > miningdata;
    miningdata miningdatas;
    double benefit;
};
