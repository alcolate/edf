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
	queue = QueueCreate(
			EQ_SIZE,            /* queue length - provided by user */
			sizeof(Event *));     /* item size */
	ASSERT(this->queue); /* queue must be created */
}

void CActive::Start()
{
	Start(defPrioity, EQ_SIZE, sizeof(Event *));
}

void CActive::Start(       uint8_t prio,       /* priority (1-based) */
		uint32_t queueLen, uint32_t itemSize)
{

	ASSERT(this->queue); /* queue must be created */

	this->thread = TaskCreate(
			&CActive::EventLoop,        /* the thread function */
			m_Name ,                    /* the name of the task */
			MINIMAL_STACK_SIZE + 128,                /* stack depth */
			this,                       /* the 'pvParameters' parameter */
			prio + tskIDLE_PRIORITY);  /* FreeRTOS priority */

	ASSERT(this->thread); /* thread must be created */
}

void CActive::Post(Event const *const e)
{
	QueueSend(Q(), e);
}

void CActive::EventLoop(void *pvParameters) {
	CActive *act = (CActive *)pvParameters;
	static Event const initEvt = { INIT_SIG };

	ASSERT(act); /* Active object must be provided */

	/* initialize the AO */
	act->Dispatcher(&initEvt);

	for (;;) /* for-ever "superloop" */
	{
		Event *e; /* pointer to event object ("message") */

		/* wait for any event and receive it into object 'e' */
		if (QueueReceive(act->queue, &e, MAX_DELAY)) /* BLOCKING! */
		{
			/* dispatch event to the active object 'act' */
			act->Dispatcher(e); /* NO BLOCKING! */
		}
	}
}


void CActive::Dispatcher(Event const * const e)
{
	switch (e->sig)
	{
	case INIT_SIG: /* intentionally fall through... */
		Initial();

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
