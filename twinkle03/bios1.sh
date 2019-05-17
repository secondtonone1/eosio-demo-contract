for a in eosio.token eosio.stake eosio.ram eosio.ramfee eosio.saving eosio.bpay eosio.vpay eosio.names eosio.msig; do ./cleos.sh create account eosio $a EOS8H93TkrkxWiTReLUURpauvSo8BD6jC91WJL31jHapV6r73bFDY EOS8H93TkrkxWiTReLUURpauvSo8BD6jC91WJL31jHapV6r73bFDY -p eosio; done

./cleos.sh set contract eosio /home/secondtonone/workspace/eocs.contracts/build/contracts/eosio.bios -p eosio

#./cleos.sh push action eosio setsymbol '["EOCS"]' -p eosio

./cleos.sh set contract eosio.token /home/secondtonone/workspace/eocs.contracts/build/contracts/eosio.token -p eosio.token


./cleos.sh push action eosio.token create '["eosio", "1000000000.0000 EOCS", 0, 0, 0]' -p eosio.token

./cleos.sh push action eosio.token issue '["eosio",  "900000000.0000 EOCS", "init"]' -p eosio

./cleos.sh set contract eosio /home/secondtonone/workspace/eocs.contracts/build/contracts/eosio.system -p eosio

./cleos.sh push action eosio init '[0,"4,EOCS"]' -p eosio

./cleos.sh system newaccount eosio eocseosioicp EOS5qXmhaP1V14fiEjWfboHRx5KYR4psuAi2bmT3qiE6THVa2i4nu EOS5qXmhaP1V14fiEjWfboHRx5KYR4psuAi2bmT3qiE6THVa2i4nu  --stake-net "100000.0000 EOCS" --stake-cpu "100000.0000 EOCS" --buy-ram-kbytes  99999 -p  eosio

./cleos.sh system newaccount eosio eocseosrelay  EOS7wEPjoAYQpN7NuJf3GkZv9VGs6mUDVqpVGnew7GGiKkir3ubTu EOS7wEPjoAYQpN7NuJf3GkZv9VGs6mUDVqpVGnew7GGiKkir3ubTu  --stake-net "100000.0000 EOCS" --stake-cpu "100000.0000 EOCS" --buy-ram-kbytes  99999 -p  eosio

./cleos.sh set account permission eocseosioicp sendaction '{"threshold":1,"keys":[], "accounts": [{"permission":{"actor":"eocseosioicp","permission":"eosio.code"},"weight":1}] }' "active" -p eocseosioicp

./cleos.sh set action permission eocseosioicp eocseosioicp sendaction sendaction -p eocseosioicp

./cleos.sh set contract eocseosioicp /home/secondtonone/workspace/eocs.contracts/build/contracts/icp  -p eocseosioicp

./cleos.sh push action eocseosioicp  setpeer '{"peer": "eocseosioicp"}' -p eocseosioicp

