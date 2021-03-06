# ibfeed

This is a dll to access interactive brokers functionality from inside kdb.

The functions available at the moment are the following:

```
ibconnect:`ibfeed 2:(`ibconnect;1)
ibdisconnect:`ibfeed 2:(`disconnect;1)
ibisconnected:`ibfeed 2:(`isconnected;1)
ibreqscanparams:`ibfeed 2:(`reqscanparams;1)
ibreqscansub:`ibfeed 2:(`reqscansub;1)
ibreqtime:`ibfeed 2:(`reqtime;1)
ibreqhist:`ibfeed 2:(`reqhist;1)
ibreqids:`ibfeed 2:(`reqids;1)
ibplaceorder:`ibfeed 2:(`placeorder;1)
ibcancelorder:`ibfeed 2:(`cancelorder;1)
```
Using the 2: kdb function the dll is accessed.<br>
The dll uses the tws api 9.71. Unlike the github project [Qinteractivebrokers](https://github.com/mortensorensen/QInteractiveBrokers) it doesn't use<br>
[Rudi Meier's TWS API](https://github.com/rudimeier/twsapi), which I had trouble
building anyway.<br>
Its purpose is not a feedhandler to asynchronously stream quotes to a kdb+ process.<br>
I think this is best done in a separate process with an executable and I will probably upload<br>
one at a later point.<br>
Most important in this library is the historical data request and order transmission.<br>

The makefile works with MS Visual C++ 14. It has been tested with the 32bit kdb+ version 3.2<br>
and the 32bit c.dll. Both this and the ibfeed dll must be copied inside the c:\q\w32<br>
directory where q.exe resides. build.bat builds the dll using the makefile and copies it<br>
inside the q directory.<br>

Here are a few examples:

Because the hostname cannot be resolved(will correct this at some point) you can use ping to find it.<br>

```
/resolve ip address
ipaddr:`${11_(first x ss ":")#x} sum 1#2_system "ping -n 1 -4 mic-asus"<br>
/connect to ip address,port,as clientid
ibconnect[enlist[`192.168.1.10;`7496;`10]]
ibconnect[enlist[ipaddr;`7496;`10]]
ibdisconnec[]
ibisconnected[]
/writes to file xmlparams.xml (about 370kb long!)
ibreqscanparams[]
/returns result of stock scanner
ibreqscansub
/prints the server time
ibreqtime[]
/returns the next orderid
ibreqids[]
/SYM,EXCH,CUR,SECTYPE,TRANSMIT(1/0),OrderID,ACTION,QTY,TYPE,AUXPRC,LMTPRC,TIF,OCAGRP,ORDREF,GAT(eg. 20161004 10:00:00 in order to use convert to symbol `$"20161004 10:00:00" )
ibplaceorder[enlist[`AAPL;`SMART;`USD;`STK;`1;`238;`BUY;`10;`LMT;`;`113;`;`DAY;`;`;`]]
/cancels order using orderid
ibcancelorder[220]
update ltime time from ibreqhist[enlist[`AAPL;`STK;`SMART;`USD;`$"20160715 23:59:59";`$"10 D";`$"1 min";`$"TRADES";`$"1";`$"2"]]
/or using a few help functions
date2dstr:{(,) over enlist[ssr[($).z.D-x;".";""];" 23:59:59"]}
get2dhist:{[a;b;c;d]update ltime time from ibreqhist[enlist[a;b;c;d;`$date2dstr[0];`$"2 D";`$"1 min";$[`STK=b;`TRADES;`MIDPOINT];`$"1";`$"2"]] }
get2dhist[`AAPL;`STK;`SMART;`USD]

/use ltime to correct time to local exchange time:
update ltime time from ibreqhist[enlist[`AAPL;`STK;`SMART;`USD;`$"20160927 23:59:59";`$"10 D";`$"1 min";`TRADES;`1;`2]]
```

About the ibreqhist function it is important to remember that the pacing limitations of ib only<br>
allow one call every 10 sec(specifically no more than 60 in 10mins). <br>
Each call can get a max. of 10days of 1min data. Because of that it takes roughly 1hour<br>
to download a decade of 1min historical data of one stock. One way to do it is by using the kdb timer. 


All functions take a single argument but it is sometimes a list of symbols, because it is easier<br>
to handle both in kdb(using enlist) and in c++(using kS(kparam)[0]).<br>

License is for [non-commercial](http://interactivebrokers.github.io/) use and most of the source code is ib sourced files anyway. <br>
This project builds on the C++ example provided in the ib api and the files modified were Main.cpp, PosixTestClient.cpp and PosixTestClient.h. Directories src and Shared are not included here because of a condition that the api code is not to be published.
