## 编译挖矿合约
eosio-cpp ./mining.cpp -o mining.wasm --abigen

## 创建账号

./cleos.sh system newaccount eosio mining EOS5qXmhaP1V14fiEjWfboHRx5KYR4psuAi2bmT3qiE6THVa2i4nu EOS5qXmhaP1V14fiEjWfboHRx5KYR4psuAi2bmT3qiE6THVa2i4nu  --stake-net "100000.0000 EOCS" --stake-cpu     "100000.0000 EOCS" --buy-ram-kbytes  99999 -p  eosio

## 发布合约
./cleos.sh set contract mining /home/secondtonone/workspace/eocs.contracts/mining  -p mining
## 调用合约
./cleos.sh push action mining regaccount '["poirot"]' -p mining@active
## 查询表
./cleos.sh get table mining mining miningdatas

## 购买
./cleos.sh push action mining buycount '["poirot",1024]' -p mining@active
## 获取利息
./cleos.sh push action mining getearnings '["poirot"]' -p mining@active
## 删除用户
./cleos.sh push action mining destroyuser  '["poirot"]' -p mining@active
