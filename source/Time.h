/*****************************************************************************
* MIT License:
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* Contact information:
* <9183399@qq.com>
*****************************************************************************/
#include <osal.h>
#include "Event.h"

#pragma once

#define MilliSecond(t)  ((t) / portTICK_RATE_MS)

namespace Edf
{
class CTimeEvent : public Event
{
public:
	Q_HANDLE queue;       /* the AO that requested this TimeEvent */
	uint32_t timeout;  /* timeout counter; 0 means not armed */
	uint32_t interval; /* interval for periodic TimeEvent, 0 means one-shot */

public:
	/*..........................................................................*/
	CTimeEvent(Signal sig, Q_HANDLE q);

	/*..........................................................................*/
	void Arm(uint32_t timeout, uint32_t interval) ;

	/*..........................................................................*/
	void Disarm() ;
};

} // namespace Edf