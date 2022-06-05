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
#pragma once

#include "Active.h"

namespace Edf
{

void InitPublish(uint32_t SigNum);

void Subscribe(Signal Sig, CActive const * const Act);

void UnSubscribe(Signal Sig, CActive const * const Act);

void Publish(Event const * const e, bool FromISR = false);

	
} // namespace Edf

