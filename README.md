# ibfeed

This is a dll to access interactive brokers functionality from inside kdb.

The functions available at the moment are the following:

ibconnect:`ibfeed 2:(`ibconnect;1)<br>
ibdisconnect:`ibfeed 2:(`disconnect;1)<br>
ibisconnected:`ibfeed 2:(`isconnected;1)<br>
ibreqscanparams:`ibfeed 2:(`reqscanparams;1)<br>
ibreqscansub:`ibfeed 2:(`reqscansub;1)<br>
ibreqtime:`ibfeed 2:(`reqtime;1)<br>
ibreqhist:`ibfeed 2:(`reqhist;1)<br>
ibplaceorder:`ibfeed 2:(`placeorder;1)<br>
ibcancelorder:`ibfeed 2:(`cancelorder;1)<br>

Using the 2: kdb function the dll is accessed.<br>
The dll uses the tws api 9.71. Unlike the github project [Qinteractivebrokers](https://github.com/mortensorensen/QInteractiveBrokers) it doesn't use<br>
[Rudi Meier's TWS API](https://github.com/rudimeier/twsapi), which I had trouble
building anyway.<br>
It's purpose is not a feedhandler to asynchronously stream quotes to a kdb+ process.<br>
I think this is best done in a separate process with an executable and I will probably upload<br>
one at a later point.<br>

The makefile was prepared for MS Visual C++ 14. It was built for the 32bit kdb+ version 3.2<br>
and uses the 32bit c.dll. Both this and the ibfeed dll must be copied inside the c:\q\w32<br>
directpry where q.exe resides.<br>

Here are a few examples:

Because the hostname cannot be resolved(will correct this at some point) you can use<br>
ping to find it.<br>
reqscanparams writes to file xmlparams.xml (about 370kb long!)<br>

/resolve ip address<br>
ipaddr:`${11_(first x ss ":")#x} sum 1#2_system "ping -n 1 -4 mic-asus"<br>
/connect to ip address,port,as clientid<br>
ibconnect[enlist[`192.168.1.10;`7496;`10]]<br>
ibconnect[enlist[ipaddq;`7496;`10]]<br>
ibdisconnec[]<br>
ibisconnected[]<br>
/creates xml file<br>
ibreqscanparams[]<br>
/returns result of stock scanner<br>
ibreqscansub<br>
/prints the server time<br>
ibreqtime[]<br>
/SYM,EXCH,CUR,SECTYPE,TRANSMIT,OID,ACTION,QTY,TYPE,AUXPRC,LMTPRC,TIF,OCAGRP,ORDREF,GAT<br>
ibplaceorder[enlist[`AAPL;`SMART;`USD;`STK;`1;`238;`BUY;`10;`LMT;`;`113;`;`DAY;`;`;`]]<br>
/cancels order using orderid<br>
ibcancelorder[220]<br>
update ltime time from ibreqhist[enlist[`AAPL;`STK;`SMART;`USD;`$"20160715 23:59:59";`$"10 D";`$"1 min";`$"TRADES";`$"1";`$"2"]]<br>
/or using a few help functions<br>
date2dstr:{(,) over enlist[ssr[($).z.D-x;".";""];" 23:59:59"]}<br>
get2dhist:{[a;b;c;d]update ltime time from ibreqhist[enlist[a;b;c;d;`$date2dstr[0];`$"2 D";`$"1 min";$[`STK=b;`TRADES;`MIDPOINT];`$"1";`$"2"]] }<br>
get2dhist[`AAPL;`STK;`SMART;`USD]	<br>

/use ltime to correct time to local exchange time:<br>
update ltime time from ibreqhist[enlist[`AAPL;`STK;`SMART;`USD;`$"20160927 23:59:59";`$"10 D";`$"1 min";`TRADES;`1;`2]]<br>

All functions take a single argument but it is sometimes a list of symbols, because it is easier<br>
to handle both in kdb(using enlist) and in c++(using kS(kparam)[0]).<br>

License is for [non-commercial](http://interactivebrokers.github.io/) and most of the source code is ib sourced files anyway.<br>
