#ifndef SERIAL_IO_CLASS_H_
#define SERIAL_IO_CLASS_H_
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
 * Serial interface to the MSP432 Debug/Uart USB port and the LocoNet bus PR3 for use
 * on linux based systems.
 *****************************************************************************/

#include <stdint.h>
#include <string>
#include <termios.h>
#include <poll.h>

#define MSP432   1
#define PR3      2

enum E_FLOW
{
    FLOW_CONTROL_NONE_E = 0,
    FLOW_CONTROL_CTS_E = 1
};

class SerialIoClass
{
public:
    SerialIoClass(int msp432orPR3);
    SerialIoClass(void);
    bool openPort(std::string ttyDevice, E_FLOW flow);
	bool openPort(std::string ttyDevice);
    bool openPort(int device, E_FLOW flow);
    bool openPort(void);
	~SerialIoClass();
	void flush(void);
    unsigned gets_n(char *str, unsigned n);
	int  getch(void);
	void putch(char c);
    void puts(char *str);
	void puts_n(char *str, unsigned n);
    bool readReady(int timeout); // milliseconds
public:
    unsigned read_n(uint8_t *s, unsigned n);
    unsigned write_n(uint8_t *s, unsigned n);
    unsigned bytesAvailable;
    int  msp432orPR3;
    bool error;
    bool simulate;
private:
	int    fd;
    struct pollfd pollFds;
	std::string ttyDevice;
	bool   initializePort(const char *ttyDevice, E_FLOW flow);
    struct termios old_termios;
    struct termios new_termios;
};

#endif /* SERIAL_IO_CLASS_H_ */
