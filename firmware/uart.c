/* 
 * This file is part of the uv3r firmware
 * More info at www.liorelazary.com
 * 
 * Created by Lior Elazary (KK6BWA) Copyright (C) 2013 <lior at elazary dot com> 
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA 
 */ 

#include <MC81F8816/MC81F8816.h>
#include <hms800.h>
#include "uart.h"

unsigned char PAGE1 txBuffer[SERIAL_BUFFER_SIZE];
unsigned char PAGE1 txHead=0;
unsigned char PAGE1 txTail=0;

unsigned char PAGE1 rxBuffer[SERIAL_BUFFER_SIZE];
unsigned char PAGE1 rxHead=0;
unsigned char PAGE1 rxTail=0;


void uartInit()
{
  //INIT UART
  ASIMR0  = 0xC0;     // 1100_0000b  no parity  org
  BRGCR0  = 0x3A;     // 0011_1010 BAUD_19200 @fx=8Mhz, BAUD_9600@4Mhz
  //BRGCR0  = 0x3A;     // 0011_1010 BAUD_19200 @fx=8Mhz, BAUD_9600@4Mhz
  //BRGCR0  = 0x08;     //BAUD_38400;   // ACK 
  //BRGCR0  = 0x4A;     //BAUD_9600 @fx=8Mhz,
}

unsigned char uartAvailable()
{
  return (SERIAL_BUFFER_SIZE + rxHead - rxTail) % SERIAL_BUFFER_SIZE;
}


short uartRead()
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (rxHead == rxTail)
  {
    return -1;
  } else {
    unsigned char c = rxBuffer[rxTail];
    rxTail = (unsigned char)(rxTail+1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

void uartWrite(unsigned char c)
{

  unsigned char i = (txHead +1) % SERIAL_BUFFER_SIZE;
  while(i == txTail)
    ;

  txBuffer[txHead] = c;
  RXBR = rxBuffer[rxHead];
  txHead = i;

}


void uartIntHandler(void)
{
  if (ASISR0)
  {
    uartInit(); 
  } else 
  { 
    if(IFRX0)
    {
      IFRX0 = 0;
      unsigned char i = (rxHead + 1) % SERIAL_BUFFER_SIZE;
      if (i != rxTail)
      {
        rxBuffer[rxHead] = RXBR;
        rxHead = i;
      }
    }

    if(IFTX0) 
    {
      IFTX0 = 0;
      if (txHead != txTail)
      {
        TXSR = txBuffer[txTail];
        txTail = (txTail + 1) % SERIAL_BUFFER_SIZE;
      }
    } 
  }   
}

void uartSendMsg(char* str)
{
  while(*str != 0)
    uartWrite(*str++);
}

void uartSendNum(char num)
{
  uartWrite('0' + num);
}


