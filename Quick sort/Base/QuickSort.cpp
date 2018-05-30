﻿#include <omp.h>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

void genRandArray(int* arr, int size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-1000000, 1000000);
	for (int i = 0; i < size; i++)
		arr[i] = dis(gen);
}

void merge(int firstThread, int secThread, int* arr, std::pair<int, int>* ind_array)
{
	int size_1 = ind_array[firstThread].second - ind_array[firstThread].first;
	int size_2 = ind_array[secThread].second - ind_array[secThread].first;
	int* tmparr_1 = new int[size_1];
	for (int i = 0; i < size_1; i++)
	{
		tmparr_1[i] = arr[ind_array[firstThread].first + i];
	}
	int* tmparr_2 = new int[size_2];
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

void quickSort(int* arr, int startInd, int endInd)
{
	if (startInd < endInd)
	{
		int pivot = arr[startInd];
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

void copy(int* copiedArr, int* pasteArr, int size)
{
	for (auto i = 0; i < size; i++)
		pasteArr[i] = copiedArr[i];
}

void sortParallelOpenMP(int* arr, int startInd, int endInd, int numThreads)
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

void sortParallelCppThread(int* arr, int startInd, int endInd, int numThreads)
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

int main()
{
	setlocale(LC_ALL, "Russian");
	int size = 100000000;
	int numThreads = 2;
	int* arr = new int[size];
	genRandArray(arr, size);
	int* parallelArr = new int[size];
	copy(arr, parallelArr, size);
	int* parallelArrCpp = new int[size];
	copy(arr, parallelArrCpp, size);

	auto start = std::chrono::high_resolution_clock::now();
	quickSort(arr, 0, size);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения последовательной части: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " (микросекунды)" << std::endl;
	
	auto startPar = std::chrono::high_resolution_clock::now();
	omp_set_num_threads(numThreads);
	sortParallelOpenMP(parallelArr, 0, size, numThreads);
	auto endPar = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения OpenMP " << std::chrono::duration_cast<std::chrono::microseconds>(endPar - startPar).count() << " (микросекунды)" << std::endl;

	auto startParCpp = std::chrono::high_resolution_clock::now();
	sortParallelCppThread(parallelArrCpp, 0, size, numThreads);
	auto endParCpp = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения Cpp threads " << std::chrono::duration_cast<std::chrono::microseconds>(endParCpp - startParCpp).count() << " (микросекунды)" << std::endl;

	for (int i = 0; i < size; i++)
	{
		if (arr[i] != parallelArr[i])
		{
			std::cout << "OpenMP версия: сортировка выполнена неврено" << std::endl;
			return 1;
		}
		if (arr[i] != parallelArrCpp[i])
		{
			std::cout << "Cpp threads версия: сортировка выполнена неврено" << std::endl;
			return 1;
		}
	}
	
	std::cout << "Результаты совпали" << std::endl;

	return 0;
}