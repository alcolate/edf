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
protected:
	template <class T>
	class CItem
	{
	public:
		CItem(T *Content) : m_Content(Content), m_Next(NULL){}
		virtual ~CItem() {}

		T *m_Content;

		CItem<T> *m_Next;
	};
public:
	CLink() : m_Head(0), m_Tail(0), m_Count(0)
    {

    }
	inline T* Head()
	{
		return ((m_Head != NULL)? m_Head->m_Content: NULL);
	}
	inline T* Tail()
	{
		return ((m_Tail != NULL) ? m_Tail->m_Content: NULL);
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
		return (FindItem(Item) != NULL);
	}
	template <typename F>
	bool IsExist(F Func)
	{
		return (FindItem(Func) != NULL);
	}
	T* FindItem(T* Item)
	{
		CItem<T>* p;

		for (p = m_Head; p && p->m_Content != Item; p = p->m_Next);

		return ((p != NULL) ? p->m_Content: NULL);
	}
	template <typename F>
	T* FindItem(F Func)
	{
		CItem<T>* p;

		for (p = m_Head; p && !Func(p->m_Content); p = p->m_Next);

		return ((p != NULL) ? p->m_Content: NULL);
	}
	template <typename F>
	void ForEach(F Func)
	{
		CItem<T>* p, * q;
		
		for (p = q = m_Head; p; q = p, p = p->m_Next, Func(q->m_Content));
	}

protected:
	void LinkHead(T* Content)
	{
		ASSERT(Content);

		CItem <T> *linkItem = new CItem <T>(Content);

		if (NULL == m_Head)
		{
			m_Head = m_Tail = linkItem;
			m_Head->m_Next = NULL;
		}
		else
		{
			linkItem->m_Next = m_Head;
			m_Head = linkItem;
		}

		++m_Count;
	}
    
	void LinkTail(T* Content)
	{
		ASSERT(Content);

		CItem <T> *linkItem = new CItem <T>(Content);

		if (NULL == m_Head)
		{
			m_Head = m_Tail = linkItem;
			m_Head->m_Next = NULL;
		}
		else
		{
			m_Tail->m_Next = linkItem;
			m_Tail = m_Tail->m_Next;
			m_Tail->m_Next = NULL;
		}

		++m_Count;
	}

	template <typename F>
	void LinkSort(T* Content, F Func)
	{
		CItem <T> *p, *q;

		ASSERT(Content);		

		if (NULL == m_Head)
		{
			LinkHead(Content);
			return;
		}

		for (p = q = m_Head; p && !Func(p->m_Content); q = p, p = p->m_Next);

		if (!p)
		{
			LinkTail(Content);
		}
		else if (q == m_Head && q == p)
		{
			LinkHead(Content);
		}
		else
		{
			CItem <T> *linkItem = new CItem <T>(Content);

			linkItem->m_Next = p;
			q->m_Next = linkItem;
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
			CItem <T>* Item = m_Head;			
			m_Head = m_Head->m_Next;

			if (NULL == m_Head)
			{
				m_Tail = m_Head;
			}

			--m_Count;

			T* Content = Item->m_Content;
			delete Item;
			return Content;
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
			CItem <T> *p, *q;

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

			T* Content = p->m_Content;
			delete p;
			return Content;
		}
	}
	T* UnLinkItem(CItem <T>* p, CItem <T>* q)
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

			T* Content = p->m_Content;
			delete p;
			return Content;

		}

		return NULL;
	}
	T* UnLinkItem(T* Item)
	{
		CItem <T>* p, * q;

		ASSERT(Item);

		for (p = q = m_Head; p && p->m_Content != Item ; q = p, p = p->m_Next);

		return UnLinkItem(p, q);
	}
	template <typename F>
	T* UnLinkItem(F Func)
	{
		CItem <T> *p, *q;

		for (p = q = m_Head; p && !Func(p->m_Content); q = p, p = p->m_Next);

		return UnLinkItem(p, q);
	}
private:
	CItem <T> *m_Head;
	CItem <T> *m_Tail;
	uint32_t m_Count;

};

template <class T>
class CQueue : public CLink <T>
{
public:
	CQueue(){}

	void Push(T* Item)
	{
		CLink<T>::LinkTail(Item);
	}
	T* Pop()
	{
		return CLink<T>::UnLinkHead();
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
