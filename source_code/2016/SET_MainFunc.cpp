#include <iostream>
#include <thread>
#include <vector>
//#include "coarse-grained_synchronization.h"
//#include "fine-grained_synchronization.h"
//#include "lazy_synchronization.h"
#include "lock-free_synchronization.h"
typedef LFList List;

const int NUM_TEST = 4000000;
const int KEY_RANGE = 1000;

List gList;

void ThreadFunc(int num_thread)
{
	int key;

	for (int i = 0; i < NUM_TEST / num_thread; ++i)
	{
		key = rand() % KEY_RANGE;
		Data data{ key };
		switch (rand() % 3)
		{
		case 0: gList.Add(data); break;
		case 1: gList.Remove(key); break;
		case 2: gList.Contains(key); break;
		default: std::cout << "Error" << std::endl; exit(-1);
		}
	}
}

int main(int argc, char *argv[]) {
	std::vector<std::thread*> threads;
	
	free_list = new LFNode(MIN_INT);

	for (int num_thread = 1; num_thread <= 16; num_thread = num_thread * 2)
	{
		gList.Initialize();
		auto time = std::chrono::high_resolution_clock::now(); // 측정하고 싶은 프로그램을 이곳에 위치시킨다.

		for (int count = 0; count < num_thread; ++count)
		{
			std::thread* tmpThread = new std::thread{ ThreadFunc, num_thread };
			threads.push_back(tmpThread);
		}

		for (auto tp : threads)
		{
			tp->join();
			delete tp;
		}

		auto d = std::chrono::high_resolution_clock::now() - time;
		std::cout << "Total Thread Count = " << num_thread << " : ";
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(d).count() << " msecs" << std::endl;

		threads.clear();
		while (0 != free_list->next)
		{
			LFNode *temp = free_list;
			free_list = free_list->GetNext();
			delete temp;
		}

	}
	
	delete free_list;

	return 0;
}