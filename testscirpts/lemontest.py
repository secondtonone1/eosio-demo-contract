#-*-coding:utf-8-*-
import argparse
import subprocess
#cleos = '/home/secondtonone/workspace/twinkle03/cleos.sh get info'
cleos = '/home/secondtonone/workspace/twinkle03/cleos.sh '
KEY = 'EOS8ieq79SvxchRVgDbt2wyDu1wvwFAjgLpKNqDRCHKSQZsZWsYBd'

def runcmd(cmds):
    try:
        (status, output) = subprocess.getstatusoutput(cmds)
        if status != 0:
            print('status error,cmd is %s' %cmds)
            print(status)
            print('\n')
            return
        print(output)
        print('\n')
    except:
        print('run %s except' %cmds)

def regproducer(*args):
    try:
        if(len(args) <= 0):
            return
        if(len(args) <= 1):
            producer1 = args[0]
            #print("**********",producer1)
            cleostmp = cleos + ' create account eosio '+ producer1+' '+KEY+' '+ KEY+' -p eosio'
            runcmd(cleostmp)
            #用eosio 注册producer1
            cleostmp1 = cleos + ' system regproducer ' + producer1 + ' '+ KEY+' https://www.eocs.io  5 eosio' 
            runcmd(cleostmp1)
            #producer1给自己投票
            cleostmp2 = cleos + ' system voteproducer prods '+ producer1 + ' ' + producer1
            runcmd(cleostmp2)
        if(len(args) >= 2):
            producer1 = args[0]
            producer2 = args[1]
            cleostmp1 = cleos + ' create account eosio '+ producer1+ ' '+KEY+' '+ KEY+' -p eosio'
            cleostmp2 = cleos + ' create account eosio '+ producer2+ ' '+KEY+' '+ KEY+' -p eosio'
            runcmd(cleostmp1)
            runcmd(cleostmp2)
             #用eosio 注册producer1
            cleostmp3 = cleos + ' system regproducer ' + producer1 + ' '+ KEY+' https://www.eocs.io  5 eosio' 
            runcmd(cleostmp3)
            #producer1给自己投票
            cleostmp4 = cleos + ' system voteproducer prods '+ producer1 + ' ' + producer1
            runcmd(cleostmp4)

            #producer2 给 producer1 注册
            cleostmp5 = cleos+ ' system regproducer ' + producer2 + ' '+KEY+' https://www.eocs.io  5 '+producer1
            runcmd(cleostmp5)

            #producer2 给自己投票
            cleostmp6 = cleos + ' system voteproducer prods '+producer2+ ' '+ producer2
            runcmd(cleostmp6)

    except:
        print("regproducer except")
        pass

# 测试set eosio.code
def setcode(*args):
    try:
        if len(args) <= 0:
            return
        ts = ''
        def initts():
            nonlocal ts
            ts = cleos+ ' set account permission ' + args[0] + ' active ' + "'"+'{"threshold": 1,"keys": [{"key":"'\
            + KEY+'", "weight":1}],"accounts": [{"permission":{"actor":"'+args[0]\
            +'","permission":"eosio.code"},"weight":1}]}'+"'"\
            +' owner -p '+args[0] +'@owner'
            pass
        initts()
        print(ts)
        print('\n')
        #cleostmp = cleos + ' create account eosio '+ args[0]+' '+KEY+' '+ KEY+' -p eosio'
        #runcmd(cleostmp)
        runcmd(ts)
    except:
        print("set code except")
        pass
 
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.description = '联盟连测试'
    parser.add_argument("-p1","--producer1",help="测试生产者切换",type=str)
    parser.add_argument("-p2","--producer2",help="测试生产者切换",type=str)
    parser.add_argument("-sd","--setcode",help="测试set code",type=str)
    args = parser.parse_args()

    if args.producer1 and (args.producer2 == None)  :
        #print("只接受到producer1，它是",args.producer1)
        regproducer(args.producer1)
    if args.producer2 and (args.producer1 == None) :
        #print("只接受到producer2，它是",args.producer2)
        regproducer(args.producer2)
    if args.producer1 and args.producer2 :
        regproducer(args.producer1,args.producer2)
        #print("收到producer1和producer2",args.producer1,args.producer2)
    if args.setcode:
        setcode(args.setcode)
