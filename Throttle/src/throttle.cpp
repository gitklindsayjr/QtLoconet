/****************************************************************************
 *  Copyright (C) 2018 Ken Lindsay
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
 * This class provides an interface between "mainwindow.cpp" QT MainWindow class
 * and a LocoNetThrottle class.
 *****************************************************************************/

#include <QString>
#include <QDebug>
#include <unistd.h>
#include "throttle.h"
#include "serial_io_class.h"
#include "ln_throttle.h"

#ifdef PR3_SIM  // Simulate loconet without physical presence
    #include "loco_sim.h"
   LocoNetThrottle *throttle;  // Extern throttle reference for simulation
#endif

Throttle *cbSignals;  // Call back signal refrence for LocoNetThrottles

Throttle::Throttle(bool dispatchChildProcess, QWidget *parent)
{   // Throttle constructor for forked child process
    this->parent = parent;
    this->dispatchChildProcess = dispatchChildProcess;
    throttleStart();
}
Throttle::Throttle(QWidget *parent)
{   // Stand alone constructor, no forked child process
    this->parent = parent;
    this->dispatchChildProcess = false;
    throttleStart();
}

void Throttle::throttleStart(void)
{   // Initialize this throttle
    QTimer *statusTimer = new QTimer(this); // Timer for checking status and throtle messages
    cbSignals = this; // Initialize notification callbacks
    throttle = new LocoNetThrottle(); // Set throttle pointer ot LocoNetThrottle class
#ifdef PR3_SIM
    throttle->init(9999);
#else
    throttle->init("/dev/loconet", 9999);  // If not simulating set our ID and USB port
#endif
    // Connect signals to MainWindow UI slots ofr LocoNetThrottle callbacks
    connect(this, SIGNAL(speedResponse(int)), parent, SLOT(speedResponse(int)));
    connect(this, SIGNAL(speedResponse(int)), parent, SLOT(speedResponse(int)));
    connect(this, SIGNAL(addressResponse(int)), parent, SLOT(addressResponse(int)));
    connect(this, SIGNAL(muteResponse(bool)), parent, SLOT(muteResponse(bool)));
    connect(this, SIGNAL(lightResponse(bool)), parent, SLOT(lightResponse(bool)));
    connect(this, SIGNAL(bellResponse(bool)), parent, SLOT(bellResponse(bool)));
    connect(this, SIGNAL(directionResponse(bool)), parent, SLOT(directionResponse(bool)));
    connect(this, SIGNAL(powerResponse(bool)), parent, SLOT(powerResponse(bool)));
    connect(this, SIGNAL(idResponse(int, bool)), parent, SLOT(idResponse(int, bool)));
    connect(this, SIGNAL(slotResponse(int, bool)), parent, SLOT(slotResponse(int, bool)));
    connect(this, SIGNAL(stateResponse(QString)), parent, SLOT(stateResponse(QString)));
    connect(this, SIGNAL(statusResponse(QString)), parent, SLOT(statusResponse(QString)));
    connect(this, SIGNAL(receiveMessage(QString)), parent, SLOT(receiveMessage(QString)));
    connect(this, SIGNAL(switchRequest(int, bool)), parent, SLOT(switchRequest(int, bool)));
    connect(this, SIGNAL(switchState(int, bool)), parent, SLOT(switchState(int, bool)));
    connect(statusTimer, SIGNAL(timeout()), this, SLOT(throttleStatus()));
    if(throttle->error)
        receiveMessage("Error: Can't open /dev/loconet");
    statusTimer->start(100);  // Start timer status updates at this rate, milliseconds
}
Throttle::~Throttle(void)
{
    delete throttle;
}
void Throttle::throttleStatus(void)
{
#ifdef PR3_SIM
    throttle->locoSimulation->locoSimulationLoop();
#endif
    if(dispatchChildProcess)
    {   // Handle stdin messages from Dispatcher
        if(cmdLine.getReadyState())
        {   // Class getline is a thread function
            cmdLineStr = cmdLine.getLine();
            cout << cmdLineStr << endl; // Echo input to stdout
        }
    }
    int packetSize = 0; // Number to bytes in loconet packet
    // Refresh loconet interface if I own this throttle by sending a speed command
    if((refreshCnt++ == REFRESH_CNT) && (throttle->myId == THIS_ID))
        { speedCommand(throttle->getSpeed()); refreshCnt = 0; }
    lnPacket = throttle->receive(&packetSize);
    if(lnPacket)
    {   // Handle loconet messages first, if none handle throttle messages
        if(!throttle->processLocoNetMessage(lnPacket, packetSize))
            throttle->processThrottleMsg(lnPacket);
    }
}
// Throttle slots connected with signals to MainWindow
void Throttle::switchStateCommand(int sw, bool state)
{   // Process switch state commands from Dispatcher
    uint8_t direction;
    if(state)
        direction = OPC_SW_STATE_THROWN;
    else
        direction = OPC_SW_STATE_CLOSED;
    throttle->switchState((uint16_t)sw, direction);
}
void Throttle::functionsCommand(int function)
{   // Handle functions 9 thru 28
#ifdef USE_SIX_BYTE
    throttle->setSixByteFunc9to28((uint8_t)function);
#else
    throttle->setFunc9to28ShortAddr(function);
#endif
}
void Throttle::muteCommand(bool state)
{   // Send mute function F8 to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteFunc0to8(F8_MUTE, state);
#else
    throttle->setFunc0to8(F8_MUTE, state);
#endif
}
void Throttle::f3Command(bool state)
{   // Send F3 function to locoNetThrottle
#ifdef USE_SIX_BYTE
    if((throttle->mySixByte0to6 & F3_FUNCTION_MASK) == F3_FUNCTION_MASK)
        throttle->setSixByteFunc0to8(F3_FUNCTION, !state);
    else
        throttle->setSixByteFunc0to8(F3_FUNCTION, state);
#else
    if((throttle->myDirFunc0to4 & DIRF_F3) == DIRF_F3)
        throttle->setFunc0to8(F3_FUNCTION, !state);
    else
        throttle->setFunc0to8(F3_FUNCTION, state);
#endif
}
void Throttle::dynamicBrakeCommand(bool state)
{   // Send F4 function to locoNetThrottle
#ifdef USE_SIX_BYTE
    if((throttle->mySixByte0to6 & F4_DYNAMIC_BRAKE_MASK) == F4_DYNAMIC_BRAKE_MASK)
        throttle->setSixByteFunc0to8(F4_DYNAMIC_BRAKE, !state);
    else
        throttle->setSixByteFunc0to8(F4_DYNAMIC_BRAKE, state);
#else
    if((throttle->myDirFunc0to4 & DIRF_F4) == DIRF_F4)
        throttle->setFunc0to8(F4_DYNAMIC_BRAKE, !state);
    else
        throttle->setFunc0to8(F4_DYNAMIC_BRAKE, state);
#endif
}
void Throttle::bellCommand(bool state)
{   // Send F1 function to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteFunc0to8(F1_BELL, state);
#else
    throttle->setFunc0to8(F1_BELL, state);
#endif
}
void Throttle::hornCommand(bool state)
{   // Send F2 function to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteFunc0to8(F2_HORN, state);
#else
    throttle->setFunc0to8(F2_HORN, state);
#endif
}
void Throttle::lightCommand(bool state)
{   // Send F0 function to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteFunc0to8(F0_LIGHT, state);
#else
    throttle->setFunc0to8(F0_LIGHT, state);
#endif
}
void Throttle::powerCommand(bool state)
{   // Send power commands to locoNetThrottle
    if(throttle->myState != TH_ST_INIT) // Set speed if not initialized
        throttle->setSpeed(0);
    throttle->setPower(state);
}
void Throttle::directionCommand(bool dir)
{   // Send loco direction to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteDir(dir);
#else
    throttle->setDirection((uint8_t)dir);
#endif
}
void Throttle::speedCommand(int speed)
{   // Send speed to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteSpeed((uint8_t)speed);
#else
    throttle->setSpeed((uint8_t)speed);
#endif
}
void Throttle::stopCommand(void)
{   // Send Emergency stop to locoNetThrottle
#ifdef USE_SIX_BYTE
    throttle->setSixByteSpeed(EMERG_STOP);
#else
    throttle->setSpeed(EMERG_STOP);
#endif
}
void Throttle::stealCommand(int address)
{   // Steal slot command for address
    throttle->stealAddress((uint16_t)address);
}
void Throttle::requestCommand(int address)
{   // Request slot for address
    throttle->requestAddress((uint16_t)address);
}
void Throttle::dispatchCommand(void)
{   // Dispatch move active slot to slot 0
    throttle->dispatchAddress();
}
void Throttle::recallCommand(void)
{
    throttle->recallAddress();
}
void Throttle::releaseCommand(bool force)
{   // Release slot, force release if required
    if(!force)
        throttle->setSpeed(0);
    throttle->releaseAddress(force);
}
void Throttle::initCommand(int address)
{   // Intialize UI
    if(address == 0) // Slot 1 initializaiton
        throttle->initLocoNet();
    else // Address initialization
        throttle->initLocoNet((uint16_t)address);
}
void Throttle::idChange(int id)
{   // Change my ID slot
    if(throttle->myState == TH_ST_INIT)
    {
        throttle->myId = (uint16_t)id;
        notifyId(id, false);  // Use notification to update ID
    }
    else
    {
        notifyId(throttle->myId, true);
        notifyThError(TH_ER_SLOT_IN_USE);
    }
}
// These callbacks from loco_net send signals back to the UI MainWindow slots
void notifyPower(uint8_t power)
{
    cbSignals->powerResponse((bool)power);
}
void notifyAddress(uint16_t address)
{
    cbSignals->addressResponse((int)address);
}
void notifySpeed(uint8_t speed)
{   // Scale to DigiTrax throttle displays
    cbSignals->speedResponse(speed);
}
void notifyDirection(uint8_t direction)
{
    if(direction == DIRF_DIR)
        cbSignals->directionResponse(false);
    else
        cbSignals->directionResponse(true);
}
void notifySixByteFunc0to8(uint8_t function, bool state)
{
    switch(function)
    {   // Handle these functions
        case F0_LIGHT:
            cbSignals->lightResponse(state);
            break;
        case F1_BELL:
            cbSignals->bellResponse(state);
            break;
        case F8_MUTE:
            cbSignals->muteResponse(state);
        default:  // Skip over F2 - F7
            break;
    }
}
void notifyFunc0to4(uint8_t function, bool state)
{
    switch(function)
    {   // Handle these functions
        case DIRF_F0:
            cbSignals->lightResponse(state);
            break;
        case DIRF_F1:
            cbSignals->bellResponse(state);
            break;
        case DIRF_F2:
        case DIRF_F3:
        case DIRF_F4:
        default:  // Skip over F2 - F7
            break;
    }
}
void notifyFunc5to8(uint8_t function, bool state)
{
    switch(function)
    {
        case SND_F8:
            cbSignals->muteResponse(state);
            break;
        case SND_F7:
        case SND_F6:
        case SND_F5:
        default:
            break;
    }
}
void notifyStatus(uint8_t status)
{   // Update UI MainWindow status messages
    QString str= cbSignals->throttle->getStatus1Str(status);
    cbSignals->statusResponse(str);
}
void notifyThState(ThState_t state)
{   // Update UI MainWindow state messages
    QString str= cbSignals->throttle->getStateStr(state);
    cbSignals->stateResponse(str);
}
void notifyThError(ThError_t error)
{   // Update UI MainWindow error messages
    QString msg = "Error: ";
    QString err = cbSignals->throttle->getErrorStr(error);
    msg.append(QString::number(error) + " - " + err);
    cbSignals->receiveMessage(msg);
}
void notifyId(uint16_t id, bool ours)
{   // Update UI ID edit box
    cbSignals->idResponse((int)id, ours);
}
void notifySlot(uint8_t slot, bool ours)
{   // Update UI slot edit box
    cbSignals->slotResponse((int)slot, ours);
}
void notifySwitchRequest(uint16_t address, uint8_t direction)
{   // Request to operate a switch forwarded to Dispatcher through stdout
    int sw = (uint16_t)address;
    bool state = (bool)direction;
    cbSignals->switchRequest(sw, state);
}
void notifySwitchState(uint16_t address, uint8_t direction)
{   // Set switch state forwarded to Dispatcher through stdout
    int sw = (uint16_t)address;
    bool state = (bool)direction;
    cbSignals->switchState(sw, state);
}
