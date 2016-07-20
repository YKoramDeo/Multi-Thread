#include <iostream>
#include <thread>
#include <vector>
#include "d:/git/git_multi-thread/source_code/lock-free_synchronization.h"

int main(int argc, char *argv[])
{
	LFList list;

	free_list = new LFNode(MIN_INT);

	list.Initialize();

	int input;
	int key = 0;
	bool loop = true;
	while (loop)
	{
		std::cout << "what do you want?" << std::endl;
		std::cout << "1: ADD, 2: REMOVE, 3: CONTAINS, 4: QUIT :: ";
		std::cin >> input;
		
		switch (input)
		{
		case 1:
			std::cout << "id ? ";
			std::cin >> key;
			Data data;
			data.id = key;
			list.Add(data);
			break;
		case 2:
			std::cout << "id ? ";
			std::cin >> key;
			list.Remove(key);
			break;
		case 3:
			std::cout << "id ? ";
			std::cin >> key;
			list.Contains(key);
			break;
		case 4:
			loop = false;
			break;
		default:
			break;
		}

		list.CheckElement();
	}

	list.CleanElement();

	while (0 != free_list->next)
	{
		LFNode *temp = free_list;
		free_list = free_list->GetNext();
		std::cout << reinterpret_cast<int>(temp) << " ";
		delete temp;
	}
	std::cout << std::endl;

	delete free_list;

	return 0;
}