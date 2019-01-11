#ifndef PR3_SIM_H
#define PR3_SIM_H
/****************************************************************************
 *  Copyright (C) 2017 Ken Lindsay
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
 *  DESCRIPTION
 * Simulated serial interface to the LocoNet bus PR3
 * for the simulator.
 *****************************************************************************/

#include <stdint.h>

extern uint8_t *_pr3RxInPtr;
extern uint8_t *_pr3RxOutPtr;
extern uint8_t *_pr3RxEndPtr;
extern uint8_t _pr3RxBuffer[];
extern uint8_t *_pr3TxInPtr;
extern uint8_t *_pr3TxOutPtr;
extern uint8_t *_pr3TxEndPtr;
extern uint8_t _pr3TxBuffer[];

#endif // PR3_SIM_H

