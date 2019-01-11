/****************************************************************************
 *  Copyright (C) 2009..2013 Alex Shepherd
 *  Copyright (C) 2013 Damian Philipp
 *  Copyright (C) 2018 Ken Lindsay
 *
 *  Portions Copyright (C) Digitrax Inc.
 *  Portions Copyright (C) Uhlenbrock Elektronik GmbH
 *
 *  This library is free software; you can redistribute it and/or
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
 *
 *****************************************************************************
 *
 *  IMPORTANT:
 *
 *  Some of the message formats used in this code are Copyright Digitrax, Inc.
 *  and are used with permission as part of the MRRwA (previously EmbeddedLocoNet) project.
 *  That permission does not extend to uses in other software products. If you wish
 *  to use this code, algorithm or these message formats outside of
 *  MRRwA, please contact Digitrax Inc, for specific permission.
 *
 *  Note: The sale any LocoNet device hardware (including bare PCB's) that
 *  uses this or any other LocoNet software, requires testing and certification
 *  by Digitrax Inc. and will be subject to a licensing agreement.
 *
 *  Please contact Digitrax Inc. for details.
 *
 *****************************************************************************
 *
 *  IMPORTANT:
 *
 *  Some of the message formats used in this code are Copyright Uhlenbrock Elektronik GmbH
 *  and are used with permission as part of the MRRwA (previously EmbeddedLocoNet) project.
 *  That permission does not extend to uses in other software products. If you wish
 *  to use this code, algorithm or these message formats outside of
 *  MRRwA, please contact Copyright Uhlenbrock Elektronik GmbH, for specific permission.
 *
 *****************************************************************************
 *  DESCRIPTION
 *  This module provides functions that manage the sending and receiving of LocoNet packets.
 *
 *  As bytes are received from the LocoNet, they are stored in a circular
 *  buffer and after a valid packet has been received it can be read out.
 *
 *  When packets are sent successfully, they are also appended to the Receive
 *  circular buffer so they can be handled like they had been received from
 *  another device.
 *
 *  Statistics are maintained for both the send and receiving of packets.
 *
 *  Any invalid packets that are received are discarded and the stats are
 *  updated approproately.
 *
 *****************************************************************************/
#include <unistd.h>
#include "loco_net.h"
#include "ln_throttle.h"
#include "ln_opc.h"

#ifdef PR3_SIM
    #include "pr3_sim.h"
    #include "loco_sim.h"
    extern LocoNetThrottle *throttle;
#endif

LocoNetThrottle::LocoNetThrottle(void)
{
#ifdef PR3_SIM
    throttle = this; // Initialize the simulation this pointer
#endif
    mySpeedSteps = TH_SP_ST_128;
    myState = TH_ST_INIT;
    mySpeed = 0;    // Stopped
    mySixByteDir = SIX_BYTE_SPD_REV;
    mySixByte0to6 = 0;
    mySixByte7to13 = 0;
    mySixByte14to20 = 0;
    mySixByte21to27 = 0;
    mySixByte28 = 0;
    myFunc9to12 = 0;
    myFunc13to20 = 0;
    myFunc13to20 = 0;
    myFunc21to28 = 0;
    myId = THIS_ID;      // Id of throttle
    mySlot = 0;          // Start with this slot
    myAddress = 0;       // Decoder Address
    myStatus1 = 0;       // Stat1
    myStatus2 = 0;       // Stat2
    myDirFunc0to4 = 0;   // Direction
    myFunc5to8 = 0;
}
LocoNetThrottle::~LocoNetThrottle(){}
void LocoNetThrottle::processThrottleMsg(lnMsg *lnPacket)
{
    int slot;
    int status1; // Process stat1
    int address;
    int id;
    int cmd = lnPacket->data[0];
    switch(cmd)
    {
        case OPC_RQ_SL_DATA:
            slot = lnPacket->sd.slot;
            if(slot == 0x7f) // Request from master for slot requests
                send(OPC_RQ_SL_DATA, 0, 0);
            break;
        case OPC_SL_RD_DATA:
            slot = lnPacket->sd.slot;
            status1 = lnPacket->sd.stat; // Process stat1
            address = (lnPacket->sd.adr2<<7) + lnPacket->sd.adr;
            id = (lnPacket->sd.id2<<7) + lnPacket->sd.id1;
            switch(myState)
            {
                case TH_ST_INIT:
                    updateSpeed(0);
                    myDirFunc0to4 = ~lnPacket->sd.dirf;
                    myDirFunc0to4&= DIRF_MASK;
                    updateDirAndFunc0to4(lnPacket->sd.dirf);
                    updateAddress(address);
                    updateId(id, false);
                    updateSlot(slot, false);
                    switch(status1 & LOCOSTAT_MASK)
                    {
                        case LOCO_IN_USE:
                            if(address == myAddress)
                            {
                                updateId(id, true);
                                updateSlot(slot, true);
                            }
                            updateThState(TH_ST_IN_USE); break;
                        case LOCO_IDLE:
                            updateThState(TH_ST_IDLE); break;
                        case LOCO_COMMON:
                            updateThState(TH_ST_COMMON); break;
                        case LOCO_FREE:
                            updateThState(TH_ST_FREE); break;
                        default:
                            updateThState(TH_ST_UNKNOWN); break;
                    }
                    setPower(true);
                    updateStatus1(status1);
                    break;
                case TH_ST_REQUEST: // Response to throttle address request
                    if(myAddress == address)
                    {   // This is for my address OK
                        if((status1 & STAT1_SL_CONUP) == 0 && (status1 & LOCO_IN_USE) != LOCO_IN_USE)
                        {  // Not CONSIST, not IN_USE therefore COMMON, IDLE, or NEW
                            updateSpeed(lnPacket->sd.spd);
                            updateDirAndFunc0to4(lnPacket->sd.dirf);
                            updateFunc5to8(lnPacket->sd.snd);
                            updateThState(TH_ST_IN_USE);
                            status1|= (LOCO_IN_USE | DEC_MODE_128);
                            lnPacket->sm.command = OPC_MOVE_SLOTS; // Do a null move
                            lnPacket->sm.src = slot;
                            lnPacket->sm.dest = slot;
                            id = THIS_ID;
                            updateSlot(slot, true);
                            updateId(id, true);
                            send(lnPacket);
                        }
                        else if(id == THIS_ID)
                        {   // Must be IN_USE
                            id = THIS_ID;
                            updateSlot(slot, true);
                            updateId(id, true);
                            updateThState(TH_ST_IN_USE) ;
                        }
                        else
                        {
                            updateSlot(slot, false);
                            updateId(id, false);
                            if(notifyThError)
                                notifyThError(TH_ER_SLOT_IN_USE);
                        }
                    }
                    updateStatus1(status1);
                    break;
                case TH_ST_DISPATCH:
                case TH_ST_RECALL:
                    if(myAddress == address && mySlot == slot)
                    {
                        updateSlot(slot, false);
                        updateId(id, false);
                        switch(status1 & LOCOSTAT_MASK)
                        {
                            case LOCO_IN_USE:
                                updateSlot(slot, true);
                                updateId(id, true);
                                updateThState(TH_ST_IN_USE);
                                break;
                            case LOCO_IDLE:
                            case LOCO_COMMON:
                                updateThState(TH_ST_DISPATCH); break;
                            case LOCO_FREE:
                                updateThState(TH_ST_FREE); break;
                            default:
                                updateThState(TH_ST_UNKNOWN); break;
                        }
                        updateStatus1(status1);
                    }
                    break;
                case TH_ST_SLOT_STEAL:
                    if(myAddress == address)
                    {
                        if((status1 & STAT1_SL_CONUP) == 0 && myState != TH_ST_FREE)
                        {
                            updateSpeed(lnPacket->sd.spd);
                            updateDirAndFunc0to4(lnPacket->sd.dirf);
                            updateFunc5to8(lnPacket->sd.snd);
                            updateThState(TH_ST_IN_USE);
                            status1|= (LOCO_IN_USE | DEC_MODE_128);
                            lnPacket->sd.stat = status1;
                            lnPacket->sd.command = OPC_WR_SL_DATA;
                            lnPacket->sd.id1 = (uint8_t)(THIS_ID & 0x7F);
                            lnPacket->sd.id2 = (uint8_t)(THIS_ID >> 7);
                            id = THIS_ID;
                            updateSlot(slot, true);
                            updateId(id, true);
                            send(lnPacket);
                        }
                        else
                        {
                            if(notifyThError)
                                notifyThError(TH_ER_SLOT_FREE);
                        }
                        updateStatus1(status1);
                    }
                    break;
                case TH_ST_RELEASE:
                    updateThState(TH_ST_COMMON);
                    status1&= ~LOCO_IDLE;
                    status1|= LOCO_COMMON;
                    updateStatus1(status1);
                    updateSlot(slot, false);
                    updateId(id, false);
                    send(OPC_SLOT_STAT1, mySlot, status1);
                    break;
                default:
                    break;
            }
            break;
        case OPC_MOVE_SLOTS:
            if(myState == TH_ST_DISPATCH || myState == TH_ST_RECALL)
                send(OPC_RQ_SL_DATA, mySlot, 0);
             break;
        case OPC_LOCO_SPD:
            if(mySlot == lnPacket->lsp.slot)
                updateSpeed(lnPacket->lsp.spd);
            break;
        case OPC_LOCO_DIRF:
            if(mySlot == lnPacket->ldf.slot)
                updateDirAndFunc0to4(lnPacket->ldf.dirf);
            break;
        case OPC_LOCO_SND:
            if(mySlot == lnPacket->ls.slot)
                updateFunc5to8(lnPacket->ls.snd);
            break;
        case OPC_SLOT_STAT1:
            if(mySlot == lnPacket->ss.stat)
                updateStatus1(lnPacket->ss.stat);
            break;
        case OPC_LONG_ACK:
            if(myState <= TH_ST_DISPATCH)
            {
                if(lnPacket->lack.opcode == (OPC_MOVE_SLOTS & 0x7F))
                    if(notifyThError)
                        notifyThError(TH_ER_NO_DISPATCH) ;
                if(lnPacket->lack.opcode == (OPC_LOCO_ADR & 0x7F))
                    if(notifyThError)
                        notifyThError(TH_ER_NO_SLOTS);
            }
            break;
        case OPC_SIX_BYTE_FUNC:
            if((lnPacket->data[2] == mySlot) && (lnPacket->data[3] == (uint8_t)(myId & 0x7f)))
            {
                uint8_t function = lnPacket->data[1];
                uint8_t value = lnPacket->data[4];
                switch(function)
                {
                    case SIX_BYTE_SPD_FWD:
                        updateSixByteSpeed(function, value);
                        break;
                    case SIX_BYTE_SPD_REV:
                     updateSixByteSpeed(function, value);
                        break;
                    case SIX_BYTE_F7_F13:
                    case SIX_BYTE_F0_F6:
                        updateSixByteFunc0to8(function, value);
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }
}
void LocoNetThrottle::init(std::string ttyDevice, uint16_t id)
{
    myId = id;
    LocoNet::init(ttyDevice);
}
void LocoNetThrottle::init(uint16_t id)
{
    myId = id;
}
bool LocoNetThrottle::initLocoNet(void)
{
    if(myState == TH_ST_INIT)
    {   // Force state machine to initialize
        send(OPC_RQ_SL_DATA, 0, 0);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_INIT) ;
    return true;

}
bool LocoNetThrottle::initLocoNet(uint16_t address)
{
    if(myState == TH_ST_INIT)
    {   // Force state machine to initialize
        updateAddress(address);
        send(OPC_LOCO_ADR, (uint8_t)(address >> 7 ), (uint8_t)(address & 0x7F));
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_INIT) ;
    return true;
}
bool LocoNetThrottle::requestAddress(uint16_t address)
{
    updateAddress(address);
    updateThState(TH_ST_REQUEST);  // State handled by response to OPC_SL_RD_DATA
    send(OPC_LOCO_ADR, (uint8_t)(address >> 7 ), (uint8_t)(address & 0x7F));
    if(myState != TH_ST_IN_USE && myState != TH_ST_INIT)
        return false; // No Error
    if(notifyThError)
    {
        ThError_t error;
        if(myState == TH_ST_INIT)
            error = TH_ER_NOT_INIT;
        else
            error = TH_ER_SLOT_IN_USE;
        notifyThError(error);
    }
    return true;
}
bool LocoNetThrottle::stealAddress(uint16_t address)
{
    if(myState != TH_ST_FREE)
    {
        updateThState(TH_ST_SLOT_STEAL);
        send(OPC_LOCO_ADR, (uint8_t) (address >> 7), (uint8_t)(address & 0x7F));
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_SLOT_FREE);
    return true;
}
bool LocoNetThrottle::dispatchAddress(void)
{
    if(myState != TH_ST_FREE)
    {
        updateThState(TH_ST_DISPATCH);
        myStatus1&= ~LOCO_IDLE;
        myStatus1|= LOCO_COMMON;
        send(OPC_SLOT_STAT1, mySlot, myStatus1);
        send(OPC_MOVE_SLOTS, mySlot, 0);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_SLOT_FREE);
    return true;
}
bool LocoNetThrottle::recallAddress(void)
{
    if(myState == TH_ST_DISPATCH)
    {
        updateThState(TH_ST_RECALL);
        send(OPC_MOVE_SLOTS, 0, 0);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_SLOT_FREE);
    return true;
}
bool LocoNetThrottle::releaseAddress(bool force)
{   // No response required
    if(myState != TH_ST_FREE || force)
    {
        if(force)
        {
            myStatus1&= ~LOCO_IDLE;
            myStatus1|= LOCO_COMMON;
            send(OPC_SLOT_STAT1, mySlot, myStatus1);
            updateThState(TH_ST_COMMON);
        }
        else
        {
            send(OPC_LOCO_ADR, (uint8_t)(myAddress >> 7), (uint8_t)(myAddress & 0x7F));
            updateThState(TH_ST_RELEASE);
        }
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_SLOT_FREE);
    return true;
}
bool LocoNetThrottle::setSixByteFunc0to8(int function, bool state)
{
    if(myState != TH_ST_IN_USE)
    {
        notifyThError(TH_ER_NOT_SELECTED);
        return true;
    }
    uint8_t myFunction = 1;
    uint8_t data;
    uint8_t range;
    uint16_t id = myId & 0x7f;
    if((function >= 0) && (function <= 6))
    {   // Toggled functions are: 0, 1, 2, 3, 4, 5, 6
        range = SIX_BYTE_F0_F6;
        data = mySixByte0to6;
        myFunction<<= function;
        if(state)
            data|= myFunction;
        else
            data&= ~myFunction;
    }
    else if((function >= 7) && (function <= 8))
    {   // Toggled functions are: 7, 8, 9, 10, 11, 12, 13
        range = SIX_BYTE_F7_F13;
        data = mySixByte7to13;
        function-= 7;
        myFunction<<= function;
        if(state)
            data|= myFunction;
        else
            data&= ~myFunction;
    }
    else
        return false;
    send(OPC_SIX_BYTE_FUNC, range, mySlot, (uint8_t)id, data);
    return true;
}
bool LocoNetThrottle::setSixByteFunc9to28(int function)
{
    if(myState != TH_ST_IN_USE)
    {
        notifyThError(TH_ER_NOT_SELECTED);
        return true;
    }
    uint8_t myFunction = 1;
    uint8_t range;
    uint16_t id = myId & 0x7f;
    if((function >= 9) && (function <= 13))
    {   // Toggled functions are: 7, 8, 9, 10, 11, 12, 13
        range = SIX_BYTE_F7_F13;
        function-= 7;
        myFunction<<= function;
        if((myFunction & mySixByte7to13) != myFunction)
            mySixByte7to13|= myFunction;
        else
            mySixByte7to13&= ~myFunction;
        myFunction = mySixByte7to13;
    }
    else if((function >= 14) && (function <= 20))
    {   // toggled functions are: 14, 15, 16, 17, 18, 20
        range = SIX_BYTE_F14_F20;
        function-= 14;
        myFunction<<= function;
        if((myFunction & mySixByte14to20) != myFunction)
            mySixByte14to20|= myFunction;
        else
            mySixByte14to20&= ~myFunction;
        myFunction = mySixByte14to20;
    }
    else if((function >= 21) && (function <= 27))
    {   // These functions are not toggled
        range = SIX_BYTE_F21_F27;
        function-= 21;
        myFunction<<= function;
        send(OPC_SIX_BYTE_FUNC, range, mySlot, (uint8_t)id, myFunction);
        myFunction = 0;
    }
    else if(function == 28)
    {
        range = SIX_BYTE_F28_XX;
        mySixByte28 = F28_NOT_USED_MASK;
        myFunction = mySixByte28;
    }
    else
        return true;
    usleep(200000);
    send(OPC_SIX_BYTE_FUNC, range, mySlot, (uint8_t)id, myFunction);
    return false;
}
bool LocoNetThrottle::setDirFunc0to4Direct(uint8_t value)
{
    value&= 0x7f; // Mask msbit
    if(myState == TH_ST_IN_USE)
    {
        send(OPC_LOCO_DIRF, mySlot, value);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
bool LocoNetThrottle::setFunc5to8Direct(uint8_t value)
{
    if(myState == TH_ST_IN_USE)
    {
        send(OPC_LOCO_SND, mySlot, value & 0x7F);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
bool LocoNetThrottle::setFunc0to8(int function, bool state)
{
    uint8_t myFunction = 1;
    uint8_t opCode ;
    uint8_t data;
    if(myState == TH_ST_IN_USE)
    {
        if(function <= 4)
        {
            opCode = OPC_LOCO_DIRF;
            data = myDirFunc0to4;
            myFunction<<= function;
            if(state)
                data|= myFunction;
            else
                data&= ~myFunction;
        }
        else
        {
            opCode = OPC_LOCO_SND;
            data = myDirFunc0to4;
            myFunction<<= (function - 5);
            if(state)
                data|= myFunction;
            else
                data&= ~myFunction;
        }
        send(opCode, mySlot, data) ;
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
bool LocoNetThrottle::setFunc9to28ShortAddr(int function)
{
    if(myState != TH_ST_IN_USE)
    {
        notifyThError(TH_ER_NOT_SELECTED);
        return true;
    }
    lnMsg msg;
    msg.sp.command = OPC_IMM_PACKET;
    msg.sp.mesg_size = IMM_BYTE_COUNT;
    msg.sp.val7f = IMM_ARG1_FIXED_VALUE;
    msg.sp.dhi = IMM_DHI_SHORT;
    msg.sp.im1 = myAddress;
    msg.sp.im3 = 0;
    msg.sp.im4 = 0;
    msg.sp.im5 = 0;

    uint8_t myFunction = 1;
    if((function >= 9) && (function <= 12))
    {   // Toggled functions are: 9, 10, 11, 12
        msg.sp.reps = (IMM_REP_REPEAT | IMM_REP_5_12_SHORT_LENGTH);
        function-= 9;
        myFunction<<= function;
        if((myFunction & myFunc9to12) != myFunction)
            myFunc9to12|= myFunction;
        else
            myFunc9to12&= ~myFunction;
        myFunction = myFunc9to12;
        msg.sp.im2 = myFunction;
        msg.sp.im2|= IM2_DCC_CMD_9_12;
        msg.sp.im2&= 0x7f;
        send(&msg);
    }
    else if((function >= 13) && (function <= 20))
    {   // toggled functions are: 13, 14, 15, 16, 17, 18, 19, 20
        msg.sp.reps = (IMM_REP_REPEAT | IMM_REP_SHORT_PKT_LENGTH);
        msg.sp.im2 = IM2_DCC_CMD_13_20;
        function-= 13;
        myFunction<<= function;
        if((myFunction & myFunc13to20) != myFunction)
            myFunc13to20|= myFunction;
        else
            myFunc13to20&= ~myFunction;
        myFunction = myFunc13to20;
        msg.sp.im3 = myFunction;
        msg.sp.im3&= 0x7f;
        if((myFunction&0x80) == 0x80)  // Check to see if bit 7 set
            msg.sp.dhi|= (1 << IMM_DHI_IM3_SHIFT); // Yes, set IM3.7 bit, in DHI bit 3 position
        send(&msg);
    }
    else if((function >= 21) && (function <= 28))
    {   // These functions are not toggled
        msg.sp.reps = (IMM_REP_REPEAT | IMM_REP_SHORT_PKT_LENGTH);
        msg.sp.im2 = IM2_DCC_CMD_21_28;
        function-= 21;
        myFunction<<= function;
        msg.sp.im3 = myFunction;
        msg.sp.im3&= 0x7f;
        if((myFunction&0x80) == 0x80)  // Check to see if bit 7 set
            msg.sp.dhi|= (1 << IMM_DHI_IM3_SHIFT); // Yes, set IM3.7 bit, in DHI bit 3 position
        send(&msg);
        usleep(200000);
        msg.sp.im3 = 0;
        send(&msg);
    }
    else
        return true; // Failed
    return false;
}
bool LocoNetThrottle::setSpeed(uint8_t speed)
{
    if(myState == TH_ST_IN_USE)
    {
        send(OPC_LOCO_SPD, mySlot, speed);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
bool LocoNetThrottle::setDirection(uint8_t direction)
{
    uint8_t dir = myDirFunc0to4;
    if(myState == TH_ST_IN_USE)
    {
        if(direction)
            dir&= ~DIRF_DIR;
        else
            dir|= DIRF_DIR;
        send(OPC_LOCO_DIRF, mySlot, dir);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
bool LocoNetThrottle::setSixByteSpeed(uint8_t speed)
{
    if(myState == TH_ST_IN_USE)
    {
        send(OPC_SIX_BYTE_FUNC, mySixByteDir, mySlot, myId, speed);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
bool LocoNetThrottle::setSixByteDir(uint8_t direction)
{
    uint8_t sixByteDir;
    if(myState == TH_ST_IN_USE)
    {
        if(direction)
            sixByteDir = SIX_BYTE_SPD_FWD;
        else
            sixByteDir = SIX_BYTE_SPD_REV;
        send(OPC_SIX_BYTE_FUNC, sixByteDir, mySlot, myId, mySpeed);
        return false;
    }
    if(notifyThError)
        notifyThError(TH_ER_NOT_SELECTED);
    return true;
}
uint8_t LocoNetThrottle::getSixByteDir(void)
{
    return myDirFunc0to4 & (uint8_t)DIRF_DIR;
}
uint8_t LocoNetThrottle::getSixByteSpeed(void)
{
    return mySpeed;
}
uint8_t LocoNetThrottle::getDirection(void)
{
    return myDirFunc0to4 & (uint8_t)DIRF_DIR;
}
uint8_t LocoNetThrottle::getSpeed(void)
{
    return mySpeed;
}
void LocoNetThrottle::updateStatus1(uint8_t status)
{
    if(myStatus1 != status)
    {
        myStatus1 = status ;
        if(notifyStatus)
            notifyStatus(status) ;
    }
}
void LocoNetThrottle::updateStatus2(uint8_t status)
{
    if(myStatus2 != status)
    {
        myStatus2 = status ;
        if(notifyStatus)
            notifyStatus(status) ;
    }
}
void LocoNetThrottle::updateThState(ThState_t state)
{
    if(myState != state)
    {
        myState = state ;
        if(notifyThState)
            notifyThState(state) ;
    }
}
void LocoNetThrottle::updateSpeed(uint8_t speed)
{
    mySpeed = speed;
    if(notifySpeed)
        notifySpeed(speed);
}
void LocoNetThrottle::updateAddress(uint16_t address)
{
    if(myAddress != address)
    {
        myAddress = address ;
        if(notifyAddress)
            notifyAddress(address);
    }
}
void LocoNetThrottle::updateId(uint16_t id, bool ours)
{
    myId = id;
    if(notifyId)
        notifyId(id, ours);
}
void LocoNetThrottle::updateSlot(uint8_t slot, bool ours)
{
    mySlot = slot;
    if(notifySlot)
        notifySlot(slot, ours) ;
}
void LocoNetThrottle::updateFunc5to8(uint8_t func5to8)
{
    if(myFunc5to8 != func5to8 )
    {
        uint8_t diffs = myFunc5to8^func5to8 ;
        myFunc5to8 = func5to8 ;
        // Check Functions 5-8
        for(uint8_t function = 5, mask = 1; function <= 8; function++ )
        {
            if(notifyFunc5to8)
                if(diffs & mask)
                    notifyFunc5to8(function, func5to8 & mask);
            mask <<= 1 ;
        }
    }
}
void LocoNetThrottle::updateDirAndFunc0to4(uint8_t dirFunc0to4)
{
    uint8_t diffs = myDirFunc0to4^dirFunc0to4;
    myDirFunc0to4 = dirFunc0to4;
    // Check Functions 1-4
    for(uint8_t function = 1, mask = 1; function <= 4; function++ )
    {
        if(notifyFunc0to4)
            if(diffs & mask)
                notifyFunc0to4(function, dirFunc0to4 & mask);
        mask <<= 1 ;
    }
    // Check Function F0
    if(notifyFunc0to4)
        if(diffs & DIRF_F0)
            notifyFunc0to4(DIRF_F0, dirFunc0to4 & (uint8_t)DIRF_F0);
    // Check Direction
    if(notifyDirection)
        notifyDirection(dirFunc0to4 & (uint8_t)DIRF_DIR);
    if((dirFunc0to4 & DIRF_DIR) == DIRF_DIR)
        mySixByteDir = SIX_BYTE_SPD_REV;
    else
        mySixByteDir = SIX_BYTE_SPD_FWD;
}
void LocoNetThrottle::updateSixByteFunc0to8(uint8_t function, uint8_t func0to8)
{
    uint8_t diffs ;
    if(function == SIX_BYTE_F0_F6)
    {
        diffs = mySixByte0to6^func0to8;
        mySixByte0to6 = func0to8;
        for(uint8_t i = 0, mask = 1; i<=6; i++ )
        {
            if(notifySixByteFunc0to8)
                if(diffs & mask)
                    notifySixByteFunc0to8(i, func0to8 & mask);
            mask <<= 1 ;
        }
    }
    else if(function == SIX_BYTE_F7_F13)
    {
        diffs = mySixByte7to13^func0to8;
        mySixByte7to13 = func0to8;
        for(uint8_t i = 7, mask = 1; i<=8; i++ )
        {
            if(notifySixByteFunc0to8)
                if(diffs & mask)
                    notifySixByteFunc0to8(i, func0to8 & mask);
            mask <<= 1 ;
        }
    }
}
void LocoNetThrottle::updateSixByteSpeed(uint8_t dir, uint8_t speed)
{
    uint8_t direction;
    mySpeed = speed;
    if(dir == SIX_BYTE_SPD_FWD)
    {
        mySixByteDir = dir;
        direction = 0;
    }
    else if(dir == SIX_BYTE_SPD_REV)
    {
        mySixByteDir = dir;
        direction = DIRF_DIR;
    }
    if(notifySpeed)
        notifySpeed(speed);
    if(notifyDirection)
        notifyDirection(direction);
}
const char *LocoNetThrottle::getStateStr(ThState_t state)
{
    switch(state)
    {
        case TH_ST_FREE:
            return "Slot free" ;
        case TH_ST_IDLE:
            return "Slot idle" ;
        case TH_ST_COMMON:
            return "Slot common" ;
        case TH_ST_REQUEST:
            return "Request" ;
        case TH_ST_RELEASE:
            return "Release" ;
        case TH_ST_DISPATCH:
            return "Dispatched" ;
        case TH_ST_RECALL:
            return "Recall" ;
        case TH_ST_SLOT_MOVE:
            return "Slot Moved" ;
        case TH_ST_SLOT_STEAL:
            return "Slot steal" ;
        case TH_ST_IN_USE:
            return "In Use" ;
        case TH_ST_UNKNOWN:
        default:
            return "Unknown" ;
    }
}
const char *LocoNetThrottle::getStatus1Str(uint8_t stat1)
{
    static std::string str;
    str = "Stat1: ";
    switch(stat1 & LOCOSTAT_MASK)
    {
        case LOCO_IN_USE:
            str+= "Loco in use"; break;
        case LOCO_IDLE:
            str+= "Loco idle"; break;
        case LOCO_COMMON:
            str+= "Loco common"; break;
        case LOCO_FREE:
            str+= "Loco free"; break;
        default:
            break;
    }
    switch(stat1 & CONSIST_MASK)
    {
        case CONSIST_MID:
            str+= ", mid"; break;
        case CONSIST_TOP:
            str+= ", top"; break;
        case CONSIST_SUB:
            str+= ", sub"; break;
        case CONSIST_NO:
            break;
        default:
            break;
    }
    switch(stat1 & DEC_MODE_MASK)
    {
        case DEC_MODE_128A:
            str+= ", mode 128A"; break;
        case DEC_MODE_28A:
            str+= ", mode 28A"; break;
        case DEC_MODE_128:
            str+= ", mode 128"; break;
        case DEC_MODE_14:
            str+= ", mode 14"; break;
        case DEC_MODE_28TRI:
            str+= ", mode 28TRI"; break;
        default:
            break;
    }
    return str.c_str();
}
const char *LocoNetThrottle::getStatus2Str(uint8_t stat2)
{
    static std::string str;
    str = "Stat2: ";
    switch(stat2 & (STAT2_ALIAS_MASK | STAT2_SL_SUPPRESS ))
    {
        case STAT2_SL_SUPPRESS:
            str+= "Stat2: consist supressed"; break;
        case STAT2_SL_NOT_ID:
            str+= "Stat2: ID1/ID2 not ID usage"; break;
        case STAT2_SL_NOTENCOD:
            str+= "Stat2: ID1/ID2 not alias"; break;
        default:
            break;
    }
    return str.c_str();
}
const char *LocoNetThrottle::getErrorStr(ThError_t error)
{
    switch(error)
    {
        case TH_ER_OK:
            return "Ok";
        case TH_ER_SLOT_NOT_IN_USE:
            return "Not in use";
        case TH_ER_SLOT_IN_USE:
            return "In Use";
        case TH_ER_SLOT_FREE:
            return "Already free";
        case TH_ER_INIT:
            return "Already initialized";
        case TH_ER_NOT_INIT:
            return "Not initialized";
        case TH_ER_BUSY:
            return "Busy" ;
        case TH_ER_NOT_SELECTED:
            return "Not Selelected";
        case TH_ER_NO_LOCO:
            return "No Loco";
        case TH_ER_NO_DISPATCH:
            return "No dispatch slot";
        case TH_ER_NO_SLOTS:
            return "No Free Slots";
        case TH_ER_SLOT_MOVE:
            return "Illegal slot move";
        default:
            return "Unknown" ;
    }
}
uint8_t LocoNetThrottle::getFunc0to8(uint8_t function)
{
    uint8_t mask ;
    if(function <= 4 )
    {
        mask = (uint8_t) (1 << ((function) ? function - 1 : 4 ));
        return myDirFunc0to4 & mask;
    }
    mask = (uint8_t) (1 << (function - 5));
    return myFunc5to8 & mask ;
}





