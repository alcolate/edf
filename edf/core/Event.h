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
#pragma once

namespace Edf
{

typedef uint16_t Signal; /* event signal */

enum Signals 
{
	INIT_SIG, 
	ENTRY_SIG,
	EXIT_SIG,
	USER_SIG,  /* first signal available to the users */
	TIMEOUT_SIG,
	TEST_SIG,
	TEST2_SIG,
	TEST3_SIG,
	TEST4_SIG,
	TEST5_SIG,
	SERIAL_IN_SIG,   // get from serial
	SERIAL_OUT_SIG,  // send to serial
	SERIAL_SEND_COMPLETE_SIG,
	MAX_SIG,
};


/* Event base class */
class Event 
{
public:
	Event(Signal s, bool DynAlloc = false);
	virtual ~Event();
	Signal Sig; 
	uint32_t	RefCount;
	const bool  DynamicAlloc;
	void InitRef(uint32_t Ref, bool FromISR = false);
	void IncRef(uint32_t Ref, bool FromISR = false);
	void DecRef(bool FromISR = false);

};

extern Event const InitEvt;
extern Event const EntryEvent;
extern Event const ExitEvent;

} // namaspace Edf
