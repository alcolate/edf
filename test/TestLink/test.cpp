// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//
// The Google C++ Testing and Mocking Framework (Google Test)
//
// This header file defines the public API for Google Test.  It should be
// included by any test program that uses Google Test.
//
// IMPORTANT NOTE: Due to limitation of the C++ language, we have to
// leave some internal implementation details in this header file.
// They are clearly marked by comments like this:
//
//   // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
//
// Such code is NOT meant to be used by a user directly, and is subject
// to CHANGE WITHOUT NOTICE.  Therefore DO NOT DEPEND ON IT in a user
// program!
//
// Acknowledgment: Google Test borrowed the idea of automatic test
// registration from Barthelemy Dagenais' (barthelemy@prologique.com)
// easyUnit framework.

// GOOGLETEST_CM0001 DO NOT DELETE
#include "pch.h"
#include "Link.h"

using namespace Edf;

/*
TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}
*/
class CItem
{
public:
	CItem(int index)
	{
		m_Index = index;
	}

	uint32_t m_Index;
	CItem* m_Next;
};


TEST(TestStack, OneItem)
{
	CStack<CItem> stack;
	CItem item(1);

	stack.Push(&item);

	CItem* pitem;

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 1);
	
	pitem = stack.Pop();
	ASSERT_EQ(pitem, (CItem *)NULL);
}


TEST(TestStack, 5Items_0)
{
	CStack<CItem> stack;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	stack.Push(&item1);
	stack.Push(&item2);
	stack.Push(&item3);
	stack.Push(&item4);
	stack.Push(&item5);

	CItem* pitem;

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 5);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 4);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 3);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = stack.Pop();
	ASSERT_EQ(pitem, (CItem *)NULL);
}

TEST(TestStack, 5Items_1)
{
	CStack<CItem> stack;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	stack.Push(&item1);
	stack.Push(&item2);
	stack.Push(&item3);


	CItem* pitem;

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 3);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	stack.Push(&item4);
	ASSERT_NE(pitem, (CItem *)NULL);
	stack.Push(&item5);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 5);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 4);

	pitem = stack.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = stack.Pop();
	ASSERT_EQ(pitem, (CItem *)NULL);
}


TEST(TestQueue, OneItem)
{
	CQueue<CItem> queue;
	CItem item(1);

	queue.Push(&item);

	CItem* pitem;

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = queue.Pop();
	ASSERT_EQ(pitem, (CItem *)NULL);
}


TEST(TestQueue, 5Items_0)
{
	CQueue<CItem> queue;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	queue.Push(&item1);
	queue.Push(&item2);
	queue.Push(&item3);
	queue.Push(&item4);
	queue.Push(&item5);

	CItem* pitem;

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 3);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 4);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 5);

	pitem = queue.Pop();
	ASSERT_EQ(pitem, (CItem *)NULL);
}

TEST(TestQueue, 5Items_1)
{
	CQueue<CItem> queue;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	queue.Push(&item1);
	queue.Push(&item2);
	queue.Push(&item3);


	CItem* pitem;

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	queue.Push(&item4);
	queue.Push(&item5);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 3);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 4);

	pitem = queue.Pop();
	ASSERT_NE(pitem, (CItem *)NULL);
	ASSERT_EQ(pitem->m_Index, 5);

	pitem = queue.Pop();
	ASSERT_EQ(pitem, (CItem *)NULL);
}



TEST(TestList, OneItem)
{
	CList<CItem> list;
	CItem* pitem;
	CItem item(1);

	list.AddHead(&item);
	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 1);
	pitem = list.RemoveHead();
	ASSERT_EQ(pitem, (CItem*)NULL);


	list.AddHead(&item);
	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 1);
	pitem = list.RemoveHead();
	ASSERT_EQ(pitem, (CItem*)NULL);

	list.AddTail(&item);
	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 1);
	pitem = list.RemoveHead();
	ASSERT_EQ(pitem, (CItem*)NULL);


	list.AddTail(&item);
	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 1);
	pitem = list.RemoveHead();
	ASSERT_EQ(pitem, (CItem*)NULL);
}


TEST(TestList, 5Items_0)
{
	CList<CItem> list;
	CItem* pitem;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	list.AddHead(&item1);
	list.AddHead(&item2);
	list.AddHead(&item3);
	list.AddHead(&item4);
	list.AddHead(&item5);

	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 5);

	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 4);

	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 3);

	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	pitem = list.RemoveHead();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = list.RemoveHead();
	ASSERT_EQ(pitem, (CItem*)NULL);

}

TEST(TestList, 5Items_1)
{
	CList<CItem> list;
	CItem* pitem;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	list.AddHead(&item1);
	list.AddHead(&item2);
	list.AddHead(&item3);
	list.AddHead(&item4);
	list.AddHead(&item5);

	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 1);

	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 3);

	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 4);

	pitem = list.RemoveTail();
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 5);

	pitem = list.RemoveHead();
	ASSERT_EQ(pitem, (CItem*)NULL);

}

TEST(TestList, 5Items_2)
{
	CList<CItem> list;
	CItem* pitem;
	CItem item1(1);
	CItem item2(2);
	CItem item3(3);
	CItem item4(4);
	CItem item5(5);

	list.AddHead(&item1);
	list.AddHead(&item2);
	list.AddHead(&item3);
	list.AddHead(&item4);
	list.AddHead(&item5);

	ASSERT_TRUE(list.IsExist([](CItem* This, CItem* That) -> bool {return This->m_Index == That->m_Index; }, &item2));

	pitem = list.RemoveItem([](CItem* This, CItem* That) -> bool {return This->m_Index == That->m_Index; }, &item2);
	ASSERT_NE(pitem, (CItem*)NULL);
	ASSERT_EQ(pitem->m_Index, 2);

	ASSERT_EQ(list.Count(), 4);

}

