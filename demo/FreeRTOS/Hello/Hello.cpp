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
// Hello.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "hello.h"
extern void vApplicationTickHook(void);

int main()
{
    std::cout << "Hello World!\n";

    CHello::Instance()->Start();
    CWorld::Instance()->Start();
#if 1
    CHello* hello[20];
    for (uint32_t i = 0; i < sizeof(hello) / sizeof(hello[0]); i++)
    {
        hello[i] = new CHello();
        hello[i]->Start();
    }

    CWorld* world[20];
    for (uint32_t i = 0; i < sizeof(world)/sizeof(world[0]); i++)
    {
        world[i] = new CWorld();
        world[i]->Start();
    }
#endif
    vApplicationTickHook();
}

