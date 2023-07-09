// #pragma once
// // Create a queue
// template<typename ENTRY>
// class Queue
// {
// 	friend class Iterator<ENTRY>;
// public:
// 	Queue();
// 	void Add(ENTRY* pEntry);
// 	void Clear();
// 	int GetNumElements() { return m_iNumElements; }
// 	ENTRY* PopFront();
// 	bool IsEmpty() { return m_iNumElements <= 0; }
// private:
// 	Iterator<ENTRY>* m_pHead;
// 	Iterator<ENTRY>* m_pEnd;
// 	int m_iNumElements;
// };
// 
// template<typename ENTRY>
// Queue<ENTRY>::Queue()
// {
// 
// }
// 
// template<typename ENTRY>
// ENTRY* Queue<ENTRY>::PopFront()
// {
// 	if (!m_pHead)
// 	{
// 		return 0;
// 	}
// 
// 	ENTRY* ret = m_pHead->GetValue();
// 	m_pHead = m_pHead->GetNext();
// 	m_iNumElements--;
// 	return ret;
// }
// 
// template<typename ENTRY>
// void Queue<ENTRY>::Clear()
// {
// 	Iterator<ENTRY>* pHead = m_pHead;
// 
// 	while (pHead != NULL)
// 	{
// 		// Clear the element
// 		Iterator<ENTRY>* temp = pHead->GetNext();
// 
// 		SafeReleaseTemplate(pHead);
// 		pHead = temp;
// 	}
// 
// 	m_iNumElements = 0;
// 	m_pHead = m_pEnd = nullptr;
// 	SafeReleaseTemplate(m_pHead);
// 	SafeReleaseTemplate(m_pEnd);
// }
// 
// template<typename ENTRY>
// void Queue<ENTRY>::Add(ENTRY* pEntry)
// {
// 	if (m_pHead == NULL)
// 	{
// 		m_pHead = new Iterator<ENTRY>(pEntry);
// 		m_pEnd = m_pHead;
// 		m_iNumElements++;
// 		return;
// 	}
// 
// 	Iterator<ENTRY>* pIterator = new Iterator<ENTRY>(pEntry);
// 	m_pEnd->m_pNext = pIterator;
// 	m_pEnd = pIterator;
// 	m_iNumElements++;
// }
