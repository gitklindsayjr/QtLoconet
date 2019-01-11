
/****************************************************************************
 * 	Copyright (C) 2009 to 2013 Alex Shepherd
 * 	Copyright (C) 2013 Damian Philipp
 * 	Copyright (C) 2018 Ken Lindsay
 * 
 * 	Portions Copyright (C) Digitrax Inc.
 * 	Portions Copyright (C) Uhlenbrock Elektronik GmbH
 * 
 * 	This library is free software; you can redistribute it and/or
 * 	modify it under the terms of the GNU Lesser General Public
 * 	License as published by the Free Software Foundation; either
 * 	version 2.1 of the License, or (at your option) any later version.
 * 
 * 	This library is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * 	Lesser General Public License for more details.
 * 
 * 	You should have received a copy of the GNU Lesser General Public
 * 	License along with this library; if not, write to the Free Software
 * 	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *****************************************************************************
 * 
 * 	IMPORTANT:
 * 
 * 	Some of the message formats used in this code are Copyright Digitrax, Inc.
 * 	and are used with permission as part of the MRRwA (previously EmbeddedLocoNet) project.
 *  That permission does not extend to uses in other software products. If you wish
 * 	to use this code, algorithm or these message formats outside of
 * 	MRRwA, please contact Digitrax Inc, for specific permission.
 * 
 * 	Note: The sale any LocoNet device hardware (including bare PCB's) that
 * 	uses this or any other LocoNet software, requires testing and certification
 * 	by Digitrax Inc. and will be subject to a licensing agreement.
 * 
 * 	Please contact Digitrax Inc. for details.
 * 
 *****************************************************************************
 * 
 * 	IMPORTANT:
 * 
 * 	Some of the message formats used in this code are Copyright Uhlenbrock Elektronik GmbH
 * 	and are used with permission as part of the MRRwA (previously EmbeddedLocoNet) project.
 *  That permission does not extend to uses in other software products. If you wish
 * 	to use this code, algorithm or these message formats outside of
 * 	MRRwA, please contact Copyright Uhlenbrock Elektronik GmbH, for specific permission.
 * 
 *****************************************************************************
 * 	DESCRIPTION
 * 	This module provides functions that manage the sending and receiving of LocoNet packets.
 * 	
 * 	As bytes are received from the LocoNet, they are stored in a circular
 * 	buffer and after a valid packet has been received it can be read out.
 * 	
 * 	When packets are sent successfully, they are also appended to the Receive
 * 	circular buffer so they can be handled like they had been received from
 * 	another device.
 * 
 * 	Statistics are maintained for both the send and receiving of packets.
 * 
 * 	Any invalid packets that are received are discarded and the stats are
 * 	updated approproately.
 * 
 *****************************************************************************/
#ifndef LOCONET_INCLUDED
#define LOCONET_INCLUDED

#ifdef QT_BUILD
    #include <QWidget>
#endif
#include "ln_buf.h"
#include "ln_opc.h"
#include "serial_io_class.h"
#include "loco_sim.h"

#define THIS_ID              9999

// Callbacks
extern void notifySensor(uint16_t address, uint8_t state ) __attribute__ ((weak));
// Address: Switch Address.
// Output: Value 0 for Coil Off, anything else for Coil On
// Direction: Value 0 for Closed/GREEN, anything else for Thrown/RED
// state: Value 0 for no input, anything else for activated
// Sensor: Value 0 for 'Aux'/'thrown' anything else for 'switch'/'closed'
extern void notifySwitchRequest(uint16_t address,uint8_t direction) __attribute__ ((weak));
extern void notifySwitchReport(uint16_t address, uint8_t output, uint8_t direction) __attribute__ ((weak));
extern void notifySwitchOutputsReport(uint16_t address, uint8_t closedOutput, uint8_t thrownOutput) __attribute__ ((weak));
extern void notifySwitchState(uint16_t address, uint8_t direction) __attribute__ ((weak));
// Power management, Transponding and Multi-Sense Device info Call-back functions
extern void notifyMultiSenseTransponder( uint16_t address, uint8_t zone, uint16_t locoAddress, uint8_t present) __attribute__ ((weak));
extern void notifyMultiSensePower( uint8_t boardID, uint8_t subdistrict, uint8_t mode, uint8_t direction) __attribute__ ((weak));
extern void notifyPower(uint8_t power) __attribute__ ((weak));

class LocoNet
{
    private:
        bool    verbose;
        LnBuf   lnBuffer;
    public:
    #ifdef PR3_SIM
        LocoSimulation *locoSimulation;
    #endif
    public:
        LocoNet(void);
        ~LocoNet();
        SerialIoClass *port;
        bool    error;
        uint8_t myPower;
        uint8_t  getPower(void){ return myPower; }
        lnMsg    *receive(int *packetSize);
        void setPower(uint8_t value);
        void init(uint8_t userData, uint8_t options, uint16_t throttleId );
        void init(std::string devStr, bool verbose);
        void init(std::string ttyDevice);
        void send(uint8_t opCode); // Two byteF7_F13
        void send(uint8_t opCode, uint8_t data1, uint8_t data2); // 4 byte
        void send(uint8_t opCode, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4); // 6 byte
        void send(lnMsg *txPacket);
        bool processLocoNetMessage(lnMsg *lnPacket, int packetSize);
        bool switchState(uint16_t address, uint8_t direction);
        LnBufStats* getStats(void);
    private: // Updates
        void sendLongAck(uint8_t ucCode);
        void requestSwitch(uint16_t Address, uint8_t output, uint8_t direction) ;
        void reportSwitch(uint16_t address) ;
        void reportSensor(uint16_t address, uint8_t state) ;
        void updatePower(uint8_t powerState);
};
#endif
