#pragma once
#pragma once
#include <mutex>

struct Data
{
	int id;
};

class FGNode
{
public:
	Data data;
	FGNode *next;

	FGNode() { next = nullptr; }
	FGNode(int value) {
		next = nullptr;
		data.id = value;
	}

	FGNode(Data data) {
		next = nullptr;
		this->data.id = data.id;
	}

	~FGNode() { }

	void Lock()
	{
		this->lock.lock();
		return;
	}

	void Unlock()
	{
		this->lock.unlock();
		return;
	}

private:
	std::mutex lock;
};

class FGList
{
public:
	FGList() {
		mHead.data.id = 0x80000000;
		mTail.data.id = 0x7FFFFFFF;
		mHead.next = &mTail;
	}
	~FGList() { }

	void Initialize()
	{
		FGNode* ptr;
		while (mHead.next != &mTail) {
			ptr = mHead.next;
			mHead.next = mHead.next->next;
			delete ptr;
		}
		return;
	}

	bool Add(Data data)
	{
		FGNode *pred, *curr;
		mHead.Lock();

		pred = &mHead;
		pred->next->Lock();
		curr = pred->next;

		while (curr->data.id < data.id) {
			pred->Unlock();
			pred = curr;
			curr->next->Lock();
			curr = curr->next;
		}

		if (data.id == curr->data.id) {
			curr->Unlock();
			pred->Unlock();
			return false;
		}
		else {
			FGNode *newNode = new FGNode(data);
			newNode->next = curr;
			pred->next = newNode;
			curr->Unlock();
			pred->Unlock();
			return true;
		}
	}

	bool Remove(Data data)
	{
		FGNode *pred, *curr;
		mHead.Lock();

		pred = &mHead;
		pred->next->Lock();
		curr = pred->next;

		while (curr->data.id < data.id) {
			pred->Unlock();
			pred = curr;
			curr->next->Lock();
			curr = curr->next;
		}

		if (data.id == curr->data.id) {
			pred->next = curr->next;
			delete curr;
			// curr->Unlock();
			// curr은 위에서 지우므로 Pred만 Unlock();
			pred->Unlock();
			return true;
		}
		else {
			curr->Unlock();
			pred->Unlock();
			return false;
		}
	}

	bool Contains(int id)
	{
		FGNode *pred, *curr;
		mHead.Lock();

		pred = &mHead;
		pred->next->Lock();
		curr = pred->next;

		while (curr->data.id < id) {
			pred->Unlock();
			pred = curr;
			curr->next->Lock();
			curr = curr->next;
		}

		if (id == curr->data.id) {
			curr->Unlock();
			pred->Unlock();
			return true;
		}
		else {
			curr->Unlock();
			pred->Unlock();
			return false;
		}
	}
private:
	FGNode mHead, mTail;
};