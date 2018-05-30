#include <stack>
#include <omp.h>
#include "Dot.h"
#include <thread>

void merge(int firstThread, int secThread, dot* arr, std::pair<int, int>* ind_array)
{
	int size_1 = ind_array[firstThread].second - ind_array[firstThread].first;
	int size_2 = ind_array[secThread].second - ind_array[secThread].first;
	dot* tmparr_1 = new dot[size_1];
	for (int i = 0; i < size_1; i++)
	{
		tmparr_1[i] = arr[ind_array[firstThread].first + i];
	}
	dot* tmparr_2 = new dot[size_2];
	for (int i = 0; i < size_2; i++)
	{
		tmparr_2[i] = arr[ind_array[secThread].first + i];
	}

	int iter1 = 0;
	int iter2 = 0;
	while ((iter1 != size_1) && (iter2 != size_2))
	{
		if (tmparr_1[iter1] < tmparr_2[iter2])
		{
			arr[ind_array[firstThread].first + iter1 + iter2] = tmparr_1[iter1];
			iter1++;
		}
		else
		{
			arr[ind_array[firstThread].first + iter1 + iter2] = tmparr_2[iter2];
			iter2++;
		}
	}
	if (iter1 == size_1)
	{
		for (int i = iter2; i < size_2; i++)
		{
			arr[ind_array[firstThread].first + i + size_1] = tmparr_2[i];
		}
	}
	else if (iter2 == size_2)
	{
		for (int i = iter1; i < size_1; i++)
		{
			arr[ind_array[firstThread].first + i + size_2] = tmparr_1[i];
		}
	}
	ind_array[firstThread].second = ind_array[secThread].second;
	ind_array[secThread].first = 0;
	ind_array[secThread].second = 0;

	delete[] tmparr_1;
	delete[] tmparr_2;
}

void mergeForMin(int firstThread, int secThread, dot* arr, std::pair<int, int>* ind_array)
{
	int size_1 = ind_array[firstThread].second - ind_array[firstThread].first;
	int size_2 = ind_array[secThread].second - ind_array[secThread].first;
	dot* tmparr_1 = new dot[size_1];
	for (int i = 0; i < size_1; i++)
	{
		tmparr_1[i] = arr[ind_array[firstThread].first + i];
	}
	dot* tmparr_2 = new dot[size_2];
	for (int i = 0; i < size_2; i++)
	{
		tmparr_2[i] = arr[ind_array[secThread].first + i];
	}

	int iter1 = 0;
	int iter2 = 0;
	while ((iter1 != size_1) && (iter2 != size_2))
	{
		if ((tmparr_1[iter1].x < tmparr_2[iter2].x) || ((tmparr_1[iter1].x == tmparr_2[iter2].x) && (tmparr_1[iter1].y < tmparr_2[iter2].y)))
		{
			arr[ind_array[firstThread].first + iter1 + iter2] = tmparr_1[iter1];
			iter1++;
		}
		else
		{
			arr[ind_array[firstThread].first + iter1 + iter2] = tmparr_2[iter2];
			iter2++;
		}
	}
	if (iter1 == size_1)
	{
		for (int i = iter2; i < size_2; i++)
		{
			arr[ind_array[firstThread].first + i + size_1] = tmparr_2[i];
		}
	}
	else if (iter2 == size_2)
	{
		for (int i = iter1; i < size_1; i++)
		{
			arr[ind_array[firstThread].first + i + size_2] = tmparr_1[i];
		}
	}
	ind_array[firstThread].second = ind_array[secThread].second;
	ind_array[secThread].first = 0;
	ind_array[secThread].second = 0;

	delete[] tmparr_1;
	delete[] tmparr_2;
}

void quickSort(dot* arr, int startInd, int endInd)
{
	if (startInd < endInd)
	{
		dot pivot = arr[startInd];
		int middleInd = startInd;
		for (auto i = startInd + 1; i < endInd; i++)
			if (arr[i] < pivot)
			{
				middleInd++;
				std::swap(arr[middleInd], arr[i]);
			}
		std::swap(arr[startInd], arr[middleInd]);
		quickSort(arr, startInd, middleInd);
		quickSort(arr, middleInd + 1, endInd);
	}
}

void sortParallelOpenMP(dot* arr, int startInd, int endInd, int numThreads)
{
	if (endInd - startInd < numThreads * 2) //Если размер массива меньше двойного кол-ва потоков выполняется сортировка всего массива в одном потоке
	{
		quickSort(arr, startInd, endInd);
		return;
	}
	std::pair<int, int>* ind_array = new std::pair<int, int>[numThreads];
	std::div_t portion = div((endInd - startInd), numThreads); // size / num_threads
	ind_array[0].first = startInd;
	ind_array[0].second = ind_array[0].first + portion.quot;
	if (portion.rem > 0)
		ind_array[0].second++;
	for (int i = 1; i < numThreads; i++)
	{
		ind_array[i].first = ind_array[i - 1].second;
		ind_array[i].second = ind_array[i].first + portion.quot;
		if (portion.rem > i)
			ind_array[i].second++;
	}//закончено распределение индексов

	int steps_amount = std::ceil(log2(numThreads)); // расчёт необходимого числа шагов в цикле слияний массивов 

#pragma omp parallel
	{
		int thread_num = omp_get_thread_num();
		quickSort(arr, ind_array[thread_num].first, ind_array[thread_num].second);
#pragma omp barrier
		for (int i = 0; i < steps_amount; i++)
		{
			if (thread_num % (int)(pow(2, i + 1)) == 0) //Если поток ведущий - сделать слияние с "соседним" потоком
			{
				if (thread_num + (int)pow(2, i) < numThreads) // Если поток с которым происходит слияние существует
				{
					merge(thread_num, thread_num + (int)pow(2, i), arr, ind_array);// Слить массив первого потока с массивом второго
				}
			}
#pragma omp barrier
		}
	}

	delete[] ind_array;
}
void quickSortSearchMin(dot* arr, int startInd, int endInd)
{
	if (startInd < endInd)
	{
		dot pivot = arr[startInd];
		int middleInd = startInd;
		for (auto i = startInd + 1; i < endInd; i++)
			if ((arr[i].x < pivot.x) || ((arr[i].x == pivot.x) && (arr[i].y < pivot.y)))
			{
				middleInd++;
				std::swap(arr[middleInd], arr[i]);
			}
		std::swap(arr[startInd], arr[middleInd]);
		quickSortSearchMin(arr, startInd, middleInd);
		quickSortSearchMin(arr, middleInd + 1, endInd);
	}
}

void searchMinElement(dot* dotArray, int size)
{
	quickSortSearchMin(dotArray, 0, size);
}

void searchMinElementOpenMP(dot* dotArray, int size, int numThreads)
{
	if (size < numThreads * 2) //Если размер массива меньше двойного кол-ва потоков выполняется сортировка всего массива в одном потоке
	{
		quickSortSearchMin(dotArray, 0, size);
		return;
	}
	std::pair<int, int>* ind_array = new std::pair<int, int>[numThreads];
	std::div_t portion = div(size, numThreads); // size / num_threads
	ind_array[0].first = 0;
	ind_array[0].second = portion.quot;
	if (portion.rem > 0)
		ind_array[0].second++;
	for (int i = 1; i < numThreads; i++)
	{
		ind_array[i].first = ind_array[i - 1].second;
		ind_array[i].second = ind_array[i].first + portion.quot;
		if (portion.rem > i)
			ind_array[i].second++;
	}//закончено распределение индексов

	int steps_amount = std::ceil(log2(numThreads)); // расчёт необходимого числа шагов в цикле слияний массивов 

#pragma omp parallel
	{
		int thread_num = omp_get_thread_num();
		quickSortSearchMin(dotArray, ind_array[thread_num].first, ind_array[thread_num].second);
#pragma omp barrier
		for (int i = 0; i < steps_amount; i++)
		{
			if (thread_num % (int)(pow(2, i + 1)) == 0) //Если поток ведущий - сделать слияние с "соседним" потоком
			{
				if (thread_num + (int)pow(2, i) < numThreads) // Если поток с которым происходит слияние существует
				{
					mergeForMin(thread_num, thread_num + (int)pow(2, i), dotArray, ind_array);// Слить массив первого потока с массивом второго
				}
			}
#pragma omp barrier
		}
	}

	delete[] ind_array;
}

std::pair<std::deque<dot>, int> grehemMethod(dot* dotArray, int size)
{
	searchMinElement(dotArray, size - 1);
	dot move = dotArray[0];
	for (auto i = 0; i < size - 1; i++)
		dotArray[i] = dotArray[i] - move;
	quickSort(dotArray, 1, size - 1);
	dotArray[size - 1] = dotArray[0];
	std::stack<dot> dotStack;
	dot dotY, dotX;
	dotStack.push(dotArray[0]);
	dotStack.push(dotArray[1]);
	for (auto i = 2; i < size; i++)
	{
		while (true)
		{
			if (dotStack.size() > 1)
			{
				dotY = dotStack.top();
				dotStack.pop();
				dotX = dotStack.top();
				if ((dotArray[i].x*(dotX.y - dotY.y) + dotArray[i].y*(dotY.x - dotX.x) + dotX.x*dotY.y - dotX.y*dotY.x) > 0)
				{
					dotStack.push(dotY);
					dotStack.push(dotArray[i]);
					break;
				}
				else
				{
					if (dotStack.size() == 1)
					{
						dotStack.push(dotArray[i]);
						break;
					}
				}
			}
		}
	}
	std::deque<dot> resultArray = dotStack._Get_container();
	for (auto i = 0; i < resultArray.size(); i++)
	{
		resultArray[i] = resultArray[i] + move;
	}
	return std::make_pair(resultArray, resultArray.size());
}

std::pair<std::deque<dot>, int> grehemMethodOpenMP(dot* dotArray, int size, int numThreads)
{
	searchMinElementOpenMP(dotArray, size - 1, numThreads);
	dot move = dotArray[0];
#pragma omp parallel for
	for (auto i = 0; i < size - 1; i++)
		dotArray[i] = dotArray[i] - move;
	sortParallelOpenMP(dotArray, 1, size - 1, numThreads);
	dotArray[size - 1] = dotArray[0];
	std::stack<dot> dotStack;
	dot dotY, dotX;
	dotStack.push(dotArray[0]);
	dotStack.push(dotArray[1]);
	for (auto i = 2; i < size; i++)
	{
		while (true)
		{
			if (dotStack.size() > 1)
			{
				dotY = dotStack.top();
				dotStack.pop();
				dotX = dotStack.top();
				if ((dotArray[i].x*(dotX.y - dotY.y) + dotArray[i].y*(dotY.x - dotX.x) + dotX.x*dotY.y - dotX.y*dotY.x) > 0)
				{
					dotStack.push(dotY);
					dotStack.push(dotArray[i]);
					break;
				}
				else
				{
					if (dotStack.size() == 1)
					{
						dotStack.push(dotArray[i]);
						break;
					}
				}
			}
		}
	}
	std::deque<dot> resultArray = dotStack._Get_container();
#pragma omp parallel for
	for (auto i = 0; i < resultArray.size(); i++)
		resultArray[i] = resultArray[i] + move;
	return std::make_pair(resultArray, resultArray.size());
}

void sortParallelCppThread(dot* arr, int startInd, int endInd, int numThreads)
{
	if (endInd - startInd < numThreads * 2) //Если размер массива меньше двойного кол-ва потоков выполняется сортировка всего массива в одном потоке
	{
		quickSort(arr, startInd, endInd);
		return;
	}
	std::pair<int, int>* ind_array = new std::pair<int, int>[numThreads];
	std::div_t portion = div((endInd - startInd), numThreads); // size / num_threads
	ind_array[0].first = startInd;
	ind_array[0].second = ind_array[0].first + portion.quot;
	if (portion.rem > 0)
		ind_array[0].second++;
	for (int i = 1; i < numThreads; i++)
	{
		ind_array[i].first = ind_array[i - 1].second;
		ind_array[i].second = ind_array[i].first + portion.quot;
		if (portion.rem > i)
			ind_array[i].second++;
	}//закончено распределение индексов

	int steps_amount = std::ceil(log2(numThreads)); // расчёт необходимого числа шагов в цикле слияний массивов 

	std::thread* threadArr = new std::thread[numThreads - 1];
	for (int i = 1; i < numThreads; i++)
	{
		threadArr[i - 1] = std::thread(quickSort, arr, ind_array[i].first, ind_array[i].second);
	}
	quickSort(arr, ind_array[0].first, ind_array[0].second);
	for (int i = 1; i < numThreads; i++)
	{
		threadArr[i - 1].join();
	}
	
	int currentWorkingThreads;
	for (int i = 0; i < steps_amount; i++)
	{
		currentWorkingThreads = 0;
		for (int thrNum = 0; thrNum < numThreads - 1; thrNum++)
		{
			if (thrNum % (int)(pow(2, i + 1)) == 0) //Если поток ведущий - сделать слияние с "соседним" потоком
			{
				if (thrNum + (int)pow(2, i) < numThreads) // Если поток с которым происходит слияние существует
				{
					threadArr[currentWorkingThreads] = std::thread(merge, thrNum, thrNum + (int)pow(2, i), arr, ind_array);// Слить массив первого потока с массивом второго
					currentWorkingThreads++;
				}
			}
		}
		for (int k = 0; k < currentWorkingThreads; k++)
		{
			threadArr[k].join();
		}
	}

	delete[] threadArr;
	delete[] ind_array;
}

void searchMinElementCppThread(dot* dotArray, int size, int numThreads)
{
	if (size < numThreads * 2) //Если размер массива меньше двойного кол-ва потоков выполняется сортировка всего массива в одном потоке
	{
		quickSortSearchMin(dotArray, 0, size);
		return;
	}
	std::pair<int, int>* ind_array = new std::pair<int, int>[numThreads];
	std::div_t portion = div(size, numThreads); // size / num_threads
	ind_array[0].first = 0;
	ind_array[0].second = portion.quot;
	if (portion.rem > 0)
		ind_array[0].second++;
	for (int i = 1; i < numThreads; i++)
	{
		ind_array[i].first = ind_array[i - 1].second;
		ind_array[i].second = ind_array[i].first + portion.quot;
		if (portion.rem > i)
			ind_array[i].second++;
	}//закончено распределение индексов

	int steps_amount = std::ceil(log2(numThreads)); // расчёт необходимого числа шагов в цикле слияний массивов 

	std::thread* threadArr = new std::thread[numThreads - 1];
	for (int i = 1; i < numThreads; i++)
	{
		threadArr[i - 1] = std::thread(quickSortSearchMin, dotArray, ind_array[i].first, ind_array[i].second);
	}
	quickSortSearchMin(dotArray, ind_array[0].first, ind_array[0].second);
	for (int i = 1; i < numThreads; i++)
	{
		threadArr[i - 1].join();
	}
	
	int currentWorkingThreads;
	for (int i = 0; i < steps_amount; i++)
	{
		currentWorkingThreads = 0;
		for (int thrNum = 0; thrNum < numThreads - 1; thrNum++)
		{
			if (thrNum % (int)(pow(2, i + 1)) == 0) //Если поток ведущий - сделать слияние с "соседним" потоком
			{
				if (thrNum + (int)pow(2, i) < numThreads) // Если поток с которым происходит слияние существует
				{
					threadArr[currentWorkingThreads] = std::thread(mergeForMin, thrNum, thrNum + (int)pow(2, i), dotArray, ind_array);// Слить массив первого потока с массивом второго
					currentWorkingThreads++;
				}
			}
		}
		for (int k = 0; k < currentWorkingThreads; k++)
		{
			threadArr[k].join();
		}
	}

	delete[] threadArr;
	delete[] ind_array;
}

std::pair<std::deque<dot>, int> grehemMethodCppThread(dot* dotArray, int size, int numThreads)
{
	searchMinElementCppThread(dotArray, size - 1, numThreads);
	dot move = dotArray[0];
	for (auto i = 0; i < size - 1; i++)
		dotArray[i] = dotArray[i] - move;
	sortParallelCppThread(dotArray, 1, size - 1, numThreads);
	dotArray[size - 1] = dotArray[0];
	std::stack<dot> dotStack;
	dot dotY, dotX;
	dotStack.push(dotArray[0]);
	dotStack.push(dotArray[1]);
	for (auto i = 2; i < size; i++)
	{
		while (true)
		{
			if (dotStack.size() > 1)
			{
				dotY = dotStack.top();
				dotStack.pop();
				dotX = dotStack.top();
				if ((dotArray[i].x*(dotX.y - dotY.y) + dotArray[i].y*(dotY.x - dotX.x) + dotX.x*dotY.y - dotX.y*dotY.x) > 0)
				{
					dotStack.push(dotY);
					dotStack.push(dotArray[i]);
					break;
				}
				else
				{
					if (dotStack.size() == 1)
					{
						dotStack.push(dotArray[i]);
						break;
					}
				}
			}
		}
	}
	std::deque<dot> resultArray = dotStack._Get_container();
	for (auto i = 0; i < resultArray.size(); i++)
	{
		resultArray[i] = resultArray[i] + move;
	}
	return std::make_pair(resultArray, resultArray.size());
}