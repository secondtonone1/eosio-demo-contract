./cleos.sh system newaccount eosio eocs11111111 EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd --stake-net "10000.0000 EOCS" --stake-cpu "10000.0000 EOCS" --buy-ram-kbytes  9999 -p  eosio
./cleos.sh system newaccount eosio eocs11111112 EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd --stake-net "10000.0000 EOCS" --stake-cpu "10000.0000 EOCS" --buy-ram-kbytes  9999 -p  eosio
./cleos.sh system newaccount eosio eocs11111113 EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd --stake-net "10000.0000 EOCS" --stake-cpu "10000.0000 EOCS" --buy-ram-kbytes  9999 -p  eosio
./cleos.sh system newaccount eosio eocs11111114 EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd --stake-net "10000.0000 EOCS" --stake-cpu "10000.0000 EOCS" --buy-ram-kbytes  9999 -p  eosio

./cleos.sh transfer eosio eocs11111111 "100000000.0000 EOCS" -p eosio
./cleos.sh transfer eosio eocs11111112 "50000000.0000 EOCS" -p eosio
./cleos.sh transfer eosio eocs11111113 "50000000.0000 EOCS" -p eosio
./cleos.sh transfer eosio eocs11111114 "50000000.0000 EOCS" -p eosio




./cleos.sh  push action eosio delegatebw '{"from":"eocs11111111", "receiver":"eocs11111111", "stake_net_quantity":"50000000.0000 EOCS", "stake_cpu_quantity":"50000000.0000 EOCS", "transfer": false}' -p eocs11111111

./cleos.sh  push action eosio delegatebw '{"from":"eocs11111112", "receiver":"eocs11111112", "stake_net_quantity":"25000000.0000 EOCS", "stake_cpu_quantity":"25000000.0000 EOCS", "transfer": false}' -p eocs11111112
./cleos.sh  push action eosio delegatebw '{"from":"eocs11111113", "receiver":"eocs11111113", "stake_net_quantity":"25000000.0000 EOCS", "stake_cpu_quantity":"25000000.0000 EOCS", "transfer": false}' -p eocs11111113
./cleos.sh  push action eosio delegatebw '{"from":"eocs11111114", "receiver":"eocs11111114", "stake_net_quantity":"25000000.0000 EOCS", "stake_cpu_quantity":"25000000.0000 EOCS", "transfer": false}' -p eocs11111114


#./cleos.sh system voteproducer prods eocs11111111 aocsproducea eocsproduceb aocsproducec
./cleos.sh system voteproducer prods eocs11111111 aocsproducea
./cleos.sh system voteproducer prods eocs11111112 eocsproduceb 
./cleos.sh system voteproducer prods eocs11111113 aocsproducec 
./cleos.sh system voteproducer prods eocs11111114 aocsproducec

#./cleos.sh system voteproducer prods eocs11111112 aocsproducea eocsproduceb aocsproducec eocsproduced eocsproducee eocsproducef eocsproduceg eocsproduceh eocsproducei eocsproducej eocsproducek eocsproducel eocsproducem eocsproducen eocsproduceo eocsproducep eocsproduceq uocsproducer socsproduces tocsproducet eocsproduceu
