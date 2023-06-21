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


#include <pigpiod_if2.h>
#include "OnkyoRI.h"

/// send command message to device
///
/// \param command  command to device
///
void OnkyoRI::send(int command)
{
  gpio_write(_pi, _outputPin, 0);

  writeHeader();

  for(int i=0;i<12;i++)
  {
    bool level = command & 0x800;
    command <<= 1;
    writeBit(level);
  }

  writeFooter();

  pigpio_stop(_pi);
}

/// write message header
void OnkyoRI::writeHeader()
{
  gpio_write(_pi, _outputPin,1);
  delayMicroseconds(3000);
  gpio_write(_pi, _outputPin,0);
  delayMicroseconds(1000);
}

/// write message bit
///
/// \param level  requested bit level (0/1)
///
void OnkyoRI::writeBit(bool level)
{
  gpio_write(_pi, _outputPin,1);
  delayMicroseconds(1000);
  gpio_write(_pi, _outputPin,0);

  if(level)
    delayMicroseconds(2000);
  else
    delayMicroseconds(1000);
}

/// write message footer
void OnkyoRI::writeFooter()
{
  gpio_write(_pi, _outputPin,1);
  delayMicroseconds(1000);
  gpio_write(_pi, _outputPin,0);
  delayMicroseconds(20000);
}

void OnkyoRI::delayMicroseconds(unsigned int howLong)
{
  struct timespec tim, tim2;
  tim.tv_sec = 0;
  tim.tv_nsec = howLong * 1000;

  nanosleep(&tim , &tim2);
}
