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
// Hello.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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
    for (int i = 0; i < sizeof(hello) / sizeof(hello[0]); i++)
    {
        hello[i] = new CHello();
        hello[i]->Start();
    }

    CWorld* world[10];
    for (int i = 0; i < sizeof(world)/sizeof(world[0]); i++)
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

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
