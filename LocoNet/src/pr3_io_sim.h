#ifndef PR3_SIM_H
#define PR3_SIM_H
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
 * Simulated serial interface to the LocoNet bus PR3 used for the
 * simulated loconet responses.  Refer to Throttle.pro for setting this switch
 *****************************************************************************/

#include <stdint.h>
extern void initializePr3IO();
extern void pr3Puts(char *);
extern void pr3Putc(char);
extern char *pr3Gets_n(char *, int);
extern char pr3Getch(void);
extern void pr3Flush(void);
extern int  pr3RxReady(void);

// Interface to serialIoClass
int  _pr3Ready(void);
void _pr3Flush(void);
char _pr3Getch(void);
void _pr3Putch(char c);

#endif // PR3_SIM_H

