/*****************************************************************************
Copyright 2021 The Edf Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Contact information:
<9183399@qq.com>
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
void Event::InitRef(uint32_t Ref, bool FromISR)
{

	uint32_t flag = OS_EnterCritical(FromISR);
	if (DynamicAlloc)
	{
		ASSERT(RefCount == 0);
		RefCount = Ref;
		//LOG_DEBUG("event init: %llX,  sig = %d, ref = %d\r\n", (long long)this, Sig, RefCount);
	}
	OS_ExitCritical(flag, FromISR);
}
void Event::IncRef(uint32_t Ref, bool FromISR)
{

	uint32_t flag = OS_EnterCritical(FromISR);
	if (DynamicAlloc)
	{        
		RefCount += Ref;
		//LOG_DEBUG("event add: %llX,  sig = %d, ref = %d\r\n", (long long)this, Sig, RefCount);
	}    
	OS_ExitCritical(flag, FromISR);
}
void Event::DecRef(bool FromISR)
{
	bool ToFree = false;
	uint32_t flag = OS_EnterCritical(FromISR);
	if (DynamicAlloc)
	{
		if (RefCount)
		{
			RefCount --;
		} 
		if (RefCount == 0)
		{
			ToFree = true;
		}
		//LOG_DEBUG("event delete: %llX,  sig = %d, ref = %d\r\n", (long long)this, Sig, RefCount);
	}
	OS_ExitCritical(flag, FromISR);

	if (ToFree)
	{
		delete this;
	}
}

Event const InitEvt(INIT_SIG);
Event const EntryEvent (ENTRY_SIG);
Event const ExitEvent (EXIT_SIG);

}
