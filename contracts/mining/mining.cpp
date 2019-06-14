#include "mining.hpp"

 
void mining::transfer(name from ,name to, asset quantity, string memo){
     print("transfer---------------------");
        //from若等于_self，那么该合约转代币也会调用这个action，我们想做的只有接收代币的时候才会触发该动作
        //to必须等于_self，因为接收者始终是我们自己
        if(from == _self || to != _self || from=="eosio"_n)
        {
            return;
        }
       
        eosio_assert( quantity.is_valid(), "invalid supply.");
        //数量大于1000EOC
        eosio_assert( quantity.amount >= MIN_QUANTITY.amount, "Minimun quantity 1000 EOC.");
        eosio_assert(quantity.symbol == MIN_QUANTITY.symbol, "invalid source symbol");
    
        members members_(_self, _self.value);
        //一个用户不能同时参与多个挖矿
        eosio_assert(members_.find(from.value) == members_.end() , "Cannot repeat the mining.");

        members_.emplace(_self, [&](auto& t){
            t.account = from;
            t.quantity = quantity;
            t.starttime = now();//获取当前时间戳
        });
}


 void mining::transfer_(name from, name to, asset quantity)
{
    eosio::print_f("transfer: %, %, %, %", from, to, quantity);
    static constexpr eosio::name token_account{"eosio.token"_n};
    static constexpr eosio::name active_permission{"active"_n};

    action(permission_level{from, "active"_n}, "eosio.token"_n, "transfer"_n, std::make_tuple(from, to, quantity, string("Mining revenue!"))).send(); 
    /*
    INLINE_ACTION_SENDER(eosio::token, transfer)(
               token_account, { {from, active_permission} },
               { from, to, quantity, memo }
               */
}


void mining::claimreward(name account)
{
        require_auth(account);

        members members_(_self, _self.value);
        auto it = members_.find(account.value);
        //没有参与
        eosio_assert(it != members_.end(), "The user was not involved.");

        //挖矿持续时间
        uint64_t duration = now() - it->starttime;
	    eosio::print_f("duration is %d", duration);
        if(duration < SEVEN_DAY_SECONDS)
        {
            transfer_(_self, account, it->quantity);
            //删除该用户挖矿记录
            members_.erase(it);
            return;
        }

        //获得挖矿奖励
        asset revenue = mining_revenue(it->quantity, duration);

        //发送挖矿奖励
        transfer_(_self, account, it->quantity+revenue);

        //删除该用户挖矿记录
        members_.erase(it);

}

    void mining::stop(uint32_t count)
    {
        require_auth(_self);

        //将所有挖矿奖励发送给对应用户
        members members_(_self, _self.value);
        for (auto it = members_.begin(); it != members_.end();)
        {
            if(count <= 0){
                break;
            }
            //挖矿持续时间
            uint64_t duration = now() - it->starttime;
            if(duration < SEVEN_DAY_SECONDS)
            {
                transfer_(_self, it->account, it->quantity);
                //删除该用户挖矿记录
                it = members_.erase(it);
                count--;
                continue;
            }
            //获得挖矿奖励
            asset revenue = mining_revenue(it->quantity, duration);
            //发送挖矿奖励
            transfer_(_self, it->account, it->quantity+revenue);
            it = members_.erase(it);
            count--;
        }
    }

extern "C" {

void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    if (code == "eosio.token"_n.value and action == "transfer"_n.value) {
        switch (action) {
            EOSIO_DISPATCH_HELPER(mining::mining, (transfer))
        }
        return;
    }

    if (code == receiver) {
        switch (action) {
            EOSIO_DISPATCH_HELPER(mining::mining, (claimreward)(stop))
        }
    }
}
}
