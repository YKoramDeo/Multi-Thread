#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <atomic>

using namespace std;
using namespace chrono;

#define THREAD_COUNT 16

mutex mylock;
volatile int gSum;
volatile bool gChoosing[THREAD_COUNT];
volatile int gLabel[THREAD_COUNT];

void InitializeGlobalData()
{
	gSum = 0;

	for (auto i = 0; i < THREAD_COUNT; ++i)
	{
		gChoosing[i] = false;
		gLabel[i] = 0;
	}
	return;
}

int MaxLabel(int _threadNum)
{
	int maxLabel = -1;

	for (auto i = 0; i < _threadNum; ++i)
	{
		if (maxLabel < gLabel[i])
			maxLabel = gLabel[i];
	}

	return maxLabel;
}

bool CompareLabel(int _index, int _myID)
{
	if (gLabel[_index] < gLabel[_myID]) return true;
	if (gLabel[_index] == gLabel[_myID])
		if (_index < _myID)
			return true;

	return false;
}

void LockWithBakery(int _myID, int _threadNum)
{
	__asm mfence;
	gChoosing[_myID] = true;
	__asm mfence;
	gLabel[_myID] = MaxLabel(_threadNum) + 1;
	__asm mfence;
	gChoosing[_myID] = false;
	for (auto j = 0; j < _threadNum; ++j)
	{
		while (gChoosing[j]);
		while ((gLabel[j] != 0) && CompareLabel(j, _myID));
	}
	return;
}

void UnlockWithBakery(int _myID)
{
	__asm mfence;
	gLabel[_myID] = 0;
	return;
}


void ThreadFunc(int _myID, int _threadNum, int _version)
{
	for (auto i = 0; i < 50000000 / _threadNum; ++i)
	{
		if (_version == 0) gSum = gSum + 2;
		else if (_version == 1)
		{
			mylock.lock();
			gSum = gSum + 2;
			mylock.unlock();
		}
		else
		{
			LockWithBakery(_myID, _threadNum);
			gSum = gSum + 2;
			UnlockWithBakery(_myID);
		}

	}

	return;
}

int main()
{
	vector<thread *> TH;

	for (int version = 0; version < 3; ++version)
	{
		// version 0 = no lock
		// version 1 = mutex
		// version 2 = bakery

		for (int threadNum = 1; threadNum <= 16; threadNum *= 2)
		{
			InitializeGlobalData();
			auto start = high_resolution_clock::now();

			for (int i = 1; i <= threadNum; ++i)
				TH.push_back(new thread{ ThreadFunc, i - 1, threadNum, version });

			for (auto th : TH)
			{
				th->join();
				delete th;
			}

			auto du = high_resolution_clock::now() - start;
			cout << "Thread = " << threadNum;
			cout << " / Sum = " << gSum;
			cout << "   " << duration_cast<milliseconds>(du).count() << " msecs\n";

			TH.clear();
		}

		cout << "--------------------------------------------------" << endl;
	}

	return 0;
}
