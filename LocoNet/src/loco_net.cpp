/****************************************************************************
 * 	Copyright (C) 2009..2013 Alex Shepherd
 *	Copyright (C) 2013 Damian Philipp
 *	Copyright (C) 2018 Ken Lindsay
 * 
 * 	Portions Copyright (C) Digitrax Inc.
 *	Portions Copyright (C) Uhlenbrock Elektronik GmbH
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

#include <string.h>
#include <stdio.h>
#include <serial_io_class.h>
#include "loco_net.h"

#ifdef PR3_SIM
    #include "pr3_sim.h"
    #include "loco_sim.h"
#endif

LocoNet::LocoNet(void)
{
    verbose = false;
    myPower = 0;
    error = false;
#ifndef PR3_SIM
    port = new SerialIoClass();
#else
    locoSimulation = new LocoSimulation(this);
    port = new SerialIoClass(PR3);
#endif
}
void LocoNet::init(std::string ttyDevice)
{
    initLnBuf(&lnBuffer);
    error = port->openPort(ttyDevice, FLOW_CONTROL_CTS_E);
}
void LocoNet::init(std::string ttyDevice, bool verbose)
{
    this->verbose = verbose;
    initLnBuf(&lnBuffer);
    error = port->openPort(ttyDevice, FLOW_CONTROL_CTS_E);
}
LocoNet::~LocoNet()
{
    delete port;
}
bool LocoNet::processLocoNetMessage(lnMsg *lnPacket, int packetSize)
{
    uint16_t address;
    uint8_t  direction;
    bool     consumedFlag = true;

    if(verbose && packetSize>0)
    {
        printf("Rx: [ 0x%02x ", lnPacket->data[0]);
        for(int i = 1; i<packetSize; i++)
        {
            if((lnPacket->data[i] & OPC_BIT_SET) == OPC_BIT_SET)
                printf("]\nRx: [ 0x%02x ", lnPacket->data[i]);
            else
                printf("0x%02x ", lnPacket->data[i]);
        }
        printf("]\n");
        fflush(stdout);
    }
    address = (lnPacket->srq.sw1 | ((lnPacket->srq.sw2 & 0x0f) << 7));
    if(lnPacket->sr.command != OPC_INPUT_REP)
        address++;

    switch(lnPacket->sr.command)
    {
        case OPC_GPOFF:
            if(notifyPower)
                notifyPower(0);
            break;
        case OPC_GPON:
            if(notifyPower)
                notifyPower(1);
            break;
        case OPC_INPUT_REP: // Sensor inputs
            address<<= 1;
            if(lnPacket->ir.in2 & OPC_INPUT_REP_SW)
                address+= 2;
            else
                address+= 1;
            if(notifySensor)
                notifySensor(address, lnPacket->ir.in2 & OPC_INPUT_REP_HI);
            break;
        case OPC_SW_REQ: // Request switch function
            if(notifySwitchRequest)
                notifySwitchRequest(address, lnPacket->srq.sw2 & OPC_SW_REQ_DIR);
            break;
        case OPC_SW_REP: // Turnout sensor state report
            if(lnPacket->srp.sn2 & OPC_SW_REP_INPUTS)
            {
                if(notifySwitchReport)
                    notifySwitchReport(address, lnPacket->srp.sn2 & OPC_SW_REP_HI, lnPacket->srp.sn2 & OPC_SW_REP_SW);
            }
            else
            {
                if(notifySwitchOutputsReport)
                    notifySwitchOutputsReport(address, lnPacket->srp.sn2 & OPC_SW_REP_CLOSED, lnPacket->srp.sn2 & OPC_SW_REP_THROWN);
            }
            break;
        case OPC_SW_STATE: //Request state of switch, OPC_LONG_ACK, ACK1 = 0x7f success
            direction = lnPacket->srq.sw2 & OPC_SW_REQ_DIR ;
            if(notifySwitchState)
                notifySwitchState(address, direction ) ;
            break;
        case OPC_SW_ACK: // Request switch with acknowledge
            break ;
        case OPC_MULTI_SENSE:
            switch(lnPacket->data[1] & OPC_MULTI_SENSE_MSG)
            {
                case OPC_MULTI_SENSE_DEVICE_INFO:
                    // This is a PM42 power event.
                    /* Working on porting this */
                    if(notifyMultiSensePower)
                    {
                        uint8_t pCMD;
                        pCMD = (lnPacket->msdi.arg3 & 0xF0);
                        if((pCMD == 0x30) || (pCMD == 0x10))
                        {
                            // Autoreverse & Circuitbreaker
                            uint8_t cm1 = lnPacket->msdi.arg3;
                            uint8_t cm2 = lnPacket->msdi.arg4;
                            uint8_t mode; // 0 = AutoReversing 1 = CircuitBreaker
                            uint8_t boardID = ((lnPacket->msdi.arg2 + 1) + ((lnPacket->msdi.arg1 & 0x1) == 1) ? 128 : 0);
                            // Report 4 Sub-Districts for a PM4x
                            uint8_t d = 1 ;
                            for(uint8_t i = 1; i < 5; i++)
                            {
                                if((cm1 & d) != 0)
                                    mode = 0 ;
                                else
                                    mode = 1 ;
                                direction = cm2&d ;
                                d = d*2 ;
                                notifyMultiSensePower(boardID, i, mode, direction ) ; // BoardID, Subdistrict, Mode, Direction
                            }
                        }
                        else if(pCMD == 0x70)
                        {
                            // Programming
                        }
                        else if (pCMD == 0x00)
                        {
                            // Device type report
                        }
                    }
                    break;
                case OPC_MULTI_SENSE_ABSENT:
                case OPC_MULTI_SENSE_PRESENT:
                    // Transponding Event
                    if(notifyMultiSenseTransponder)
                    {
                        uint16_t locoaddr ;
                        uint8_t  present ;
                        char     zone ;
                        address = lnPacket->mstr.zone + ((lnPacket->mstr.type & 0x1F) << 7);
                        present = (lnPacket->mstr.type & 0x20) != 0 ? true : false;
                        address++ ;
                        if(lnPacket->mstr.adr1 == 0x7D)
                            locoaddr = lnPacket->mstr.adr2;
                        else
                            locoaddr = (lnPacket->mstr.adr1 * 128) + lnPacket->mstr.adr2;
                        if((lnPacket->mstr.zone&0x0F) == 0x00 )zone = 'A';
                        else if((lnPacket->mstr.zone&0x0F) == 0x02 )zone = 'B' ;
                        else if((lnPacket->mstr.zone&0x0F) == 0x04 )zone = 'C';
                        else if((lnPacket->mstr.zone&0x0F) == 0x06 )zone = 'D';
                        else if((lnPacket->mstr.zone&0x0F) == 0x08 )zone = 'E';
                        else if((lnPacket->mstr.zone&0x0F) == 0x0A )zone = 'F';
                        else if((lnPacket->mstr.zone&0x0F) == 0x0C )zone = 'G';
                        else if((lnPacket->mstr.zone&0x0F) == 0x0E )zone = 'H';
                        else zone = lnPacket->mstr.zone&0x0F ;
                        notifyMultiSenseTransponder(address, zone, locoaddr, present);
                        break ;
                    }
                }
                break ;
            default:
                consumedFlag = false ;
    }
    return consumedFlag ;
}
void LocoNet::setPower(uint8_t value)
{
     uint8_t opc;
    if(value)
        opc = OPC_GPON;
    else
        opc = OPC_GPOFF;
    send(opc);
    myPower = value;
}
bool LocoNet::switchState(uint16_t address, uint8_t direction)
{
    send(OPC_LONG_ACK, (0x7f & OPC_SW_STATE), direction);
    if(direction == OPC_SW_STATE_THROWN)
        send(OPC_SW_REP, address, OPC_SW_REP_THROWN);
    else
        send(OPC_SW_REP, address, OPC_SW_REP_CLOSED);
    return true;
}
void LocoNet::send(lnMsg *lnPacket)
{
    uint8_t  checkSum;
    uint8_t  checkLength;
    uint8_t  lnTxLength;
    if((lnPacket->sz.command & (uint8_t)0x60 ) == (uint8_t)0x60 )
        lnTxLength = lnPacket->sz.mesg_size;
    else
        lnTxLength = ((lnPacket->sz.command & (uint8_t)0x60 ) >> (uint8_t)4 ) + 2 ;
    // First calculate the checksum
    checkLength = lnTxLength - 1;
    checkSum = 0xff;
    for(int i=0; i<checkLength; i++)
        checkSum^= lnPacket->data[i];
    lnPacket->data[checkLength] = checkSum;
#ifdef PR3_SIM
    locoSimulation->locoRemapMsg(lnPacket, lnTxLength);
#else
    port->write_n(lnPacket->data, lnTxLength);
#endif
    if(verbose)
    {
        printf("Tx: [ ");
        for(unsigned i = 0; i<lnTxLength; i++)
            printf("0x%02x ", lnPacket->data[i]);
        printf("]\n");
        fflush(stdout);
    }
}
void LocoNet::send(uint8_t opCode)
{
    lnMsg sendPacket ;
    sendPacket.data[ 0 ] = opCode ;
    send(&sendPacket) ;
}
void LocoNet::send(uint8_t opCode, uint8_t data1, uint8_t data2)
{
    lnMsg sendPacket;
    sendPacket.data[ 0 ] = opCode;
    sendPacket.data[ 1 ] = data1;
    sendPacket.data[ 2 ] = data2;
    send(&sendPacket);
}
void LocoNet::send(uint8_t opCode, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    lnMsg sendPacket ;
    sendPacket.data[ 0 ] = opCode ;
    sendPacket.data[ 1 ] = data1 ;
    sendPacket.data[ 2 ] = data2 ;
    sendPacket.data[ 3 ] = data3 ;
    sendPacket.data[ 4 ] = data4 ;
    send(&sendPacket) ;
}
void LocoNet::sendLongAck(uint8_t ucCode)
{
    lnMsg sendPacket ;
    sendPacket.data[ 0 ] = OPC_LONG_ACK;
    sendPacket.data[ 1 ] = OPC_PEER_XFER-0x80;
    sendPacket.data[ 2 ] = ucCode;
    send(&sendPacket) ;
}
void LocoNet::requestSwitch(uint16_t address, uint8_t output, uint8_t direction)
{
    uint8_t addrH = (--address >> 7) & 0x0F;
    uint8_t addrL = address & 0x7F;

    if(output)
        addrH|= OPC_SW_REQ_OUT;
    if(direction)
        addrH|= OPC_SW_REQ_DIR;
    send(OPC_SW_REQ, addrL, addrH) ;
}
void LocoNet::reportSwitch(uint16_t address)
{
    address-= 1;
    return send(OPC_SW_STATE, (address & 0x7F), ((address >> 7) & 0x0F));
}
lnMsg*  LocoNet::receive(int *packetSize)
{   // Called from main()
    uint8_t newByte = 0;
    if(port->readReady(10)) // Read wait time 10ms
    {
        int n_bytes = port->bytesAvailable;
        for(int i=0; i<n_bytes; i++)
        {
            int n_read = port->read_n(&newByte, 1);
            if(n_read == 0)
            {   // Never get here unless no data read
                if(verbose)
                    printf("Error: Failed read\n");
                n_bytes = 0;
                break;
            }
            // static addByteLnBuf(&LnBuffer, newByte);
            lnBuffer.buf[lnBuffer.writeIndex++] = newByte;
            if(lnBuffer.writeIndex >= LN_BUF_SIZE)
                lnBuffer.writeIndex = 0 ;
        }
        *packetSize = n_bytes;
    }
    return recvLnMsg(&lnBuffer);
}
LnBufStats *LocoNet::getStats(void)
{
    return &lnBuffer.stats;
}

