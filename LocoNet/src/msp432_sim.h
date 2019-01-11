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
 * Simulated sSerial interface to the MSP432 Debug/Uart USB port
 * for the simulator.
 *****************************************************************************/

#ifndef MSP432_SIM_H_
#define MSP432_SIM_H_
#define UART_BUFFER_SIZE        256

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
void initializeUart(void);
void initializeSpiMaster(void);
bool startSpiMasterTransmit(uint8_t *, int, uint8_t *, int, bool);
void initializeAtod(void);
void startAtodConversion();
void mySleep(double);
void mySleepMs(unsigned);

// Variables
extern bool spiMasterReady;
extern uint16_t resultsBuffer[];
extern uint8_t *_uartRxInPtr;
extern uint8_t *_uartRxOutPtr;
extern uint8_t *_uartRxEndPtr;
extern uint8_t _uartRxBuffer[];
extern uint8_t *_uartTxInPtr;
extern uint8_t *_uartTxOutPtr;
extern uint8_t *_uartTxEndPtr;
extern uint8_t _uartTxBuffer[];
extern bool    spiMasterReady;

#endif /* MSP432_SIM_H_ */
