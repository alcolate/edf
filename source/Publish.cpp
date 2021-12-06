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
class CSubscriber
{
public:
	CActive const *  m_Act;
	CSubscriber *m_Next;

	CSubscriber(CActive const *  Act, CSubscriber *Next = 0)
	{
		this->m_Act = Act;
		this->m_Next = Next;
	}

	void Update(const Event * const e, bool FromISR = false)
	{
		QueueSend(m_Act->Q(), e, FromISR);
	}

};

class CPublisher
{
public:
	static CPublisher *Instance();

	void Subscribe(Signal Sig, CActive const * const Act);

	void UnSubscribe(Signal Sig, CActive const * const Act);

	void Publish(Event * const e, bool FromISR = false);



private:
	void AddTail(CSubscriber **Head, CActive const * const Act);

	void AddHead(CSubscriber **Head, CActive const * const Act);

	void Delete(CSubscriber **Head, CActive const * const Act);

private:
	CPublisher();
private:
	CSubscriber *m_Subs[MAX_SIG];

};

} // namespace Edf

namespace Edf
{
CPublisher * CPublisher::Instance()
{
	static CPublisher puber;
	return &puber;
}

void CPublisher::Subscribe(Signal Sig, CActive const * const Act)
{
	ASSERT(Sig < MAX_SIG);

	AddTail(&(m_Subs[Sig]), Act);

}

void CPublisher::UnSubscribe(Signal Sig, CActive const * const Act)
{
	ASSERT(Sig < MAX_SIG);

	Delete(&(m_Subs[Sig]), Act);
}

void CPublisher::Publish(Event * const e, bool FromISR)
{
	ASSERT(e->Sig < MAX_SIG);

	CSubscriber *suber = m_Subs[e->Sig];

	while (suber)
	{
		e->IncRef();
		suber->Update(e, FromISR);
		suber = suber->m_Next;
	}
}


CPublisher::CPublisher()
{
	for (uint32_t i = 0; i < sizeof(m_Subs) / sizeof(m_Subs[0]); i ++)
	{
		m_Subs[i] = 0;
	}
}

void CPublisher::AddTail(CSubscriber **Head, CActive const * const Act)
{
	if (*Head == 0)
	{
		*Head = new CSubscriber(Act);

		return;
	}

	if ((*Head)->m_Act == Act)
	{
		return;
	}

	AddTail(&((*Head)->m_Next), Act);
}

void CPublisher::AddHead(CSubscriber **Head, CActive const * const Act)
{
	CSubscriber *p = *Head;

	for (; p; p = p->m_Next)
	{
		if (p->m_Act == Act)
		{
			return;
		}
	}

	*Head = new CSubscriber(Act, *Head);
}

void CPublisher::Delete(CSubscriber **Head, CActive const * const Act)
{
	CSubscriber *p = *Head, *q = *Head;

	for (; p; q = p, p = p->m_Next)
	{
		if (p->m_Act == Act)
		{
			if (q == p)
			{
				q = p->m_Next;
			}
			else
			{
				q->m_Next = p->m_Next;
			}

			delete p;

			break;
		}
	}
}

} // namespace Edf

namespace Edf
{
void Subscribe(Signal Sig, CActive const * const Act)
{
	Edf::CPublisher::Instance()->Subscribe(Sig, Act);
}

void UnSubscribe(Signal Sig, CActive const * const Act)
{
	Edf::CPublisher::Instance()->UnSubscribe(Sig, Act);
}

void Publish(Event * const e, bool FromISR)
{
	Edf::CPublisher::Instance()->Publish( e, FromISR);
}
}
