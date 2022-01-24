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
	class CItem
	{
	public:
		CItem(T *Content) : m_Content(Content){}

		T *m_Content;

		CItem *m_Next;
	};
public:
	CLink() : m_Head(0), m_Tail(0), m_Count(0)
    {

    }
	inline T* Head()
	{
		return m_Head;
	}
	inline T* Tail()
	{
		return m_Tail;
	}
	inline T* Next(T* Item)
	{
		return Item->m_Next;
	}
	inline bool IsEmpty()
	{
		return (m_Head == NULL);
	}
	inline uint32_t Count()
	{
		return m_Count;
	}
	bool IsExist(T* Item)
	{
		T* p = FindItem(Item);

		return (p != NULL);
	}
	template <typename F>
	bool IsExist(F Func)
	{
		T* Item = FindItem(Func);

		return (Item != NULL);
	}
	T* FindItem(T* Item)
	{
		T* p;

		for (p = m_Head; p && p != Item; Item = Item->m_Next);

		return p;
	}
	template <typename F>
	T* FindItem(F Func)
	{
		T* Item;

		for (Item = m_Head; Item && !Func(Item); Item = Item->m_Next);

		return Item;
	}
	template <typename F>
	void ForEach(T* From, F Func)
	{
		for (T* Cur = From, *Next = From; Next; Cur = Next, Next = Next->m_Next, Func(Cur));
	}

protected:
	void LinkHead(T* Item)
	{
		ASSERT(Item);

		if (NULL == m_Head)
		{
			m_Head = m_Tail = Item;
			m_Head->m_Next = NULL;
		}
		else
		{
			Item->m_Next = m_Head;
			m_Head = Item;
		}

		++m_Count;
	}
    
	void LinkTail(T* Item)
	{
		ASSERT(Item);

		if (NULL == m_Head)
		{
			m_Head = m_Tail = Item;
			m_Head->m_Next = NULL;
		}
		else
		{
			m_Tail->m_Next = Item;
			m_Tail = m_Tail->m_Next;
			m_Tail->m_Next = NULL;
		}

		++m_Count;
	}

	template <typename F>
	void LinkSort(T* Item, F Func)
	{
		T *p, *q;

		ASSERT(Item);

		if (NULL == m_Head)
		{
			LinkHead(Item);
			return;
		}

		for (p = q = m_Head; p && !Func(p); q = p, p = p->m_Next);

		if (!p)
		{
			LinkTail(Item);
		}
		else if (q == m_Head && q == p)
		{
			LinkHead(Item);
		}
		else
		{
			Item->m_Next = p;
			q->m_Next = Item;
			++m_Count;
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

			if (NULL == m_Head)
			{
				m_Tail = m_Head;
			}

			--m_Count;

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

			for (p = q = m_Head; p != m_Tail; q = p, p = p->m_Next);

			if (q == p)
			{
				m_Head = m_Tail = 0;
			}
			else
			{
				m_Tail = q;
				q->m_Next = 0;
			}

			--m_Count;

			return p;
		}
	}
	T* UnLinkItem(T* p, T* q)
	{
		if (p)
		{
			if (p == m_Head)
			{
				m_Head = m_Head->m_Next;

				if (NULL == m_Head)
				{
					m_Tail = m_Head;
				}
			}
			else  // at least two items on link
			{
				if (p == m_Tail)
				{
					m_Tail = q;
				}

				q->m_Next = p->m_Next;
			}

			--m_Count;

			return p;

		}

		return NULL;
	}
	T* UnLinkItem(T* Item)
	{
		T* p, * q;

		ASSERT(Item);

		for (p = q = m_Head; p && p!= Item ; q = p, p = p->m_Next);

		return UnLinkItem(p, q);
	}
	template <typename F>
	T* UnLinkItem(F Func)
	{
		T *p, *q;

		for (p = q = m_Head; p && !Func(p); q = p, p = p->m_Next);

		return UnLinkItem(p, q);
	}
private:
    T *m_Head;
    T *m_Tail;
	uint32_t m_Count;

};

#define USE_LINK(T)  T* m_Next = NULL

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
class CDeQueue : public CLink <T>
{
public:
	CDeQueue(){}

	void PushHead(T* Item)
	{
		CLink<T>::LinkHead(Item);
	}
	void PushTail(T* Item)
	{
		CLink<T>::LinkTail(Item);
	}
	T* PopHead()
	{
		return CLink<T>::UnLinkHead();
	}
	T* PopTail()
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
	template <typename F>
	void AddSort(T* Item, F Func)
	{
		CLink<T>::LinkSort(Item, Func);
	}
	T* RemoveHead()
	{
		return CLink<T>::UnLinkHead();
	}
	T* RemoveTail()
	{
		return CLink<T>::UnLinkTail();
	}
	T* RemoveItem(T* Item)
	{
		return CLink<T>::UnLinkItem(Item);
	}
	template <typename F>
	T* RemoveItem(F Func)
	{
		return CLink<T>::UnLinkItem(Func);
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
