echo "will start"

/home/secondtonone/workspace/twinkle02/start.sh  --genesis-json  genesis.json;
sleep 2;
/home/secondtonone/workspace/twinkle01/start.sh  --genesis-json  genesis.json;
sleep 2;
/home/secondtonone/workspace/twinkle03/start.sh  --genesis-json  genesis.json;
sleep 2;

echo "2222222222222"
#cd /tkteos/home/twinkle/twinkle03
#pwd
#sleep 2
#sh /home/secondtonone/workspace/twinkle03/bios1.sh 
#sleep 2
#sh /home/secondtonone/workspace/twinkle03/create_accout.sh
#sleep 2
#sh /home/secondtonone/workspace/twinkle03/regproducer.sh
#sleep 2
#sh /home/secondtonone/workspace/twinkle03/bios2.sh
#sleep 2

echo " twinkl03 start end"



