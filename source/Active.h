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
#include <stdint.h>
#include "osal.h"
#include "Event.h"
#include "State.h"


#pragma once

namespace Edf
{
constexpr uint8_t defPrioity = (MAX_PRIORITIES - 5);

class CActive
{
public:
	CActive(char *Name);

	void Start();

	void Start(uint8_t prio, uint32_t queueLen, uint32_t itemSize);

	bool Post(Event const *const e, bool FromISR = false);

	void Run(void);

	void EventLoop(void) ;

	virtual void Dispatcher(Event const * const e);

	virtual void Initial() = 0;

	void SetPriority(uint32_t Priority)
	{
		m_Priority = Priority;
	}

	void SetStackSize(uint32_t StackSize)
	{
		m_StackSize = StackSize;
	}

public:
	Q_HANDLE Q() const
	{
		ASSERT(m_Queue);
		return m_Queue;
	}

	//State m_State;

public:
	char* m_Name;
private:
	T_HANDLE m_Thread;   
    Q_HANDLE m_Queue;    
    uint32_t m_Priority;
	uint32_t m_StackSize;

	enum {EQ_SIZE = 10};



public:
	const char* m_StateName;
	const char* m_NextStateName;
	DEF_STATEMACHINE(CActive);
};
    
} // namespace Edf

#include "Timer.h"
#include "Publish.h"
