#include <omp.h>
#include <iostream>
#include <random>
#include <chrono>
#include <math.h>
#include "Grehem_Methods.h"

void copy(dot* copiedArr, dot* pasteArr, int size)
{
	for (auto i = 0; i < size; i++)
		pasteArr[i] = copiedArr[i];
}

void genRandArray(dot* arr, int size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-10000, 10000);
	for (int i = 0; i < size; i++)
	{
		arr[i].x = dis(gen);
		arr[i].y = dis(gen);
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	int size = 1000000;
	int numThreads = 8;
	dot* arr = new dot[size + 1];
	genRandArray(arr, size);

	dot* parallelArr = new dot[size + 1];
	copy(arr, parallelArr, size);
	dot* parallelCppThread = new dot[size + 1];
	copy(arr, parallelCppThread, size);

	auto start = std::chrono::high_resolution_clock::now();
	std::pair<std::deque<dot>, int> answer = grehemMethod(arr, size + 1);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " (микросекунды)" << std::endl;
	

	auto startPar = std::chrono::high_resolution_clock::now();
	std::pair<std::deque<dot>, int> answer_OpenMP = grehemMethodOpenMP(parallelArr, size + 1);
	auto endPar = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(endPar - startPar).count() << " (микросекунды)" << std::endl;


	auto startPar_cpp = std::chrono::high_resolution_clock::now();
	std::pair<std::deque<dot>, int> answer_cpp_thread = grehemMethodCppThread(parallelCppThread, size + 1, numThreads);
	auto endPar_cpp = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(endPar_cpp - startPar_cpp).count() << " (микросекунды)" << std::endl;

	if (answer.second != answer_OpenMP.second)
		std::cout << "Параллельный результат OpenMp неверен: количество точек в выпуклых оболочках не совпадают" << std::endl;

	if (answer.second != answer_cpp_thread.second)
		std::cout << "Параллельный результат cpp_thread неверен: количество точек в выпуклых оболочках не совпадают" << std::endl;

	for (int i = 0; i < answer.second; i++)
	{
		if (answer.first[i] != answer_OpenMP.first[i])
		{
			std::cout << "Параллельный результат OpenMp неверен: выпуклые оболчки не совпадают" << std::endl;
			return 1;
		}
	}

	for (int i = 0; i < answer.second; i++)
	{
		if (answer.first[i] != answer_cpp_thread.first[i])
		{
			std::cout << "Параллельный результат cpp_thread неверен: выпуклые оболчки не совпадают" << std::endl;
			return 1;
		}
	}

	std::cout << "Выпуклые оболочки совпали" << std::endl;
	return 0;
}