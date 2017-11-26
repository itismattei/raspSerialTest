/*
 * SerialComm.h
 *
 *  Created on: 18/apr/2017
 *      Author: massimo
 *
 *  Realizza le funzione di basso livello per gestire la UART su raspberry PI
 *  e le ingloba nella classe SerialComm.
 *  I medtodi sono writeBuff che scrive un buffer della classe di almeno di un carattere
 *  e readBuff che legge uno o più caratteri ponendoli nel buffer di ricezione
 *  fornito dal chiamante dell'oggetto.
 */



#ifndef SERIALCOMM_H_
#define SERIALCOMM_H_

class SerialComm {
public:
	SerialComm();
	virtual ~SerialComm();

	int openSer();
	void serClose();
	int writeBuff(char *, int n);
	int sendBuff(int);
	int readBuff(char *);

	int  mBaud;
	int  mfd;
	int  ind;
	char mBuff[128];
};

#endif /* SERIALCOMM_H_ */
