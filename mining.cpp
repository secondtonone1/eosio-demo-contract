#include "mining.hpp"
void mining::buycount(name account, uint64_t count){
    require_auth(_self);
    uint64_t curtime = now();
    auto iter = miningdatas.find(account.value);
    eosio_assert(iter != miningdatas.end(), "the account not found");
    eosio_assert(iter->ifbuy == 0, "the account has buyed!");
    miningdatas.modify(iter, _self, [&](auto& newmining){
            newmining.time_stamp = now();
            newmining.count = count;
            newmining.earnings = 0;
            newmining.ifbuy = 1;
        });
    
}

void mining::getearnings(name account)
{
    require_auth(_self);
    auto iter = miningdatas.find(account.value);
    eosio_assert(iter != miningdatas.end(), "the account not found");
    eosio_assert(iter -> ifearnings == 0, "the account has got earnings");
    auto nowearns = calc_earnings(iter->time_stamp, iter->count, iter->earnings);
    miningdatas.modify(iter, _self, [&](auto& newmining){
            newmining.time_stamp = now();
            newmining.count = 0;
            newmining.earnings = nowearns;
            newmining.ifearnings = 1;
        });
}

void mining::regaccount(name account){

    require_auth(_self);
    auto iter = miningdatas.find(account.value);
    
    eosio_assert(iter == miningdatas.end(), "the farm already exists");

    miningdatas.emplace(_self, [&](auto& newmining){
            newmining.pk = account.value;
            newmining.account = account;
            newmining.time_stamp = now();
            newmining.count = 0;
            newmining.earnings = 0;
            newmining.ifbuy = 0;
            newmining.ifearnings = 0;
        });
}

double mining::calc_earnings(uint64_t lasttime, uint64_t count, double earnings)
{
    if (count <= 0)
        return 0;
    uint64_t curtime = now();
    if ((curtime -lasttime)/(24*60*60*7) <= 0)
        return earnings;
    return (curtime - lasttime) /(24*60*60) * count * benefit /365 + count;
}


void mining::destroyuser(name account)
{
    require_auth(_self);
    auto findit = miningdatas.find(account.value);
    eosio_assert(findit != miningdatas.end(), "the account not found");
    miningdatas.erase(findit);
}


EOSIO_DISPATCH(mining, (buycount)(regaccount)(getearnings)(destroyuser))