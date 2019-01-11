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
#ifndef LOCO_SIMULATION_H
#define LOCO_SIMULATION_H

#include "ln_opc.h"

class LocoNet;

class LocoSimulation
{
    public:
        LocoSimulation(LocoNet *parent = 0);
        ~LocoSimulation();
        void locoRemapMsg(lnMsg *lnPacket, uint8_t lnTxLength);
        void locoSimulationLoop(void);
    private:
        LocoNet *parent;
        uint8_t calcCheckSum(lnMsg *lnPacket, uint8_t lnTxLength);
};

#endif // LOCO_SIMULATION_H
