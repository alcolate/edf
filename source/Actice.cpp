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

	ASSERT(this->m_Thread); 
}

void CActive::Post(Event const *const e)
{
	QueueSend(Q(), e);
}

void CActive::Run(void)
{
	EventLoop();
}

void CActive::EventLoop()
{

	static Event const initEvt = { INIT_SIG };
	
	this->Dispatcher(&initEvt);

	for (;;) 
	{
		Event *e; 

		/* wait for any event and receive it into object 'e' */
		if (QueueReceive(this->m_Queue, &e, MAX_DELAY))
		{			
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

		break;

	case ENTRY_SIG:
		RUN_STATE();
		break;

	case EXIT_SIG:
		RUN_STATE();
		TO_NEXT_STATE();
		break;

	default:
		RUN_STATE();
		break;

	}
}

} // namespace Edf
