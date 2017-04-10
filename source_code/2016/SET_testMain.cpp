#include <iostream>
#include <thread>
#include <vector>
#include "d:/git/git_multi-thread/source_code/lock-free_synchronization.h"


LFList *list;

const int NUM_TEST = 4000000;
const int KEY_RANGE = 1000;

void ThreadFunc(int num_thread)
{
	int key;

	for (int i = 0; i < NUM_TEST / num_thread; ++i)
	{
		key = rand() % KEY_RANGE;
		Data data{ key };
		switch (rand() % 3)
		{
		case 0: list->Add(data); break;
		case 1: list->Remove(key); break;
		case 2: list->Contains(key); break;
		default: std::cout << "Error" << std::endl; exit(-1);
		}
	}
	
	return;
}

int main(int argc, char *argv[])
{
	std::vector<std::thread*> threads;
	list = new LFList();
	free_list = new LFNode(MIN_INT);

	for (int total_num_thread = 1; total_num_thread <= 16; total_num_thread = total_num_thread * 2)
	{
		list->Initialize();
		auto time = std::chrono::high_resolution_clock::now(); // 측정하고 싶은 프로그램을 이곳에 위치시킨다.
		
		for (int num_thread = 0; num_thread < total_num_thread; ++num_thread)
		{
			std::thread *temp_thread = new std::thread{ ThreadFunc, total_num_thread };
			threads.push_back(temp_thread);
		}

		for (auto thread_ptr : threads)
		{
			thread_ptr->join();
			delete thread_ptr;
		}

		auto d = std::chrono::high_resolution_clock::now() - time;
		std::cout << "Total Thread Count = " << total_num_thread << " : ";
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
	delete list;

	return 0;
}