/*
 * TxRxcmd.cpp
 *
 *  Created on: 20/apr/2017
 *      Author: massimo
 */

#include <iostream>
using namespace std;
#include "TRcmd.h"
#include <stdlib.h>
#include <stdio.h>

int TxRxcmd::numSer = 0;

TxRxcmd::TxRxcmd() {
	// TODO Auto-generated constructor stub
	scPtr = NULL;
	CHS = 0xA9;
	if (numSer == 0){
		numSer++;
		/// si ricorda che puo' funzionare
		ID = 1;
	}
	else
		ID = 0;
	isOK = false;
}

TxRxcmd::~TxRxcmd() {
	// TODO Auto-generated destructor stub
	ID = 0;
	scPtr = NULL;
}


/// invio del comando con solo carattere
void TxRxcmd::sendCmd(char c){

	int def_ = 0;
	/// per questo ripo di comandi il byte n. 1 e' sempre pari a '0'
	txBuff[1] = '0';
	//CHS ^= '0';
	/// terminatore di stringa
	txBuff[4] = '\0';

	switch(c){
	case 'F':
		txBuff[0] = 'F';
		txBuff[2] = 'F' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	case 'B':
		txBuff[0] = 'B';
		txBuff[2] = 'B' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	case 'I':
		txBuff[0] = 'I';
		txBuff[2] = 'I' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	case 'S':
		txBuff[0] = 'S';
		txBuff[2] = 'S' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	case 'R':
		txBuff[0] = 'R';
		txBuff[2] = 'R' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	case 'L':
		txBuff[0] = 'L';
		txBuff[2] = 'L' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	case 'P':
		txBuff[0] = 'P';
		txBuff[2] = 'P' ^ CHS ^ '0';
		txBuff[3] = '*';
	break;

	default:
		cout << "comando non valido. " << endl;
		def_ = 1;
	break;
	}

	if (scPtr != NULL && def_ == 0){
//		cout << "scritto txbuff" << endl;
//		cout << txBuff[0] << endl;
//		cout << txBuff[3] << endl;
		scPtr->writeBuff(txBuff, 4);
	}
}

///
/// comando da 2 caratteri
/// ad esempio la lettura di un sensore
void TxRxcmd::sendCmd(char c, int num){

	int def_ = 0;

	switch(c){
		case 'G':
			txBuff[0] = 'G';
			txBuff[1] = num & 0xFF;
			txBuff[2] = 'G' ^ CHS ^ txBuff[1];
			txBuff[3] = '*';

		break;

		case 'D':
			txBuff[0] = 'D';
			txBuff[1] = num & 0xFF;
			txBuff[2] = 'D' ^ CHS ^ txBuff[1];
			txBuff[3] = '*';
		break;

		default:
			def_ = 1;
		break;

	}

	/// scrive se il puntatore alla classe SerailComm e' definito 
	/// e il comando  e' 'G' oppure 'D'
	if (scPtr != NULL && def_ == 0){
		scPtr->writeBuff(txBuff, 4);
	}
}

///
/// si connette alla seriale
void TxRxcmd::connect(SerialComm * p){

	if (ID == 0)
		/// non puo' avere la seriale perché l'oggetto non e' stato inzializzato
		return;
	if (p != NULL){
		scPtr = p;
		/// apre la seriale
		scPtr->openSer();
		if (scPtr->mfd != -1){
			isOK = true;
#ifdef _DEBUG_SERIAL_
			printf("aperta la seriale\n");
#endif
		}
		else
			isOK = false;
	}
}

///
/// ricezione dati dalla seriale
int TxRxcmd::receiveCmd(void){
	int num, STATO = 0, i0 = 0, reply = 0;
	char locBuff[128];

	while (num = scPtr->readBuff(rxBuff)){
		if (STATO == 1){
			for (int i = i0; i <= 5; i++)
				locBuff[i] = rxBuff[ i - i0];
			STATO = 2;
		}

		/// ho ricevuto meno di 5bytes e devo aspettare un po'
		if (num < 5 && STATO == 0){
			int i;
			/// prima volta che attendo
			for(i = 0; i <= num; i++)
				locBuff[i] = rxBuff[i];
			STATO = 1;
			i0 = i;
			/// breve attesa
			for (volatile int i = 1000000; i > 0; i--);
		}
		else
			if (STATO == 0 && num == 5){
				/// copia in locBuff i contenuto di rxBuff
				for (int i = 0; i < 5; i++){
					locBuff[i] = rxBuff[i];
/// per scopi di debug
#ifdef _DEBUG_SERIAL_
///					cout << (int) locBuff[i] << endl;
#endif
				}
//				cout << "copiati 5 bytes" << endl;
				STATO = 3;
			}

	//	cout << "bytes letti " << num << " stato " << STATO << endl;
	}



	if (STATO > 0){
		/// dovrebbe aver copiato i 4 bytes e controlla il checksum
		unsigned int checksum = locBuff[0] ^ locBuff[1] ^ locBuff[2] ^ CHS;
		if ( checksum == locBuff[3] && locBuff[4] == '*'){
			/// la ricezione e' corretta
			isOK = true;
#ifdef _DEBUG_SERIAL_
			cout << "messaggio ricevuto ok" << endl;
#endif
		}
		else{
			isOK = false;
#ifdef _DEBUG_SERIAL_
			cout << "messaggio ricevuto errato" << endl;
#endif
		}

	}
	else{
#ifdef _DEBUG_SERIAL_
			cout << "mancanza di comunicazione" << endl;
#endif		
		isOK = false;
	}

	return (int)isOK;

}
