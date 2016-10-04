/execute build.bat file to build
/output: .dll,.lib,.exp,.obj

ibconnect:`ibfeed 2:(`ibconnect;1)
ibdisconnect:`ibfeed 2:(`disconnect;1)
ibisconnected:`ibfeed 2:(`isconnected;1)
ibreqscanparams:`ibfeed 2:(`reqscanparams;1)
ibreqscansub:`ibfeed 2:(`reqscansub;1)
ibreqtime:`ibfeed 2:(`reqtime;1)
ibreqhist:`ibfeed 2:(`reqhist;1)
ibplaceorder:`ibfeed 2:(`placeorder;1)
ibcancelorder:`ibfeed 2:(`cancelorder;1)

/examples
/ipaddr:`${11_(first x ss ":")#x} sum 1#2_system "ping -n 1 -4 mic-asus"
/ibconnect[enlist[`192.168.1.10;`7496;`10]]
/ibconnect[enlist[ipaddq;`7496;`10]]
/ibdisconnec[]
/ibisdiconnected[]
/ibreqscanparams[]
/ibreqscansub
/ibreqtime[]
/ibplaceorder[enlist[`AAPL;`SMART;`USD;`STK;`238;`BUY;`10;`LMT;`113;`DAY]]
/ibcancelorder[220]
/get2dhist[`AAPL;`STK;`SMART;`USD]	
/getd2dhist[`AAPL;`STK;`SMART;`USD;3]

/use ltime to correct time to local exchange time:
/update ltime time from ibreqhist[enlist[`AAPL;`STK;`SMART;`USD;`$"20160927 23:59:59";`$"10 D";`$"1 min";`TRADES;`1;`2]]
