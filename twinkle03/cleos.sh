#!/bin/bash
################################################################################
#
# Scrip Created by http://CryptoLions.io
# For EOS Junlge testnet
#
# https://github.com/CryptoLions/
#
################################################################################
CLEOS=/home/secondtonone/workspace/eocs/build/programs/cleos/cleos
$CLEOS -u http://127.0.0.1:5993 --wallet-url http://127.0.0.1:55553 "$@"
