/****************************************************************************
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
 *********************************************************************************************
 *  DESCRIPTION
 *  This class provides functions that allow simulation of LocoNet reponses without connection
 *  to the physical loconet bus.  The simulated messages derived from a DigiTrax DCS-210 with a
 *  DT-500D throttle connected.
 *
 *****************************************************************************/

#include <unistd.h>
#include <serial_io_class.h>
#include "ln_throttle.h"
#include "loco_net.h"
#include "loco_sim.h"
#include "pr3_io_sim.h"

extern LocoNetThrottle *throttle; // This pointer to expose class variables

uint8_t LocoSimulation::calcCheckSum(lnMsg *lnPacket, uint8_t lnTxLength)
{
    uint8_t checkLength = lnTxLength - 1;
    uint8_t checkSum = 0xff;
    for(int i=0; i<checkLength; i++)
        checkSum^= lnPacket->data[i];
    return checkSum;
}
void LocoSimulation::locoSimulationLoop(void)
{
    uint8_t c;
    while(pr3RxReady())
    {   // Map Rx to Tx
        c = pr3Getch();
        pr3Putc(c);
    }
}
LocoSimulation::LocoSimulation(LocoNet *parent)
{
    this->parent = parent;
    initializePr3IO();
    sleep(1);
}
void LocoSimulation::locoRemapMsg(lnMsg *lnPacket, uint8_t lnTxLength)
{
    parent->port->write_n(lnPacket->data, lnTxLength); // Send the message
    int state = throttle->myState;
    uint8_t opc = lnPacket->data[0];
    uint8_t size = 0x0e;
    uint8_t dirf = (DIRF_DIR | F0_LIGHT);
    uint8_t spd = throttle->mySpeed;
    uint8_t id2 = (throttle->myId >> 7);
    uint8_t id1 = (throttle->myId & 0x7f);
    uint8_t slot = throttle->mySlot;
    uint8_t ss2 = throttle->myStatus2;
    uint8_t adr = (throttle->myAddress & 0x7f);
    uint8_t adr2 = (throttle->myAddress >> 7);
    uint8_t snd = 0x01;
    uint8_t trk = 0x47;
    switch(opc)
    {
        case OPC_SLOT_STAT1:
            size = 4;
            lnPacket->ss.command = OPC_SLOT_STAT1;
            lnPacket->ss.stat = (LOCO_FREE | STAT1_SL_SPDEX | STAT1_SL_SPD28);
            lnPacket->ss.slot = slot;
            lnPacket->ss.chksum = calcCheckSum(lnPacket, size);
            parent->port->write_n(lnPacket->data, size);
            break;
        case OPC_MOVE_SLOTS:
            if(state == TH_ST_IN_USE || state == TH_ST_RECALL)
            {
                lnPacket->sd.command = OPC_SL_RD_DATA;
                lnPacket->sd.mesg_size= size;
                lnPacket->sd.slot = slot;
                lnPacket->sd.stat = (LOCO_IN_USE | STAT1_SL_SPDEX | STAT1_SL_SPD28);
                lnPacket->sd.adr  = adr; lnPacket->sd.adr2 = adr2;
                lnPacket->sd.id1 = id1;  lnPacket->sd.id2 = id2;
                lnPacket->sd.spd  = spd;
                lnPacket->sd.dirf = dirf;
                lnPacket->sd.trk = trk;
                lnPacket->sd.ss2 = ss2;
                lnPacket->sd.snd = snd;
                lnPacket->sd.chksum = calcCheckSum(lnPacket, size);
                parent->port->write_n(lnPacket->data, size);
            }
            break;
        case OPC_RQ_SL_DATA:
            if(state == TH_ST_DISPATCH || state == TH_ST_INIT)
            {
                lnPacket->sd.command = OPC_SL_RD_DATA;
                lnPacket->sd.mesg_size= size;
                lnPacket->sd.slot = slot;
                if(state == TH_ST_INIT)
                    lnPacket->sd.stat = (LOCO_IN_USE | STAT1_SL_SPDEX | STAT1_SL_SPD28);
                else
                    lnPacket->sd.stat = (LOCO_IDLE | STAT1_SL_SPDEX | STAT1_SL_SPD28);
                lnPacket->sd.adr  = 2; lnPacket->sd.adr2 = 0;
                lnPacket->sd.id1 = id1;  lnPacket->sd.id2 = id2;
                lnPacket->sd.spd  = 0;
                lnPacket->sd.dirf = dirf;
                lnPacket->sd.trk = 0;
                lnPacket->sd.ss2 = 0;
                lnPacket->sd.snd = 0x01;
                lnPacket->sd.chksum = calcCheckSum(lnPacket, size);
                parent->port->write_n(lnPacket->data, size);
            }
            break;
        case OPC_LOCO_ADR: // Map d1 -> d9 and d2 -> d4
            switch(state)
            {
                case TH_ST_INIT:
                case TH_ST_SLOT_STEAL:
                    lnPacket->sd.id1 = 0x5a; lnPacket->sd.id2 = 0x52;
                    lnPacket->sd.stat = (LOCO_COMMON | STAT1_SL_SPDEX | STAT1_SL_SPD28);
                    /* no break */
                case TH_ST_RELEASE:
                case TH_ST_REQUEST: // Initialized to IN_USE need steal
                    size = 0x0e;
                    lnPacket->sd.command = OPC_SL_RD_DATA;
                    lnPacket->sd.mesg_size= size;
                    lnPacket->sd.slot = 0x01;
                    lnPacket->sd.adr  = adr;
                    lnPacket->sd.adr2 = adr2;
                    lnPacket->sd.spd  = 0;
                    lnPacket->sd.dirf = dirf;
                    lnPacket->sd.trk = trk;
                    lnPacket->sd.ss2 = ss2;
                    lnPacket->sd.snd = snd;
                    if(state == TH_ST_REQUEST)
                    {
                        id2 =  0x0f; id1 =  0x4e;
                        lnPacket->sd.stat = (LOCO_FREE | STAT1_SL_SPDEX | STAT1_SL_SPD28);
                    }
                    lnPacket->sd.chksum = calcCheckSum(lnPacket, size);
                    parent->port->write_n(lnPacket->data, size);
                    break;
                default:
                    break;
            }
            break;
        case OPC_WR_SL_DATA:
            if(state == TH_ST_IN_USE)
            {
                size = 4;
                lnPacket->lack.command = OPC_LONG_ACK;
                lnPacket->lack.ack1 = 0x7f;
                lnPacket->lack.opcode = (opc & 0x80);
                lnPacket->sd.chksum = calcCheckSum(lnPacket, size);
                parent->port->write_n(lnPacket->data, size);
                lnPacket->lsp.command = OPC_LOCO_SPD;
                lnPacket->lsp.slot = lnPacket->sd.slot;
                lnPacket->lsp.spd = lnPacket->sd.spd;
                lnPacket->sd.chksum = calcCheckSum(lnPacket, size);
                parent->port->write_n(lnPacket->data, size);
            }
            break;
        default:
            break;
    }
}


