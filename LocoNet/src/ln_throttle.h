/****************************************************************************
 *  Copyright (C) 2009 to 2013 Alex Shepherd
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
#ifndef SRC_THROTTLE_H_
#define SRC_THROTTLE_H_

#include <stdint.h>
#include "ln_opc.h"
#include "loco_net.h"

typedef enum
{   // Note do not change the order of the first 4 states because of <= TH_ST_REQUEST use in the code
    TH_ST_FREE,   TH_ST_IDLE,      TH_ST_RELEASE,    TH_ST_REQUEST, TH_ST_DISPATCH,
    TH_ST_RECALL, TH_ST_SLOT_MOVE, TH_ST_SLOT_STEAL, TH_ST_IN_USE,  TH_ST_COMMON,
    TH_ST_MOVE,   TH_ST_INIT,      TH_ST_UNKNOWN
} ThState_t;
typedef enum
{
    TH_ER_OK,      TH_ER_SLOT_IN_USE, TH_ER_SLOT_FREE,   TH_ER_NOT_INIT,  TH_ER_BUSY,
    TH_ER_NO_LOCO, TH_ER_INIT,        TH_ER_NO_DISPATCH, TH_ER_SLOT_MOVE, TH_ER_NO_SLOTS,
    TH_ER_NOT_SELECTED, TH_ER_SLOT_NOT_IN_USE
} ThError_t;

typedef enum
{
    TH_SP_ST_28      = 0,  // 000=28 step/ 3 BYTE PKT regular mode
    TH_SP_ST_28_TRI  = 1,  // 001=28 step. Generate Trinary packets for this Mobile ADR
    TH_SP_ST_14      = 2,  // 010=14 step MODE
    TH_SP_ST_128     = 3,  // 011=send 128 speed mode packets
    TH_SP_ST_28_ADV  = 4,  // 100=28 Step decoder ,Allow Advanced DCC consisting
    TH_SP_ST_128_ADV = 7   // 111=128 Step decoder, Allow Advanced DCC consisting
} ThSpeedSteps_t;

#define TH_OP_DEFERRED_SPEED 0x01
#define THIS_ID              9999
#define INIT_SLOT            1

// Callbacks
// Power management, Transponding and Multi-Sense Device info Call-back functions
extern void notifyThState(ThState_t state) __attribute__ ((weak));
extern void notifyThError(ThError_t error) __attribute__ ((weak));
extern void notifyDirection(uint8_t direction) __attribute__ ((weak));
extern void notifySixByteFunc0to8(uint8_t function, bool state) __attribute__ ((weak));
extern void notifyFunc0to4(uint8_t function, bool state) __attribute__ ((weak));
extern void notifyFunc5to8(uint8_t function, bool state) __attribute__ ((weak));
extern void notifyAddress(uint16_t address) __attribute__ ((weak));
extern void notifySpeed(uint8_t speed) __attribute__ ((weak));
extern void notifyStatus( uint8_t status) __attribute__ ((weak));
extern void notifySlot(uint8_t slot, bool ours) __attribute__ ((weak));
extern void notifyId(uint16_t id, bool ours) __attribute__ ((weak));
extern void notifyThSpeedSteps(ThSpeedSteps_t speedSteps) __attribute__ ((weak));

class LocoNetThrottle : public LocoNet
{
    public:
        LocoNetThrottle();
        ~LocoNetThrottle();
    public: // Variables
        ThSpeedSteps_t mySpeedSteps;
        ThState_t myState;
        uint16_t  myAddress;       // Decoder Address
        uint16_t  myId;            // Id of throttle
        uint8_t   mySpeed;         // Locomotive Speed
        uint8_t   mySlot;          // Master Slot index
        uint8_t   myStatus1;       // Stat1
        uint8_t   myStatus2;       // Stat2
        uint8_t   myDirFunc0to4;   // Direction
        uint8_t   myFunc5to8;
        uint8_t   myFunc9to12;
        uint8_t   myFunc13to20;
        uint8_t   myFunc21to28;
        uint8_t   mySixByteDir;    // Direction
        uint8_t   mySixByte28;
        uint8_t   mySixByte21to27;
        uint8_t   mySixByte14to20;
        uint8_t   mySixByte7to13;
        uint8_t   mySixByte0to6;
    public: // Functions
        void     init(std::string ttyDevice, uint16_t throttleId);
        void     init( uint16_t throttleId);
        bool     initLocoNet(void);
        bool     initLocoNet(uint16_t address);
        void     processThrottleMsg(lnMsg *lnPacket);
        bool     requestAddress(uint16_t address);
        bool     stealAddress(uint16_t address);
        bool     dispatchAddress(void);
        bool     dispatchAddress(uint16_t address);
        bool     recallAddress(void);
        bool     releaseAddress(bool force);
        uint16_t getAddress(void){ return myAddress; }
        uint8_t  getFunc0to8(uint8_t function);
        uint8_t  getSixByteSpeed(void) ;
        uint8_t  getSixByteDir(void);
        uint8_t  getSpeed(void) ;
        uint8_t  getDirection(void);
        bool     setSixByteFunc9to28(int function);
        bool     setSixByteFunc0to8(int function, bool state);
        bool     setDirFunc0to4Direct(uint8_t value);
        bool     setFunc5to8Direct(uint8_t value);
        bool     setFunc0to8(int function, bool state);
        bool     setFunc9to28ShortAddr(int function);
        bool     setSixByteSpeed(uint8_t speed) ;
        bool     setSixByteDir(uint8_t direction) ;
        bool     setSpeed(uint8_t speed) ;
        bool     setDirection(uint8_t direction) ;
        const char *getStateStr(ThState_t state);
        const char *getErrorStr(ThError_t error);
        const char *getStatus1Str(uint8_t stat1);
        const char *getStatus2Str(uint8_t stat2);
    private: // Updates
        void     updateSpeedSteps(ThSpeedSteps_t speedSteps);
        void     updateSpeed(uint8_t speed);
        void     updateAddress(uint16_t address);
        void     updateSlot(uint8_t slot, bool ours);
        void     updateId(uint16_t id, bool ours);
        void     updateThState(ThState_t state);
        void     updateStatus1(uint8_t status);
        void     updateStatus2(uint8_t status);
        void     updateDirAndFunc0to4(uint8_t dirFunc0to4);
        void     updateFunc5to8(uint8_t func5to8);
        void     updateSixByteFunc0to8(uint8_t func0to8, uint8_t value);
        void     updateSixByteSpeed(uint8_t dir, uint8_t speed);
};

#endif /* SRC_THROTTLE_H_ */
