/*
 * test_monitor.cpp
 *
 *  Created on: Sep 29, 2017
 *      Author: pops
 *  Use for monitoring DT500 loconet traffic
 */
#include <stdio.h>
#include "loco_net.h"
#include "ln_throttle.h"

void loop(void);

LocoNet locoNet;
LocoNetThrottle  *throttle;

int main(void)
{
    printf("LocoNet Monitor\n");
    // First initialize the LocoNet interface
    //Normally use device "/dev/ttyACMxx".  Device "/dev/loconet" is an
    // alias on my system only
    locoNet.init("/dev/loconet");
    fflush(stdout);
    loop();
    return 0;
}

void loop()
{
    uint8_t msgLen;
    lnMsg   *lnPacket;
    int     packetSize;

    while(true)
    {
        packetSize = 0;
        // Check for any received LocoNet packets
        lnPacket = locoNet.receive(&packetSize) ;
        if(lnPacket)
        {  // First print out the packet in HEX
            printf("RX: [ ");
            msgLen = getLnMsgSize(lnPacket);
            for (uint8_t i=0; i< msgLen; i++)
            {
                uint8_t val = lnPacket->data[i];
                printf("0x%02x ", val);
            }
            // If this packet was not a Switch or Sensor Message then print a new line
            if(!locoNet.processLocoNetMessage(lnPacket, packetSize))
                printf("]\n");
            fflush(stdout);
        }
    }
}

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Sensor messages
void notifySensor( uint16_t Address, uint8_t State )
{
    printf("Sensor: %d - ", Address);
    if(State)
        printf("Active ]\n");
    else
        printf("Inactive ]\n");
    fflush(stdout);
}

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Switch Request messages
void notifySwitchRequest( uint16_t Address, uint8_t Output, uint8_t Direction )
{
    printf("Switch Request: %d:", Address);
    if(Direction)
        printf("Closed - ");
    else
        printf("Thrown - ");
    if(Output)
        printf("On ]\n");
    else
        printf("Off ]\n");
    fflush(stdout);
}

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Switch Output Report messages
void notifySwitchOutputsReport( uint16_t Address, uint8_t ClosedOutput, uint8_t ThrownOutput)
{
    printf("Switch Outputs Report: %d: Closed - ", Address);
    if(ClosedOutput)
        printf("On: Thrown - ");
    else
        printf("Off: Thrown - ");
    if(ThrownOutput)
        printf("On ]\n");
    else
        printf("Off ]\n");
    fflush(stdout);

}

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Switch Sensor Report messages
void notifySwitchReport( uint16_t Address, uint8_t State, uint8_t Sensor )
{
    printf("Switch Sensor Report: %d:", Address);
    if(Sensor)
        printf("Switch - ");
    else
        printf("Aux - ");
    printf(" - ");
    if(State)
        printf("Active ]\n");
    else
        printf("Inactive ]\n");
    fflush(stdout);
}

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Switch State messages
void notifySwitchState( uint16_t Address, uint8_t Output, uint8_t Direction )
{
    printf("Switch State: %d:", Address);
    if(Direction)
        printf("Closed - ");
    else
        printf("Thrown - ");
    if(Output)
        printf("On ]\n");
    else
        printf("Off ]\n");
    fflush(stdout);
}




