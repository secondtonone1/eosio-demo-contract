#!/bin/bash
################################################################################
#
# Scrip Created by http://CryptoLions.io
# For EOS Junlge testnet
#
# https://github.com/CryptoLions/
#
################################################################################

DATADIR=/home/secondtonone/workspace/wallet
$DATADIR/stop.sh
/home/secondtonone/workspace/eocs/build/programs/keosd/keosd  --unlock-timeout 999999999 --http-validate-host=false  --wallet-dir $DATADIR --data-dir $DATADIR --http-server-address 127.0.0.1:55553 "$@" > $DATADIR/stdout.txt 2> $DATADIR/stderr.txt  & echo $! > $DATADIR/wallet.pid
#/eos/home/Jungle/private_chain/eos-source/build/programs/keosd/keosd --wallet-dir $DATADIR --data-dir $DATADIR --http-server-address 47.92.97.56:55553 "$@" > /eos/home/Jungle/private_chain/wallet/stdout.txt 2> /eos/home/Jungle/private_chain/wallet/stderr.txt  & echo $! > $DATADIR/wallet.pid
echo "Wallet started"
