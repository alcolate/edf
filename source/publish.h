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

#pragma once


namespace Edf
{
class CSubscriber
{
public:
	CActive const *  Act;
	CSubscriber *Next;

	void Update(const Event * const e)
	{
		QueueSend(Act->Q(), e);
	}
	void UpdateFromISR(const Event * const e)
	{
		QueueSend(Act->Q(), e, true);
	}
};

class CPublisher
{
public:
	static CPublisher *Instance();

	void Subscribe(Signal Sig, CActive const * const Act);

	void Post(const Event * const e);

	void PostFromISR(const Event * const e);


private:
	void AddTail(CSubscriber **Head, CActive const * const Act);

	void AddHead(CSubscriber **Head, CActive const * const Act);

private:
	CPublisher();
private:
	CSubscriber *m_Subs[MAX_SIG];

};

} // namespace Edf
