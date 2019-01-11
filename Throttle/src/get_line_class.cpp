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
 * Class for getting single line of keyboard text terminated by ENTER/RETURN key
 * as a non blocking thread.
 * Sample the data ready state by using the polling function getReadyState()
 * If the return value is true then their is a lin of text available for reading.
 * The function getLine() will return the keyboard input string.
 * **********************************************************************************/
#include <poll.h>

#include "get_line_class.h"

GetLineClass::GetLineClass(void)
{
	exitThread = false;
	runThread = true;
	dataReady =  false;
	getLineThread = thread(&GetLineClass::threadFunc, this);
}
GetLineClass::~GetLineClass()
{
	getLineThread.detach();
	runThread = false;
	while(!exitThread) { sleep(0); }
}

void GetLineClass::threadFunc(void)
{
	struct pollfd fds;
	int    timeout = 1000; // Ms
	fds.fd = 0;
	fds.events = POLLIN;
	fds.revents = 0;

	int retVal;
	exitThread = false;

	while(runThread)
	{
		retVal = poll(&fds, 1, timeout);
		if((retVal == 1) && ((fds.revents&POLLIN) == POLLIN))
		{   // Check returned event for input
			getline(cin, dataStr);
			dataReady = true;
		}
		else if(retVal < 0)
			perror("ERROR: GetLine thread, ");
	}
	dataReady = false;
	exitThread = true;
}

void GetLineClass::setReadyState(bool state)
{
	dataReady = state;
}

string GetLineClass::getLine(void)
{
	dataReady = false;
	return dataStr;
}

void GetLineClass::flush(void)
{
	dataReady = false;
	dataStr = "";
}

