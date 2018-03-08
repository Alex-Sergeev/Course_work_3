#include <omp.h>
#include <iostream>
#include <random>
#include <chrono>

void genRandArray(int* arr, int size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-1000000, 1000000);
	for (int i = 0; i < size; i++)
		arr[i] = dis(gen);
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

void quickSortPar(int* arr, int startInd, int endInd)
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
		#pragma omp parallel sections num_threads(2)
		{
			#pragma omp section
			quickSort(arr, startInd, middleInd);
			#pragma omp section
			quickSort(arr, middleInd + 1, endInd);
		}
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	int size = 100000000;
	int* arr = new int[size];
	genRandArray(arr, size);
	int* parallelArr = new int[size];
	copy(arr, parallelArr, size);
	auto start = std::chrono::high_resolution_clock::now();
	quickSort(arr, 0, size);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " (микросекунды)" << std::endl;
	
	auto startPar = std::chrono::high_resolution_clock::now();

		quickSortPar(parallelArr, 0, size);

	auto endPar = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(endPar - startPar).count() << " (микросекунды)" << std::endl;
	return 0;
}