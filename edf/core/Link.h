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

template <typename T>
class CLink
{
protected:
	template <typename CT>
	class CItem
	{
	public:
		CItem(CT *Content) : m_Content(Content), m_Next(nullptr){}
		virtual ~CItem() {}

		CT *m_Content;

		CItem<CT> *m_Next;
	};
public:
	CLink() : m_Head(nullptr), m_Tail(nullptr), m_Count(0)
    {

    }
	inline T *Head()
	{
		return ((m_Head != nullptr)? m_Head->m_Content: nullptr);
	}
	inline T *Tail()
	{
		return ((m_Tail != nullptr) ? m_Tail->m_Content: nullptr);
	}
	inline bool IsEmpty()
	{
		return (m_Head == nullptr);
	}
	inline uint32_t Count()
	{
		return m_Count;
	}
	T *Get(uint32_t Index)
	{
		if (m_Count == 0) return nullptr;

		if (Index >= m_Count) return nullptr;

		if (Index == (m_Count - 1)) return Tail();

		auto p = m_Head;
		uint32_t i = 0;
		for (; p && i != Index ; p = p->m_Next, ++i);

		return p->m_Content;
	}
	T * operator [](uint32_t Index)
	{
		return Get(Index);
	}
	bool IsExist(T *Item)
	{
		return (FindItem(Item) != nullptr);
	}
	template <typename F>
	bool IsExist(F Func)
	{
		return (FindItem(Func) != nullptr);
	}
	T *FindItem(T *Item)
	{
		auto p = m_Head;

		for (; p && p->m_Content != Item; p = p->m_Next);

		return ((p != nullptr) ? p->m_Content: nullptr);
	}
	template <typename F>
	T *FindItem(F Func)
	{
		auto p = m_Head;

		for (; p && !Func(p->m_Content); p = p->m_Next);

		return ((p != nullptr) ? p->m_Content: nullptr);
	}
	template <typename F>
	void Modify(F Func)
	{
		for (auto p = m_Head; p && !Func(&p->m_Content); p = p->m_Next);
	}
	template <typename F>
	void ForEach(F Func)
	{
		for (auto p = m_Head, q = m_Head; p; 
				q = p, p = p->m_Next, Func(q->m_Content));
	}
	template <typename F>
	void Clean(F Func)
	{
		for (auto p = m_Head, q = m_Head; p; 
				q = p, p = p->m_Next, Func(q->m_Content), UnLinkHead());
	}
protected:
	void LinkHead(T *Content)
	{
		ASSERT(Content);

		CItem<T> *linkItem = new CItem<T>(Content);

		if (nullptr == m_Head)
		{
			m_Head = m_Tail = linkItem;
			m_Head->m_Next = nullptr;
		}
		else
		{
			linkItem->m_Next = m_Head;
			m_Head = linkItem;
		}

		++m_Count;
	}
    
	void LinkTail(T *Content)
	{
		ASSERT(Content);

		CItem<T> *linkItem = new CItem<T>(Content);

		if (nullptr == m_Head)
		{
			m_Head = m_Tail = linkItem;
			m_Head->m_Next = nullptr;
		}
		else
		{
			m_Tail->m_Next = linkItem;
			m_Tail = m_Tail->m_Next;
			m_Tail->m_Next = nullptr;
		}

		++m_Count;
	}

	template <typename F>
	void LinkSort(T *Content, F Func)
	{

		ASSERT(Content);		

		if (nullptr == m_Head)
		{
			LinkHead(Content);
			return;
		}

		auto p = m_Head, q = m_Head;

		for (; p && !Func(p->m_Content); q = p, p = p->m_Next);

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
			CItem<T> *linkItem = new CItem<T>(Content);

			linkItem->m_Next = p;
			q->m_Next = linkItem;
			++m_Count;
		}
	}

	T *UnLinkHead()
	{
		if (!m_Head)
		{
			return nullptr;
		}

		T *Content = m_Head->m_Content;

		delete m_Head;

		m_Head = m_Head->m_Next;

		if (nullptr == m_Head)
		{
			m_Tail = m_Head;
		}

		--m_Count;

		return Content;		
	}

	T *UnLinkTail()
	{
		if (!m_Tail)
		{
			return nullptr;
		}

		auto p = m_Head, q = m_Head;

		for (; p != m_Tail; q = p, p = p->m_Next);

		if (q == p)
		{
			m_Head = m_Tail = nullptr;
		}
		else
		{
			m_Tail = q;
			q->m_Next = nullptr;
		}

		--m_Count;

		T *Content = p->m_Content;
		delete p;
		return Content;		
	}
	T *UnLinkItem(CItem<T> *p, CItem<T> *q)
	{
		if (!p)
		{
			return nullptr;
		}

		if (p == m_Head)
		{
			m_Head = m_Head->m_Next;

			if (nullptr == m_Head)
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

		T *Content = p->m_Content;
		delete p;
		return Content;		
	}
	T *UnLinkItem(T *Item)
	{
		ASSERT(Item);

		auto p = m_Head, q = m_Head;

		for (; p && p->m_Content != Item ; q = p, p = p->m_Next);

		return UnLinkItem(p, q);
	}
	template <typename F>
	T *UnLinkItem(F Func)
	{
		auto p = m_Head, q = m_Head;

		for (; p && !Func(p->m_Content); q = p, p = p->m_Next);

		return UnLinkItem(p, q);
	}
private:
	CItem<T> *m_Head;
	CItem<T> *m_Tail;
	uint32_t m_Count;

};

template <typename T>
class CQueue : public CLink<T>
{
public:
	CQueue(){}

	void Push(T *Item)
	{
		CLink<T>::LinkTail(Item);
	}
	T *Pop()
	{
		return CLink<T>::UnLinkHead();
	}
};

template <typename T>
class CDeQueue : public CLink<T>
{
public:
	CDeQueue(){}

	void PushHead(T *Item)
	{
		CLink<T>::LinkHead(Item);
	}
	void PushTail(T *Item)
	{
		CLink<T>::LinkTail(Item);
	}
	T *PopHead()
	{
		return CLink<T>::UnLinkHead();
	}
	T *PopTail()
	{
		return CLink<T>::UnLinkTail();
	}
};

template <typename T>
class CList : public CLink<T>
{
public:
	CList() {}
	void AddHead(T *Item)
	{
		CLink<T>::LinkHead(Item);
	}
	void AddTail(T *Item)
	{
		CLink<T>::LinkTail(Item);
	}
	template <typename F>
	void AddSort(T *Item, F Func)
	{
		CLink<T>::LinkSort(Item, Func);
	}
	T *RemoveHead()
	{
		return CLink<T>::UnLinkHead();
	}
	T *RemoveTail()
	{
		return CLink<T>::UnLinkTail();
	}
	T *RemoveItem(T *Item)
	{
		return CLink<T>::UnLinkItem(Item);
	}
	template <typename F>
	T *RemoveItem(F Func)
	{
		return CLink<T>::UnLinkItem(Func);
	}
};

template <typename T>
class CStack : public CLink<T>
{
public:
	CStack() {}

	void Push(T *Item)
	{
		CLink<T>::LinkHead(Item);
	}
	T *Pop()
	{
		return CLink<T>::UnLinkHead();
	}
};

}
