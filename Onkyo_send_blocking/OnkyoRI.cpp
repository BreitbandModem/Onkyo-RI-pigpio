//---------------------------------------------------------------------------
//
// Name:        OnkyoRI.cpp
// Author:      Vita Tucek
// Created:     19.1.2016
// License:     MIT
// Description: Library for control Onkyo devices with RI port.
//              Message is composed from header, 12 databits and footer.
//
//              For timing delay() function is used. That means until send
//              is completed program is blocked (max send() duration is 61ms).
//
//---------------------------------------------------------------------------


#include <time.h>
#include <sys/time.h>
#include <pigpio.h>
#include "OnkyoRI.h"

/// send command message to device
///
/// \param command  command to device
///
void OnkyoRI::send(int command)
{
  writeHeader();

  for(int i=0;i<12;i++)
  {
    bool level = command & 0x800;
    command <<= 1;
    writeBit(level);
  }

  writeFooter();
}

/// write message header
void OnkyoRI::writeHeader()
{
  gpioWrite(_outputPin,1);
  gpioDelay(3000);
  gpioWrite(_outputPin,0);
  gpioDelay(1000);
}

/// write message bit
///
/// \param level  requested bit level (0/1)
///
void OnkyoRI::writeBit(bool level)
{
  gpioWrite(_outputPin,1);
  gpioDelay(1000);
  gpioWrite(_outputPin,0);

  if(level)
    gpioDelay(2000);
  else
    gpioDelay(1000);
}

/// write message footer
void OnkyoRI::writeFooter()
{
  gpioWrite(_outputPin,1);
  gpioDelay(1000);
  gpioWrite(_outputPin,0);
  gpioDelay(20000);
}

// The following code is taken from the original wiringPi project:
// https://github.com/WiringPi/WiringPi/blob/f97a6230160b819e6daea7eb242404afa708e421/wiringPi/wiringPi.c#L2127
/*
 * delayMicroseconds:
 *	This is somewhat intersting. It seems that on the Pi, a single call
 *	to nanosleep takes some 80 to 130 microseconds anyway, so while
 *	obeying the standards (may take longer), it's not always what we
 *	want!
 *
 *	So what I'll do now is if the delay is less than 100uS we'll do it
 *	in a hard loop, watching a built-in counter on the ARM chip. This is
 *	somewhat sub-optimal in that it uses 100% CPU, something not an issue
 *	in a microcontroller, but under a multi-tasking, multi-user OS, it's
 *	wastefull, however we've no real choice )-:
 *
 *      Plan B: It seems all might not be well with that plan, so changing it
 *      to use gettimeofday () and poll on that instead...
 *********************************************************************************
 */

void OnkyoRI::delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void OnkyoRI::delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    // subtract 100 microseconds to account for system delay
    sleeper.tv_nsec = (long)(uSecs * 1000L - 100000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}
