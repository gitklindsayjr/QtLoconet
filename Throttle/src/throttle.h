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
 *******************************************************************************/
/*******************************************************************************
* DESCRIPTION
* This class provides an interface between Qt MainWindow class
* and a LocoNetThrottle class.
*****************************************************************************/
#ifndef THROTTLE_H_
#define THROTTLE_H_

#include <QWidget>
#include <QTimer>
#include "serial_io_class.h"
#include "ln_throttle.h"
#include "get_line_class.h"

#define REVERSE      false
#define FORWARD      true
#define EMERG_STOP   1
#define UPDATE_TIME  100  // 100ms
#define UPDATE_RATE  10   // 1 Second
#define REFRESH_CNT (100*UPDATE_RATE) // Number of counts, in 100 seconds

class Throttle : public QWidget
{
    Q_OBJECT
public:
    Throttle(QWidget *parent);
    Throttle(bool dispatchChildProcess, QWidget *parent);
    ~Throttle(void);
public:
    LocoNetThrottle *throttle;
private:
    QWidget *parent;         // Out parent is the MainWindow for signaling
    lnMsg  *lnPacket;        // Loconet message packet
    int     refreshCnt;      // 200 second update counter
    bool    dispatchChildProcess;
    void    throttleStart(void);
    GetLineClass cmdLine;
    string  cmdLineStr;
public slots:
// Throttle slots
    void throttleStatus(void);
    void functionsCommand(int function);
    void bellCommand(bool state);
    void muteCommand(bool state);
    void dynamicBrakeCommand(bool state);
    void f3Command(bool state);
    void hornCommand(bool state);
    void lightCommand(bool state);
    void powerCommand(bool state);
    void directionCommand(bool dir);
    void speedCommand(int speed);
    void stopCommand(void);
    void stealCommand(int address);
    void requestCommand(int address);
    void initCommand(int address);
    void dispatchCommand(void);
    void recallCommand(void);
    void releaseCommand(bool force);
    void switchStateCommand(int sw, bool state);
    void idChange(int id);
// Throttle signals
signals:
    void muteResponse(bool state);
    void lightResponse(bool state);
    void bellResponse(bool state);
    void directionResponse(bool direction);
    void speedResponse(int speed);
    void addressResponse(int address);
    void slotResponse(int slot, bool ours);
    void idResponse(int id, bool ours);
    void powerResponse(bool state);
    void switchRequest(int sw, bool state);
    void switchState(int sw, bool state);
    void stateResponse(QString str);
    void statusResponse(QString str);
    void receiveMessage(QString msg);
};
#endif
