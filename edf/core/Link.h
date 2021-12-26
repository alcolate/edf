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

namespace Edf
{

template <class T>
class CLink
{
public:
    CLink() : m_Head(0), m_Tail(0)
    {

    }
	T* Head()
	{
		return m_Head;
	}
	T* Tail()
	{
		return m_Tail;
	}
	T* Next(T* Item)
	{
		return Item->m_Next;
	}
	bool IsExist(bool (*Call)(T* This, T* That), T* That)
	{
		T* p;

		for (p = m_Head; p && !Call(p, That); p = p->m_Next);

		return (p != NULL);
	}

protected:
	void LinkHead(T* Item)
	{
		if (NULL == m_Head)
		{
			m_Head = m_Tail = Item;
		}
		else
		{
			Item->m_Next = m_Head;
			m_Head = Item;
		}
	}
    
	void LinkTail(T* Item)
	{
		if (NULL == m_Head)
		{
			m_Head = m_Tail = Item;
		}
		else
		{
			m_Tail->m_Next = Item;
			m_Tail = m_Tail->m_Next;
		}
	}

	T* UnLinkHead()
	{
		if (NULL == m_Head)
		{
			return NULL;
		}
		else
		{
			T* Item = m_Head;
			m_Head = m_Head->m_Next;

			if (!m_Head)
			{
				m_Tail = m_Head;
			}

			return Item;
		}
	}

	T* UnLinkTail()
	{
		if (NULL == m_Head)
		{
			return NULL;
		}
		else
		{
			T *p, *q;

			for (p = q = m_Head; p->m_Next; q = p, p = p->m_Next);

			if (q == m_Head)
			{
				m_Head = m_Tail = 0;
			}
			else
			{
				m_Tail = q;
				q->m_Next = 0;
			}

			return p;
		}
	}

	T* UnLinkItem(bool (*Call)(T* This, T* That), T* That)
	{
		T *p, *q;

		for (p = q = m_Head; p && !Call(p, That); q = p, p = p->m_Next);

		if (p)
		{
			if (p == m_Head)
			{
				UnLinkHead();
			}
			else if (p == m_Tail)
			{
				UnLinkTail();
			}
			else
			{
				q->m_Next = p->m_Next;
			}

			return p;

		}
		
		return NULL;
	}
private:
    T *m_Head;
    T *m_Tail;

};

template <class T>
class CQueue : public CLink <T>
{
public:
	CQueue(){}

	void Push(T* Item)
	{
		CLink<T>::LinkHead(Item);
	}
	T* Pop()
	{
		return CLink<T>::UnLinkTail();
	}
};

template <class T>
class CList : public CLink <T>
{
public:
	CList() {}
	void AddHead(T* Item)
	{
		CLink<T>::LinkHead(Item);
	}
	void AddTail(T* Item)
	{
		CLink<T>::LinkTail(Item);
	}
	T* RemoveHead()
	{
		return CLink<T>::UnLinkHead();
	}
	T* RemoveTail()
	{
		return CLink<T>::UnLinkTail();
	}
	T* RemoveItem(bool (*Call)(T* This, T* That), T* That)
	{
		return CLink<T>::UnLinkItem(Call, That);
	}
};

template <class T>
class CStack : public CList <T>
{
public:
	CStack() {}

	void Push(T* Item)
	{
		CLink<T>::LinkHead(Item);
	}
	T* Pop()
	{
		return CLink<T>::UnLinkHead();
	}
};

}
