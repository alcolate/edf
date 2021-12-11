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

Event::Event(Signal s, bool DynAlloc) : Sig(s), RefCount(0), DynamicAlloc(DynAlloc) 
{

}
Event::~Event() 
{
    
}
void Event::IncRef(uint32_t Ref, bool FromISR)
{

	uint32_t flag = OS_EnterCritical(FromISR);
    if (DynamicAlloc)
    {        
        RefCount += Ref;
        LOG_DEBUG("event add: %llX,  sig = %d, ref = %d\r\n", (long long)this, Sig, RefCount);
    }    
    OS_ExitCritical(flag, FromISR);
}
void Event::DecRef(void)
{
	bool ToFree = false;
    OS_EnterCritical();
    if (DynamicAlloc)
    {
        RefCount --;
        LOG_DEBUG("event delete: %llX,  sig = %d, ref = %d\r\n", (long long)this, Sig, RefCount);
    
        ToFree = (RefCount == 0);
    }
    OS_ExitCritical();

    if (ToFree)
    {
        delete this;
    }


}

Event const EntryEvent (ENTRY_SIG);
Event const ExitEvent (EXIT_SIG);

}
