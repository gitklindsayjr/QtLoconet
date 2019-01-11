/****************************************************************************
    Copyright (C) 2002,2003,2004 Alex Shepherd
    Copyright (C) 2017 Ken Lindsay

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************

 Acknowledgment:   Alex Shepherd <kiwi64ajs@sourceforge.net>
 Title :   LocoNet Infrared TV/VCR Remote controlled Throttle
 Date:     4-Dec-2004
 Software: AVR-GCC
 Target:   AtMega8

 DESCRIPTION
    This project is a LocoNet throttle which re-worked for an linux xterm 16.04
    Console/Keyboard.  The original source was an Arduino sketch, converted to
    a GCC compiled c++ application. The development environment used is:
    Eclipse IDE For C/C++ Developers, Version: Neon.3 Release (4.6.3)

*****************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include <stdint.h>
#include <get_key_class.h>
#include "ln_throttle.h"

uint16_t locoAddr = 0;
lnMsg    *rxPacket ;
uint32_t lastTimerTick;
bool     exitFlag = false;
bool     okToExit = false;
void loop(void); // loop() prototype

#define FORWARD           true
#define REVERSE           false

LocoNetThrottle  *throttle;
GetKeyClass key;

void drawStaticText(void)
{
    printf("\e[2J");   // Clear console display
    printf("\e[1;1H"); // Set to position(0,0);
    printf("LocoNet Throttle Library Demonstration Version: 1\n\n");
    printf("address    :\n\n");
    printf("speed     :\n\n");
    printf("Direction  :\n\n");
    printf("Power      :\n\n");
    printf("Functions  : 0 1 2 3 4 5 6 7 8\n\n");
    //                 1         2         3
    //        123456789012345678901234567890
    printf("Status     :\n\n");
    printf("Error      :\n\n");
    printf("Last Key   :\n\n");
    printf("Keys: A    - Acquire previously Dispatched Loco address\n");
    printf("Keys: D    - Dispatch Loco address\n");
    printf("Keys: L    - Request Loco address\n");
    printf("Keys: P    - Toggle power on/off\n");
    printf("Keys: S    - Steal Loco address\n");
    printf("Keys: Q    - Release Loco address\n");
    printf("Keys: X    - Free Loco address\n");
    printf("Keys: [    - Reduce Speed\n");
    printf("Keys: ]    - Increase Speed\n");
    printf("Keys: F    - Forward\n");
    printf("Keys: R    - Reverse\n");
    printf("Keys: T    - Toggle Direction\n");
    printf("Keys: E    - Exit this application\n");
    printf("Keys: <SP> - Stop Locomotive\n");
    printf("Keys: 0..8 - When IN_USE Toggle Functions 0..8\n");
    printf("Keys: 0..9, <BS> - When FREE Edit address\n");
    printf("\e[17;13H");
}
void notifyAddress(uint16_t address)
{
    printf("\e[3;13H");
    printf("0x%04x", address);
    printf("     "); // Erase any extra chars
    printf("\e[17;13H");
};
void notifySpeed(uint8_t speed)
{
    printf("\e[5;13H");
    printf("0x%2x", speed);
    printf("     "); // Erase any extra chars
    printf("\e[17;13H");
};
void notifyDirection(uint8_t direction )
{
    printf("\e[7;13H");
    if(direction)
        printf("Reverse");
    else
        printf("Forward");
    printf("\e[17;13H");
};
void notifyPower(uint8_t power)
{
    printf("\e[9;13H");
    if(power)
        printf("On ");
    else
        printf("Off");
    printf("\e[17;13H");
}
void notifyFunc5to8(uint8_t function, bool state)
{
    int horizPos = 14;
    horizPos+= 2*function;
    printf("\e[11;%dH", horizPos);
    if(state)
        printf("\e[1m"); // set parameters to BOLD
    printf("%d", function);
    if(state)
        printf("\e[0m"); // set parameters NORMAL
    printf("\e[17;13H");
}
void notifyFunc0to4(uint8_t function, bool state)
{
    int horizPos = 14;
    horizPos+= 2*function;
    printf("\e[11;%dH", horizPos);
    if(state)
        printf("\e[1m"); // set parameters to BOLD
    printf("%d", function);
    if(state)
        printf("\e[0m"); // set parameters NORMAL
    printf("\e[17;13H");
}
void notifyStatus(uint8_t status ){}
void notifyThState(ThState_t state )
{
    if(exitFlag && state == TH_ST_COMMON)
        okToExit = true;

    printf("\e[13;13H");
    printf("%d %s", state, throttle->getStateStr(state));
    printf("                   ");
    printf("\e[17;13H");
}
void notifyThError(ThError_t error )
{
    printf("\e[15;13H");
    printf("%d %s", error, throttle->getErrorStr(error));
    printf("                   ");
    printf("\e[17;13H");
}
bool isTime(uint32_t *timeMark, uint32_t timeInterval)
{
    struct tms tmsBuffer;
    clock_t sysTime = times(&tmsBuffer);
    unsigned long timeNow = sysTime*sysconf(_SC_CLK_TCK);
    timeNow/= 1000; // Convert to milliseconds
    if(timeNow - *timeMark >= timeInterval)
    {
        *timeMark = timeNow;
        return true;
    }
    return false;
}
int main(void)
{
    //Normally use device "/dev/ttyACMxx".  Device "/dev/loconet" is a
    // alias on my system only
    throttle = new LocoNetThrottle();
#ifndef SIMULATE
    throttle->init("/dev/loconet", 9999);
#endif
    // First initialize the LocoNet interface
    drawStaticText();
    notifyThState(TH_ST_FREE);
    throttle->myState = TH_ST_FREE;
//    throttle->initLocoNet(2);
    loop();
    return 0;
}
void loop(void)
{
    int packetSize = 0;

    // Check for any received LocoNet packets
    while(!okToExit)
    {
        rxPacket = throttle->receive(&packetSize);
#ifdef SIMULATE
        throttle->locoSimulation->locoSimulationLoop();
#endif
        if(rxPacket )
        {
            if(!throttle->processLocoNetMessage(rxPacket, packetSize) )
                throttle->processThrottleMsg(rxPacket);
        }
        if(key.getReadyState())
        {
            printf("\e[15;13H"); // Erase error message
            printf("                               ");
            printf("\e[17;13H");
            int16_t inChar = key.getkey(); // Currently requires enter key
            inChar = toupper(inChar);
            printf("\e[17;13H");
            printf("%c", inChar);
            switch(inChar)
            {
                case 'E':
                    exitFlag = true;
                    throttle->releaseAddress(true);
                    printf("\e[36;1H");
                    break;
                case 'L':
                    throttle->myAddress = locoAddr;
                    throttle->requestAddress(locoAddr);
                    break;
                case 'A':
                    throttle->recallAddress();
                    break;
                case 'D':
                    throttle->dispatchAddress();
                    locoAddr = 0;
                    break;
                case 'S':
                    throttle->stealAddress(locoAddr);
                    break;
                case 'X':
                    drawStaticText();
                    throttle->releaseAddress(false);
                    locoAddr = 0;
                    break;
                case 'Q':
                    drawStaticText();
                    throttle->releaseAddress(true);
                    locoAddr = 0;
                    break;
                case 'F':
                    throttle->setDirection(FORWARD);
                    break;
                case 'R':
                    throttle->setDirection(REVERSE);
                    break;
                case 'P':
                    throttle->setPower(!throttle->getPower());
                    break;
                case 'T':
                    if((throttle->getDirection()&DIRF_DIR) == DIRF_DIR)
                        throttle->setDirection(FORWARD);
                    else
                        throttle->setDirection(REVERSE);
                    break;
                case '[':
                    if(throttle->getSpeed() > 0 )
                        throttle->setSpeed(throttle->getSpeed() - 1);
                    break;
                case ']':
                    if(throttle->getSpeed() < 127 )
                        throttle->setSpeed(throttle->getSpeed() + 1);
                    break;
                case ' ':
                    throttle->setSpeed(0);
                    break;
                default:
                    if(throttle->myState != TH_ST_IN_USE)
                    {
                        if((inChar >= '0') && (inChar <= '9') && (locoAddr < 999) )
                        {
                            locoAddr*= 10;
                            locoAddr+= inChar - '0';
                        }
                        else if(inChar >= 7)
                            locoAddr/= 10;
                        notifyAddress(locoAddr);
                    }
                    else if((inChar >= '0') && (inChar <= '8'))
                        throttle->setFunc0to8( inChar - '0', !throttle->getFunc0to8(inChar - '0'));
                    break;
            }
        }
        fflush(stdout);  // Required for printf updates
        usleep(10000);
    }
    printf("\e[36;1H");
    fflush(stdout);
}
