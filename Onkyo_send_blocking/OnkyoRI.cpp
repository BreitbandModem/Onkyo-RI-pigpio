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
  delayMicroseconds(3000);
  gpioWrite(_outputPin,0);
  delayMicroseconds(1000);
}

/// write message bit
///
/// \param level  requested bit level (0/1)
///
void OnkyoRI::writeBit(bool level)
{
  gpioWrite(_outputPin,1);
  delayMicroseconds(1000);
  gpioWrite(_outputPin,0);

  if(level)
    delayMicroseconds(2000);
  else
    delayMicroseconds(1000);
}

/// write message footer
void OnkyoRI::writeFooter()
{
  gpioWrite(_outputPin,1);
  delayMicroseconds(1000);
  gpioWrite(_outputPin,0);
  delayMicroseconds(20000);
}

void OnkyoRI::delayMicroseconds(int micros)
{
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = micros * 1000;

  nanosleep(&tim , &tim2);
}
