/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#ifdef _WIN32
# include <Windows.h>
# define sleep( seconds) Sleep( seconds * 1000);
#else
# include <unistd.h>
#endif

#define KXVER 3

#include <stdio.h>
#include <float.h>
#include "k.h"
#include "Contract.h"
#include "Order.h"
#include <iostream>

#include "PosixTestClient.h"

const unsigned MAX_ATTEMPTS = 5;
const unsigned SLEEP_TIME = 10;
extern int finish=0;
extern int reconnect=0;

PosixTestClient client;

extern K khist=0;
extern K kcontr=0;
extern std::string scannerparams="";
extern std::string curtimestr="";
extern int nextid=0;

//{11_(first x ss ":")#x} sum 1#2_system "ping -n 1 -4 mic-asus"   / get micasus ip
//do NOT use exit(), it exits kdb!
extern "C" K ibconnect(K host){ //single arg -> default port,clientId
	unsigned int port = 7496;
	int clientId = 9;
	if(client.isConnected()) {
		return 0;
	} else {
		reconnect=0;
		if(host->t<0)
			client.connect( host->s, port, clientId);
		else {
			client.connect(kS(host)[0],std::stoi(kS(host)[1]),std::stoi(kS(host)[2]));
		}
	}
	return 0;
}

extern "C" K disconnect(K ignore){
	if(client.isConnected()) {
		client.disconnect();
	}
	return 0;
}

extern "C" K reqscanparams(K ignore){
	if(client.isConnected()) {
		client.reqscanparams();
		while( client.isConnected()) {
			client.processMessages();
			//http://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char#347959
			if(finish==1) { 
				finish=0;
		//		K res;
		//		printf("size:%d\n",scannerparams.size());
		//		char * str = new char[scannerparams.size() + 1];
		//		std::copy(scannerparams.begin(), scannerparams.end(), str);
		//		str[scannerparams.size()] = '\0';
				return 0;
		//		res=ks(ss(str));
		//		delete[] str;
		//		return res;
			}
		}
	}	
	return 0;
}

extern "C" K reqscansub(K ignore){
	if(client.isConnected()) {
		client.reqscansub();
		while( client.isConnected()) {
			client.processMessages();
	
			if(finish==1) { 
				finish=0;
				return 0;
				//return ks(ss("scan"));
			}
		}
	}	
	return 0;
}

extern "C" K isconnected(K ignore){
	if(client.isConnected()) 
		return kb(1);
	else return kb(0);
}

extern "C" K reqtime(K ignore){
	if(client.isConnected()) {
		client.reqCurrentTime();
		while( client.isConnected()) {
			client.processMessages();
	
			if(finish==1) { 
				finish=0;
				char * str = new char[curtimestr.size() + 1];
				std::copy(curtimestr.begin(), curtimestr.end(), str);
				str[curtimestr.size()] = '\0';
				return ks(ss(str));
			}
		}
	}
	
	return 0;
}

extern "C" K reqids(K ignore){
	if(client.isConnected()) {
		client.reqids();
		while( client.isConnected()) {
			client.processMessages();
	
			if(finish==1) { 
				finish=0;
				return ki(nextid);
			}
		}
	}
	
	return 0;
}

extern "C" K placeorder(K contr){
		Contract c;
		Order o;
		std::string auxprc,lmtprc;
		auxprc=kS(contr)[9];
		lmtprc=kS(contr)[10];
	if(client.isConnected()) {
		c.symbol =   		kS(contr)[0]; //"AAPL";
		c.exchange = 		kS(contr)[1]; //"SMART";
		c.currency = 		kS(contr)[2]; //"USD";
		c.secType =  		kS(contr)[3]; //"STK";
		o.transmit  = (bool)std::stoi(kS(contr)[4]);//true
		o.orderId = std::stoi(kS(contr)[5]);
		o.action =   		kS(contr)[6]; //"BUY";
		o.totalQuantity = std::stoi(kS(contr)[7]); //10
		o.orderType = 		kS(contr)[8]; //"LMT";
		o.auxPrice = 	auxprc.empty()?0.0:std::stof(kS(contr)[9]); //112.1;
		o.lmtPrice = 	lmtprc.empty()?0.0:std::stof(kS(contr)[10]); //112.1;
		o.tif = 			kS(contr)[11]; //"DAY";
		o.ocaGroup =	    kS(contr)[12];
		o.orderRef =	    kS(contr)[13];
		o.goodAfterTime=  kS(contr)[14]; //20161004 10:00:00;
		//ibplaceorder[enlist[`AAPL;`SMART;`USD;`STK;`1;`238;`BUY;`10;`LMT;`;`112;`DAY;`;`;`]]
		client.placeOrder(c,o);
		while( client.isConnected()) {
			client.processMessages();
	
			if(finish==1) { 
				break; 
			}
		}
	}
	finish=0;			
	return 0;
}

extern "C" K cancelorder(K orderId){
	//printf("%d\n",finish); //finish is 0 when started
	if(client.isConnected()) {
		client.cancelOrder(orderId->i);
		while( client.isConnected()) {
			client.processMessages();
	
			if(finish==1) { 
				break; 
			}
		}
	}

	return 0;
}

//extern Contract contract;

//extern "C" K reqhist(K host,K contr) {
extern "C" K reqhist(K contr) {
//dur: S(default),D,W
//step: 1 sec, 5/15/30 secs, 1 min, 2/3/5/15/30 mins, 1 hour, 1 day
//what: TRADES,MIDPOINT(forex),BID,ASK,BID_ASK(2 queries),HISTORICAL_VOLATILITY,OPTION_IMPLIED_VOLATILITY
	Contract c;
	Histstruct hs;
//	unsigned int port = 7496;
//	int clientId = 9;
//	unsigned attempt = 0;
//	finish=0; //set to 0 or next function call will have old value?
	khist=(K)0;
	//std::string str;
/*	if(contr->t<0) {  //atom
		c.symbol = contr->s;
		c.secType = "STK";
		c.exchange = "SMART";
		c.currency = "USD";	
		hs.from="20160715 00:00:00";
		hs.dur="5 D";
		hs.step="1 min";
		hs.what="TRADES";
		hs.rth=1;
		hs.date=2;
	} else {	
*/	//list
	//	printf("%s %s %s %s \n",kS(contr)[0],kS(contr)[1],kS(contr)[2],kS(contr)[3]);
	// incorrect exchange check... (cannot cancel?)
		c.symbol =   kS(contr)[0];
		c.secType =  kS(contr)[1];
		c.exchange = kS(contr)[2];
		c.currency = kS(contr)[3];
		hs.from=	  kS(contr)[4];
		hs.dur=	  kS(contr)[5];
		hs.step=     kS(contr)[6];
		hs.what=     kS(contr)[7];
		hs.rth=      std::stoi(kS(contr)[8]);
		hs.date=     std::stoi(kS(contr)[9]);
		//"20160715 00:00:00","5 D","1 min","TRADES",1,2
//	}

	if(client.isConnected()) {
		client.reqHistData(c,hs);
		while( client.isConnected()) {
			client.processMessages();
	
			if(finish==1) { 
				break; 
			}
		}
	}

	finish=0;
	return khist;
}
