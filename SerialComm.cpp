/*
 * SerialComm.cpp
 *
 *  Created on: 18/apr/2017
 *      Author: massimo
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

/// 
/// funzioni di base per l'uso della UART
///
int serialOpen (const char *device, const int baud)
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case     50:        myBaud =     B50 ; break ;
    case     75:        myBaud =     B75 ; break ;
    case    110:        myBaud =    B110 ; break ;
    case    134:        myBaud =    B134 ; break ;
    case    150:        myBaud =    B150 ; break ;
    case    200:        myBaud =    B200 ; break ;
    case    300:        myBaud =    B300 ; break ;
    case    600:        myBaud =    B600 ; break ;
    case   1200:        myBaud =   B1200 ; break ;
    case   1800:        myBaud =   B1800 ; break ;
    case   2400:        myBaud =   B2400 ; break ;
    case   4800:        myBaud =   B4800 ; break ;
    case   9600:        myBaud =   B9600 ; break ;
    case  19200:        myBaud =  B19200 ; break ;
    case  38400:        myBaud =  B38400 ; break ;
    case  57600:        myBaud =  B57600 ; break ;
    case 115200:        myBaud = B115200 ; break ;
    case 230400:        myBaud = B230400 ; break ;

    default:
      return -2 ;
  }
  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] =  50 ;        // five seconds (50 deciseconds)

  tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;      // 10mS

  return fd ;
}

///
/// chiude la comunicazione seriale
///
void serialClose(int fd){
	close(fd);
}

///
/// scrive un carattere su seriale
///
void serialPutchar (const int fd, const unsigned char c){
	write (fd, &c, 1) ;
}



///
/// verifica la presenza di caratteri da leggere
///
int serialDataAvail (const int fd){
	
	int result ;
	
	if (ioctl (fd, FIONREAD, &result) == -1)
	return -1 ;
	
	return result ;
}

/*
 * serialGetchar:
 *      Get a single character from the serial device.
 *      Note: Zero is a valid character and this function will time-out after
 *      5 seconds.
 *********************************************************************************
 */

int serialGetchar (const int fd){
  
	uint8_t x ;

	if (read (fd, &x, 1) != 1)
	return -1 ;
	
	return ((int)x) & 0xFF ;
}



using namespace std;

#include "SerialComm.h"

SerialComm::SerialComm() {
	// TODO Auto-generated constructor stub

	mBaud = 115200;
	ind = 0;
	mfd = -1;
}

SerialComm::~SerialComm() {
	// TODO Auto-generated destructor stub
	serClose();
}

///
/// apre la seriale: ttyAM0
int SerialComm::openSer(){

	if ((mfd = serialOpen ("/dev/ttyAMA0", mBaud)) < 0){
		printf("Unable to open serial device. \n");
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		mfd = -1 ;
	}
	else{
		printf("aperta UART\n");
	}

	return mfd;
}

///
///
/// scrive N byte sul canale seriale
int SerialComm::writeBuff(char * buff, int num){

	int fd;
	if (num > 127){
		/// limita il buffer a 127 caratteri
		mBuff[127] = '\0';
		num = 127;
	}

	strncpy(mBuff, buff, num);
	mBuff[num] = '\0';
//	cout << "copiati " << num << "bytes" << endl;
//	cout << mBuff[0] << endl;
//	cout << mBuff[3] << endl;
//	cout << buff[0] << endl;
//	cout << buff[3] << endl;
	fd = sendBuff(num);
	return num;
}


///
/// legge N byte dal canale e restituisce il numero di bytes effettivamente letti
int SerialComm::readBuff(char *buff){
	ind = 0;
	/// interroga la seriale sulla disponibilita' di bytes
	while (serialDataAvail (mfd)){
		//printf("ricevuto \n");
		//printf (" -> %3d", serialGetchar (fd)) ;
		mBuff[ind] = serialGetchar(mfd);
		//printf("%c", mBuff[ind]);
		fflush (stdout) ;
		ind++;
		/// modulo 128
		ind &= 0x7F;
		if (ind == 0){
			/// nel caso in cui raggiungesse la fine del buffer verra' troncato
			/// l'ultimo byte
			mBuff[127] = '\0';
			strncpy(buff, mBuff, 127);
		}
		else{
			mBuff[ind] = '\0';
			/// inserita questa,perche' se una lettura del sensore ha un byte a 0,
			/// la strcpy si arresta nella copia ed il dato inviato e' sbagliato
			for (int i = 0; i < ind; i++)
				buff[i] = mBuff[i];
			//strcpy(buff, mBuff);
		}
	}
	return ind;
}

///
///
/// invia sul buffer tx della scheda
int SerialComm::sendBuff(int n){
	if (n == 1)
		serialPutchar (mfd, mBuff[0]);
	else
		if(n > 1){
			
			for (int i = 0; i < n; i++)
				/// invia n bytes sulla UART
				serialPutchar (mfd, mBuff[i]);

//			cout << "inviati " << n << "bytes" << endl;
//			cout << mBuff[0] << endl;
//			cout << "CHK " << (int) mBuff[2] << endl;
//			cout << mBuff[3] << endl;
		}
		else return -1;
	return 0;
}
//
//
/////
/////
///// chiude la seriale
void SerialComm::serClose(){

	if(mfd > 0){
		serialClose(mfd);
		printf("Seriale chiusa\n");
	}
	else
		printf("Seriale gia' chiusa.\n");
	mfd = -1;
}
