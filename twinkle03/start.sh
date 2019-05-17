#!/bin/bash
################################################################################
#
# Scrip Created by http://CryptoLions.io
# For EOS Junlge testnet
#
# https://github.com/CryptoLions/
#
################################################################################

NODEOS=/home/secondtonone/workspace/eocs/build/programs/nodeos/nodeos
#NODEOS=/tkteos/home/eocs/build/programs/nodeos/nodeos
DATADIR=/home/secondtonone/workspace/twinkle03
DIR=/home/secondtonone/workspace/twinkle03
$DATADIR/stop.sh
#--genesis-json  genesis.json  --max-irreversible-block-age 500000
$NODEOS  --data-dir $DIR/data/ --config-dir $DATADIR "$@" > $DATADIR/stdout.txt 2> $DATADIR/stderr.txt &  echo $! > $DATADIR/nodeos.pid
