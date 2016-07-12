#pragma once
#include <mutex>

struct Data
{
	int id;
};

class ZNode
{
public:
	Data data;
	ZNode *next;
	bool marked;

	ZNode() { next = nullptr; }
	ZNode(int value) {
		next = nullptr;
		data.id = value;
		marked = false;
	}

	ZNode(Data data) {
		next = nullptr;
		this->data.id = data.id;
		marked = false;
	}

	~ZNode() { }

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

ZNode *free_list = nullptr;
std::mutex fl_lock;

class ZList
{
public:
	ZList() {
		mHead.data.id = 0x80000000;
		mTail.data.id = 0x7FFFFFFF;
		mHead.next = &mTail;
	}
	~ZList() { }

	void Initailize()
	{
		ZNode* ptr;
		while (mHead.next != &mTail) {
			ptr = mHead.next;
			mHead.next = mHead.next->next;
			delete ptr;
		}
		return;
	}

	bool Add(Data data)
	{
		ZNode *pred, *curr;

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
				ZNode *newNode = new ZNode(data);
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
		ZNode *pred, *curr;

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
				curr->marked = true;
				pred->next = curr->next;
				
				// delete curr;
				fl_lock.lock();
				curr->next = free_list;
				free_list = curr;
				fl_lock.unlock();

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
		ZNode *curr = &mHead;

		while (curr->data.id < id)
			curr = curr->next;

		return curr->data.id == id && !curr->marked;
	}

private:
	ZNode mHead, mTail;

	bool Validate(ZNode *pred, ZNode *curr) {
		return !pred->marked && !curr->marked && pred->next == curr;
	}
};