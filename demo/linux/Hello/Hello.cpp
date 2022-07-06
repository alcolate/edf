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

#include <iostream>
#include "hello.h"


int main()
{
    std::cout << "Hello World!\n";
    Edf::EdfStart(MAX_SIG);

    CHello::Instance()->Start();
    CWorld::Instance()->Start();
#if 1
    CHello* hello[10];
    for (size_t i = 0; i < sizeof(hello) / sizeof(hello[0]); i++)
    {
        hello[i] = new CHello();
        hello[i]->Start();
    }

    CWorld* world[10];
    for (size_t i = 0; i < sizeof(world)/sizeof(world[0]); i++)
    {
        world[i] = new CWorld();
        world[i]->Start();
    }
#endif
    while (true)
    {
        OS_Sleep(1000);
    }
}

