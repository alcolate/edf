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
#include "Active.h"

namespace Edf
{
CActive::CActive(char *Name)
{
	m_Name = Name;
	m_Queue = 0;
	m_Thread = 0;
	m_Priority = defPrioity;
	m_StackSize = MINIMAL_STACK_SIZE;
	m_NextStateName = 0;
	m_StateName = 0;

}

void CActive::Start()
{
	Start(m_Priority, EQ_SIZE, sizeof(Event *));
}

void CActive::Start(uint8_t prio, uint32_t queueLen, uint32_t itemSize)
{

	this->m_Thread = TaskCreate(
			m_Name,
			m_StackSize,               
			this,                       
			prio, &(this->m_Queue), EQ_SIZE);  

	ASSERT(this->m_Queue);

	ASSERT(this->m_Thread); 
}

bool CActive::Post(Event const *const e, bool FromISR)
{
	LOG_DEBUG("%s post e = %d, ref = %d\r\n", m_Name, e->Sig, e->RefCount);
	bool result = QueueSend(Q(), e, FromISR);
	ASSERT(result);
	return result;
}

void CActive::Run(void)
{
	EventLoop();
}

void CActive::EventLoop()
{

	static Event const initEvt(INIT_SIG);
	
	this->Dispatcher(&initEvt);

	for (;;) 
	{
		Event* e;
		if (QueueReceive(this->m_Queue, &e, MAX_DELAY))
		{			
			LOG_DEBUG("%s get event sig = %d, ref = %d\r\n", m_Name, e->Sig, e->RefCount);
			this->Dispatcher(e);

			e->DecRef();
		}
	}
}


void CActive::Dispatcher(Event const * const e)
{
	switch (e->Sig)
	{
	case INIT_SIG: 
		Initial();
#if (TRACE_STATE == 1)
		LOG_DEBUG("Init:\t%s of %s\r\n", m_StateName, m_Name);
#endif
		break;

	case ENTRY_SIG:
#if (TRACE_STATE == 1)
		LOG_DEBUG("Enter:\t%s of %s\r\n", m_StateName, m_Name);
#endif
		RUN_STATE();
		break;

	case EXIT_SIG:
#if (TRACE_STATE == 1)
		LOG_DEBUG("Exit:\t%s of %s\r\n", m_StateName, m_Name);
#endif
		RUN_STATE();
		TO_NEXT_STATE();
		break;

	default:
		RUN_STATE();
		break;

	}
}

} // namespace Edf
