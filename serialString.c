/*
 * serialString.c:
 *	Invia sulla seriale un blocco di caratteri 
 *      la seriale e' configurata fisicamente in loopback
 *      quindi viene poi riletto il blocco di caratteri
 *       
 *      Lo scopo e' avere un software di test per verificare il 
 *      funzionamento della seriale della raspberry
 */

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


static uint64_t epochMilli, epochMicro ;


/*
 * delay:
 *      Wait for some number of milliseconds
 *********************************************************************************
 */

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}



/*
 * millis:
 *      Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis (void)
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

  return (uint32_t)(now - epochMilli) ;
}





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


/*
 * serialPutchar:
 *      Send a single character to the serial port
 *********************************************************************************
 */

void serialPutchar (const int fd, const unsigned char c)
{
  write (fd, &c, 1) ;
}


/*
 * serialDataAvail:
 *      Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

int serialDataAvail (const int fd)
{
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

int serialGetchar (const int fd)
{
  uint8_t x ;

  if (read (fd, &x, 1) != 1)
    return -1 ;

  return ((int)x) & 0xFF ;
}
 


int main ()
{
  int fd ;
  int count ;
  uint8_t buff[8] = "salve\0";
  unsigned int nextTime ;

  if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }


// ciclo infinito, dove trasmette i caratteri 

  while(1){
  nextTime = millis () + 300 ;

  printf("invio: %s\n", buff);

  /// invia 5 caratteri
  for (count = 0; count < 5; count++)
	serialPutchar (fd, buff[count]);
  /// uscito dal ciclo for invia il carattere di new line (\n)
  serialPutchar(fd, '\n');

  /// attende 5 ms
  nextTime = millis () + 10 ;
  while(millis() < nextTime);
  
  printf("ricevo: ");
  while (serialDataAvail (fd)){
      /// legge un carattere alla volta
      //printf (" -> %3d", serialGetchar (fd)) ;
      /// e li stamap in codifica ascii
      printf ("%c", serialGetchar (fd)) ;
      fflush (stdout) ;
    }
 
  /// e si ferma per 1 secondo
  sleep(1);
  /// poi ripete l'invio.
  }
  printf ("\n") ;
  return 0 ;
}


