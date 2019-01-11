/****************************************************************************
 *  Copyright (C) 2018 Ken Lindsay
 *
 *  This ;lbrary is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/
/*******************************************************************************
 * Class for getting single line of keyboard text terminated by ENTER/RETURN key
 * as a non blocking thread.
 * Sample the data ready state by using the polling function getReadyState()
 * If the return value is true then their is a lin of text available for reading.
 * The function getLine() will return the keyboard input string.
 * **********************************************************************************/
#include <string.h>
#include <unistd.h>
#include "pr3_io_sim.h"

#define PR3_BUFFER_SIZE      256
#define PR3_BUFFER_MASK      (PR3_BUFFER_SIZE - 1)

uint8_t   _pr3RxBuffer[PR3_BUFFER_SIZE];
uint32_t  _pr3RxHead = 0;
uint32_t  _pr3RxTail = 0;
uint8_t   _pr3TxBuffer[PR3_BUFFER_SIZE];
uint32_t  _pr3TxHead = 0;
uint32_t  _pr3TxTail = 0;

// Function calls from MspTrainControl class to simulate my_stdio functions
void initializePr3IO(void)
{
    _pr3RxHead  = _pr3RxTail = 0;
    _pr3TxHead  = _pr3TxTail = 0;
}
void pr3Flush(void)
{
    _pr3RxHead = _pr3RxTail = 0;
}
int pr3RxReady(void)
{
    if(_pr3RxHead == _pr3RxTail)
        return false;
    return true;
}
void pr3Putc(char c)
{
    // Place data into Tx buffer
    _pr3TxBuffer[_pr3TxHead++] = c;
    _pr3TxHead&= PR3_BUFFER_MASK;
    while(_pr3TxHead == _pr3TxTail)
    {   // Buffer overrun
       usleep(10000);
    }
}
void pr3Puts(char *str)
{
    char c;
    while((c = *str++))
        pr3Putc(c);
}
char pr3Getch(void)
{   // Get data from Rx buffer
    char c;
    c = _pr3RxBuffer[_pr3RxTail++];
    _pr3RxTail&= PR3_BUFFER_MASK;
    return c;
}
char *pr3Gets_n(char *str, int numChars)
{
    int i, in;
    for(i = 0; i<numChars; i++)
    {
        in = pr3Getch();
        if(in == '\n' || in == '\r' || in == 0x1b)
            break;
        str[i] = in;
    }
    str[i] = '\0';
    return str;
}

// Function calls from SerialIoClass
void _pr3Flush(void)
{   // This is the buffer connected to SerialIoClass read
    _pr3RxHead  = _pr3RxTail = 0;
    _pr3TxHead  = _pr3TxTail = 0;
}
int _pr3Ready(void)
{   // Data from PR3 is placed in the Tx buffer
    unsigned nBytes;
    nBytes = _pr3TxHead - _pr3TxTail;
    nBytes&= PR3_BUFFER_MASK;
    return (int)nBytes;
}
char _pr3Getch(void)
{   // Buffer to be read by SerialIoClass
    char c;
    while(_pr3TxHead == _pr3TxTail)
        usleep(10000);
    c = _pr3TxBuffer[_pr3TxTail++];
    _pr3TxTail&= PR3_BUFFER_MASK;
    return c;
}
void _pr3Putch(char c)
{
    _pr3RxBuffer[_pr3RxHead++] = (uint8_t)c; // Load buffer
    _pr3RxHead&= PR3_BUFFER_MASK;
    while(_pr3RxHead == _pr3RxTail)
    {   // Buffer Overrun
        usleep(10000);
    }
}
