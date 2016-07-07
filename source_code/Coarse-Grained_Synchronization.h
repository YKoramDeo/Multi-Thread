#pragma once
#include <mutex>

struct Data
{
	int id;
};

class CGNode
{
public:
	Data data;
	CGNode *next;

	CGNode() { next = nullptr; }
	CGNode(int value) {
		next = nullptr;
		data.id = value;
	}

	CGNode(Data data){
		next = nullptr;
		this->data.id = data.id;
	}

	CGNode() { }
};

class CGList
{
public:
	CGList() {
		mHead.data.id = 0x80000000;
		mTail.data.id = 0x7FFFFFFF;
		mHead.next = &mTail;
	}
	~CGList() { }

	void Initailize()
	{
		CGNode* ptr;
		while (mHead.next != &mTail) {
			ptr = mHead.next;
			mHead.next = mHead.next->next;
			delete ptr;
		}
		return;
	}

	bool Add(Data data)
	{
		CGNode *pred, *curr;

		pred = &mHead;
		this->mLock.lock();
		curr = pred->next;
		while (curr->data.id < data.id) {
			pred = curr;
			curr = curr->next;
		}

		if (data.id == curr->data.id) {
			this->mLock.unlock();
			return false;
		}
		else {
			CGNode *node = new CGNode(data);
			node->next = curr;
			pred->next = node;
			this->mLock.unlock();
			return true;
		}
	}

	bool Remove(Data data)
	{
		CGNode *pred, *curr;
		
		pred = &mHead;
		this->mLock.lock();
		curr = pred->next;
		while (curr->data.id < data.id) {
			pred = curr;
			curr = curr->next;
		}

		if (data.id == curr->data.id) {
			pred->next = curr->next;
			delete curr;
			this->mLock.unlock();
			return true;
		}
		else {
			this->mLock.unlock();
			return false;
		}
	}

	bool Contains(int id)
	{
		CGNode *pred, *curr;
		pred = &mHead;
		this->mLock.lock();
		curr = pred->next;
		while (curr->data.id < id) {
			pred = curr;
			curr = curr->next;
		}

		if (id == curr->data.id) {
			this->mLock.unlock();
			return true;
		}
		else {
			this->mLock.unlock();
			return false;
		}
	}
private:
	CGNode mHead, mTail;
	std::mutex mLock;
};