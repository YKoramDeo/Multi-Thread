#pragma once
#include <mutex>

struct Data
{
	int id;
};

class ONode
{
public:
	Data data;
	ONode *next;

	ONode() { next = nullptr; }
	ONode(int value) {
		next = nullptr;
		data.id = value;
	}

	ONode(Data data) {
		next = nullptr;
		this->data.id = data.id;
	}

	~ONode() { }

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

class OList
{
public:
	OList() {
		mHead.data.id = 0x80000000;
		mTail.data.id = 0x7FFFFFFF;
		mHead.next = &mTail;
	}
	~OList() { }

	void Initialize()
	{
		ONode* ptr;
		while (mHead.next != &mTail) {
			ptr = mHead.next;
			mHead.next = mHead.next->next;
			delete ptr;
		}
		return;
	}

	bool Add(Data data)
	{
		ONode *pred, *curr;

		while (true) {
			pred = &mHead;
			curr = pred->next;

			while (curr->data.id < data.id) {
				pred = curr;
				curr = curr->next;
			}
			pred->Lock(); curr->Lock();

			if (!Validate(pred, curr)) {
				curr->Unlock();
				pred->Unlock();
				continue;
			}

			if (data.id == curr->data.id) {
				curr->Unlock();
				pred->Unlock();
				return false;
			}
			else {
				ONode *newNode = new ONode(data);
				newNode->next = curr;
				pred->next = newNode;
				curr->Unlock();
				pred->Unlock();
				return true;
			}
		}
	}

	bool Remove(Data data)
	{
		ONode *pred, *curr;

		while (true) {
			pred = &mHead;
			curr = pred->next;

			while (curr->data.id < data.id) {
				pred = curr;
				curr = curr->next;
			}
			pred->Lock(); curr->Lock();

			if (!Validate(pred, curr)) {
				curr->Unlock();
				pred->Unlock();
				continue;
			}

			if (data.id == curr->data.id) {
				pred->next = curr->next;
				// delete curr;
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
	}

	bool Contains(int id)
	{
		ONode *pred, *curr;
		while (true) {
			pred = &mHead;
			curr = pred->next;

			while (curr->data.id < id) {
				pred = curr;
				curr = curr->next;
			}
			pred->Lock(); curr->Lock();

			if (!Validate(pred, curr)) {
				curr->Unlock();
				pred->Unlock();
				continue;
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
	}

private:
	ONode mHead, mTail;

	bool Validate(ONode *pred, ONode *curr) {
		ONode *ptr = &mHead;
		while (ptr->data.id < pred->data.id) ptr = ptr->next;
		if (ptr == pred)
			return ptr->next == curr;
		return false;
	}
};