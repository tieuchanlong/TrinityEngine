#pragma once
template<typename ENTRY>
class List;

template<typename ENTRY>
class Queue;

template<typename ENTRY>
class Iterator
{
	friend class List<ENTRY>;
	friend class Queue<ENTRY>;
public:
	Iterator(ENTRY* pValue);
	Iterator* GetNext() { return m_pNext; }
	ENTRY* GetValue() { return m_pValue; }
private:
	Iterator* m_pNext;
	ENTRY* m_pValue;
};

// Create a linked list
template<typename ENTRY>
class List
{
	friend class Iterator<ENTRY>;
public:
	List();
	Iterator<ENTRY>* GetHead() { return m_pHead; }
	void Add(ENTRY* pEntry);
	void AddFront(ENTRY* pEntry);
	bool Remove(ENTRY* pEntry);
	void Clear();
	int GetNumElements() { return m_iNumElements; }
	Iterator<ENTRY>* GetElementAtIndex(int i)
	{
		int iIndex = 0;

		for (Iterator<ENTRY>* it = GetHead(); it != NULL; it = it->GetNext())
		{
			if (iIndex++ == i)
			{
				return it;
			}
		}
	}
private:
	Iterator<ENTRY>* m_pHead;
	Iterator<ENTRY>* m_pEnd;
	int m_iNumElements;
};

// Create a queue
template<typename ENTRY>
class Queue
{
	friend class Iterator<ENTRY>;
public:
	Queue();
	void Add(ENTRY* pEntry);
	void Clear();
	int GetNumElements() { return m_iNumElements; }
	ENTRY* PopFront();
	bool IsEmpty() { return m_iNumElements <= 0; }
private:
	Iterator<ENTRY>* m_pHead;
	Iterator<ENTRY>* m_pEnd;
	int m_iNumElements;
};

template<typename ENTRY>
List<ENTRY>::List()
{
	m_pHead = m_pEnd = NULL;
	m_iNumElements = 0;
}

template<typename ENTRY>
void List<ENTRY>::Add(ENTRY* pEntry)
{
	if (m_pHead == NULL)
	{
		m_pHead = new Iterator<ENTRY>(pEntry);
		m_pEnd = m_pHead;
		m_iNumElements++;
		return;
	}

	Iterator<ENTRY>* pIterator = new Iterator<ENTRY>(pEntry);
	m_pEnd->m_pNext = pIterator;
	m_pEnd = pIterator;
	m_iNumElements++;
}

template<typename ENTRY>
void List<ENTRY>::AddFront(ENTRY* pEntry)
{
	if (m_pHead == NULL)
	{
		m_pHead = new Iterator<ENTRY>(pEntry);
		m_pEnd = m_pHead;
		m_iNumElements++;
		return;
	}

	Iterator<ENTRY>* pIterator = new Iterator<ENTRY>(pEntry);
	pIterator->m_pNext = m_pHead;
	m_pHead = pIterator;

	m_iNumElements++;
}


template<typename ENTRY>
bool List<ENTRY>::Remove(ENTRY* pEntry)
{
	Iterator<ENTRY>* pHead = m_pHead;

	// In case the head is the entry, set the new head as the next one
	if (m_pHead->GetValue() == pEntry)
	{
		if (m_pHead == m_pEnd)
		{
			// In case of only 1 element, set the header and end to null
			SafeReleaseTemplate(m_pHead);
			m_pHead = m_pEnd = 0;
			m_iNumElements = 0;
			return true;
		}

		m_pHead = pHead->m_pNext;
		pHead->m_pValue = 0;
		SafeReleaseTemplate(pHead);
		m_iNumElements--;
		return true;
	}

	while (pHead != NULL)
	{
		Iterator<ENTRY>* temp = pHead->GetNext();

		if (temp != NULL && temp->GetValue() == pEntry)
		{
			if (temp == m_pEnd)
			{
				// In case we are removing the last element of the list, set the end node as the current node
				m_pEnd = pHead;
			}

			pHead->m_pNext = temp->m_pNext;
			m_iNumElements--;
			return true;
		}

		pHead = temp;
	}

	return false;
}

template<typename ENTRY>
void List<ENTRY>::Clear()
{
	Iterator<ENTRY>* pHead = m_pHead;

	while (pHead != NULL)
	{
		// Clear the element
		Iterator<ENTRY>* temp = pHead->GetNext();
		SafeReleaseTemplate(pHead);
		pHead = temp;
	}

	m_iNumElements = 0;
	m_pHead = m_pEnd = 0;
}

template<typename ENTRY>
Queue<ENTRY>::Queue()
{

}

template<typename ENTRY>
ENTRY* Queue<ENTRY>::PopFront()
{
	if (!m_pHead)
	{
		return 0;
	}

	ENTRY* ret = m_pHead->GetValue();
	m_pHead = m_pHead->GetNext();
	m_iNumElements--;
	return ret;
}

template<typename ENTRY>
void Queue<ENTRY>::Clear()
{
	Iterator<ENTRY>* pHead = m_pHead;

	while (pHead != NULL)
	{
		// Clear the element
		Iterator<ENTRY>* temp = pHead->GetNext();

		SafeReleaseTemplate(pHead);
		pHead = temp;
	}

	m_iNumElements = 0;
	m_pHead = m_pEnd = nullptr;
	SafeReleaseTemplate(m_pHead);
	SafeReleaseTemplate(m_pEnd);
}

template<typename ENTRY>
void Queue<ENTRY>::Add(ENTRY* pEntry)
{
	if (m_pHead == NULL)
	{
		m_pHead = new Iterator<ENTRY>(pEntry);
		m_pEnd = m_pHead;
		m_iNumElements++;
		return;
	}

	Iterator<ENTRY>* pIterator = new Iterator<ENTRY>(pEntry);
	m_pEnd->m_pNext = pIterator;
	m_pEnd = pIterator;
	m_iNumElements++;
}

template<typename ENTRY>
Iterator<ENTRY>::Iterator(ENTRY* pEntry)
{
	m_pValue = pEntry;
	m_pNext = NULL;
}