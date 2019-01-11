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
 * Serial interface to the MSP432 Debug/Uart USB port and the LocoNet bus PR3
 *****************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#include "serial_io_class.h"
#ifdef MSP_SIM
    #include "msp_io_sim.h"
    #define SIMULATE
#endif
#ifdef PR3_SIM
    #include "pr3_io_sim.h"
    #define SIMULATE
#endif
SerialIoClass::SerialIoClass(int msp432orPR3)
{
    this->msp432orPR3 = msp432orPR3;
    simulate = false;
    error = false;
    fd = 0;
    bytesAvailable = 0;
}
SerialIoClass::SerialIoClass(void)
{
    this->msp432orPR3 = 0;
    simulate = false;
    error = false;
    fd = 0;
    bytesAvailable = 0;
}
bool SerialIoClass::openPort(std::string ttyDevice, E_FLOW flow)
{
    this->ttyDevice  = ttyDevice;
    error = initializePort(ttyDevice.c_str(), flow);
    return error;
}
bool SerialIoClass::openPort(int device, E_FLOW flow)
{
    if(device == 0)
        this->ttyDevice = "/dev/ttyACM0";
    else if(device == 1)
        this->ttyDevice = "/dev/ttyACM1";
    else if(device == 2)
        this->ttyDevice = "/dev/ttyACM2";
    else
    {
        error = true;
        return error;
    }
    error = initializePort(this->ttyDevice.c_str(), flow);
    return error;
}
bool SerialIoClass::openPort(std::string ttyDevice)
{
    this->ttyDevice  = ttyDevice;
    error = initializePort(ttyDevice.c_str(), FLOW_CONTROL_NONE_E);
    return error;
}
bool SerialIoClass::openPort(void)
{
    this->ttyDevice = "/dev/ttyACM0";
    error = initializePort(ttyDevice.c_str(), FLOW_CONTROL_NONE_E);
    return error;
}
SerialIoClass::~SerialIoClass(void)
{
    tcsetattr(fd, TCSANOW, &old_termios);
    close(fd);
}
void SerialIoClass::flush(void)
{
#ifndef SIMULATE
    tcflush(fd, TCIFLUSH);
#else
    #ifdef MSP_SIM
    if(msp432orPR3 == MSP432)
        _mspFlush();
    #endif
    #ifdef PR3_SIM
    if(msp432orPR3 == PR3)
        _pr3Flush();
    #endif
#endif
}
bool SerialIoClass::readReady(int timeout) // Timeout in milliseconds
{
#ifndef SIMULATE
    int retVal;
    retVal = poll(&pollFds, 1, timeout);
    if((retVal == 1) && ((pollFds.revents&POLLIN) == POLLIN))
    {
        ioctl(fd, FIONREAD, &bytesAvailable);
        return true;
    }
    else
        return false;
#else
    usleep(1000*timeout);
    #ifdef MSP_SIM
    if(msp432orPR3 == MSP432)
    {
        if((bytesAvailable = _mspReady()))
            return true;
        else
            return false;
    }
    #endif
    #ifdef PR3_SIM
    if(msp432orPR3 == PR3)
    {
        if((bytesAvailable = _pr3Ready()))
            return true;
        else
            return false;
    }
    #endif
    return true;
#endif
}
int SerialIoClass::getch(void)
{
    char c;
#ifndef SIMULATE
    read(fd, &c, 1);
#else
    #ifdef MSP_SIM
    if(msp432orPR3 == MSP432)
        c = _mspGetch();
    #endif
    #ifdef PR3_SIM
    if(msp432orPR3 == PR3)
        c = _pr3Getch();
    #endif
#endif
    return c;
}
unsigned SerialIoClass::gets_n(char *s, unsigned n)
{
    char c;
    unsigned i = 0;
    n--;
    c = getch();
    while(c != '\n')
    {
        if(i >= n)
            break;
        s[i++] = c;
        c = getch();
    }
    s[i] = 0;
    return i;
}
void SerialIoClass::puts_n(char *s, unsigned n)
{
    char c;
    if(n == 0)
        return;
    for(unsigned i = 0; i<n; i++)
    {
        c = *s++;
        putch(c);
    }
}
void SerialIoClass::puts(char *s)
{
    char c;
    int  n = strlen((char *)s);
    if(n == 0)
        return;
    for(int i = 0; i<n; i++)
    {
        c = *s++;
        putch(c);
    }
}
void SerialIoClass::putch(char c)
{
#ifndef SIMULATE
    write(fd, &c, 1);
#else
    #ifdef MSP_SIM
    if(msp432orPR3 == MSP432)
        _mspPutch(c);
    #endif
    #ifdef PR3_SIM
    if(msp432orPR3 == PR3)
        _pr3Putch(c);
    #endif
#endif
}
unsigned SerialIoClass::read_n(uint8_t *s, unsigned n)
{
#ifdef SIMULATE
    for(unsigned i = 0; i<n; i++)
        s[i] = getch();
    return n;
#else
    return read(fd, s, n);
#endif
}
unsigned SerialIoClass::write_n(uint8_t *s, unsigned n)
{
#ifdef SIMULATE
    for(unsigned i = 0; i<n; i++)
        putch(s[i]);
    return n;
#else
    return write(fd, s, n);
#endif
}
bool SerialIoClass::initializePort(const char *ttyDevice, E_FLOW flow)
{
    fd = open(ttyDevice, (O_RDWR | O_NOCTTY));
    if(fd < 0)
    {
        fprintf(stderr, "Error: couldn't open file %s\n", ttyDevice);
        return true;
    }
    if(tcgetattr(fd, &old_termios) != 0)
    {
        fprintf(stderr, "Error: tcgetattr(fd, &old_termios) failed: %s\n", strerror(errno));
        return true;
    }
    memset(&new_termios, 0, sizeof(new_termios));
    new_termios.c_iflag = IGNPAR | CRTSCTS; // Ignore framing and parity errors
    new_termios.c_oflag = CRTSCTS;      // No output flags
    new_termios.c_cflag = CS8 | CREAD | CLOCAL | HUPCL; // Eight bits, enable receiver
    new_termios.c_lflag = 0; // ~(ICANON | ECHO | ECHOE | ISIG);
    new_termios.c_cc[VINTR]    = 0; // Interrupt character, ctrl-C
    new_termios.c_cc[VQUIT]    = 0; // Quit character
    new_termios.c_cc[VERASE]   = 0; // Erase character, BACKSPACE, ctrl-H
    new_termios.c_cc[VKILL]    = 0; // Kill character
    new_termios.c_cc[VEOF]     = 4; // End of file EOF, ctrl-D, 4
    new_termios.c_cc[VTIME]    = 0; // Time out
    new_termios.c_cc[VMIN]     = 1; // Minimum characters to block
    new_termios.c_cc[VSWTC]    = 0; // Not supported
    new_termios.c_cc[VSTART]   = 0; // CSTART, XON Character, DC1 ctrl-Q
    new_termios.c_cc[VSTOP]    = 0; // CSTOP, XOFF Character, DC3, ctrl-S
    new_termios.c_cc[VSUSP]    = 0; // Suspend Character, ctrl-Z
    new_termios.c_cc[VEOL]     = 0; // Additional EOL character
    new_termios.c_cc[VREPRINT] = 0; // Reprint unread characters, ctrl-R
    new_termios.c_cc[VDISCARD] = 0; // Discard pending output
    new_termios.c_cc[VWERASE]  = 0; // Word erase
    new_termios.c_cc[VLNEXT]   = 0; // Next input character, ctrl-V
    new_termios.c_cc[VEOL2]    = 0; // Another EOL
    // Set hardware flow control on/off:
    switch(flow)
    {
        case FLOW_CONTROL_NONE_E:   // disable flow control
            new_termios.c_iflag&= ~CRTSCTS;
            new_termios.c_oflag&= ~CRTSCTS;
            break;
        case FLOW_CONTROL_CTS_E:    // enable flow control
            new_termios.c_iflag|= CRTSCTS;
            new_termios.c_oflag|= CRTSCTS;
            break;
        default:
            new_termios.c_iflag|= IXOFF |IXON;
            new_termios.c_oflag|= CRTSCTS;
            break;
    }
/* cfmakeraw(struct termios *termios_p) -- sets the following
 * new_termios.c_iflag&= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
 * new_termios.c_oflag&= ~OPOST;
 * new_termios.c_lflag&= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
 * new_termios.c_cflag&= ~(CSIZE | PARENB);
 * new_termios.c_cflag|= CS8;
*/
    // cfmakeraw(&new_termios);
    if(cfsetispeed(&new_termios, B57600) != 0)
    {
        fprintf(stderr, "Error: cfsetispeed(&new_termios, B57600) failed: %s\n", strerror(errno));
        return true;
    }
    if(cfsetospeed(&new_termios, B57600) != 0)
    {
        fprintf(stderr, "Error: cfsetospeed(&new_termios, B57600) failed: %s\n", strerror(errno));
        return true;
    }
    if(tcsetattr(fd, TCSANOW, &new_termios) != 0)
    {
        fprintf(stderr, "Error: tcsetattr(fd, TCSANOW, &new_termios) failed: %s\n", strerror(errno));
        return true;
    }
    pollFds.fd = fd;
    pollFds.events = POLLIN;
    pollFds.revents = 0;
    tcflush(fd, TCIFLUSH);
    tcflush(fd, TCOFLUSH);
    return false;
}
