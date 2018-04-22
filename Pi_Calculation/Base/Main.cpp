#include <chrono>
#include <omp.h>
#include <iostream>
#include <stack>
#include <math.h>
#include <thread>

double calculate_PI(const int& numPoints)
{
	double result = 0;
	double dx = 1.0 / numPoints;
	double x = 0;
	for (int i = 0; i < numPoints; i++)
	{
		result += (4.0 / (1.0 + pow(x, 2)))*dx;
		x = x + dx;
	}
	return result;
}

double calculate_PI_par_OMP(const int& numPoints)
{
	double result = 0;
	double dx = 1.0 / numPoints;
	double x = 0;
#pragma omp parallel for shared(dx) firstprivate(x) reduction (+ : result)
	for (int i = 0; i < numPoints; i++)
	{
		x = i*dx;
		result += (4.0 / (1.0 + pow(x, 2)))*dx;
	}
	return result;
}

void thread_calculation(int thread_num,const double& dx, int startPoint, int endPoint, double* resArr)
{
	resArr[thread_num] = 0.0;
	double x = 0;
	for (int i = startPoint; i < endPoint; i++)
	{
		x = i*dx;
		resArr[thread_num] += (4.0 / (1.0 + pow(x, 2)))*dx;
	}
}

double calculate_PI_cpp_thread(const int& numPoints, int numThreads)
{
	double result = 0;
	double dx = 1.0 / numPoints;
	std::pair<int, int>* ind_array = new std::pair<int, int>[numThreads];
	std::div_t portion = div(numPoints, numThreads); 
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

	double* resArr = new double[numThreads];
	std::thread* threadArr = new std::thread[numThreads - 1];
	for (int i = 1; i < numThreads; i++)
	{
		threadArr[i - 1] = std::thread(thread_calculation, i, dx, ind_array[i].first, ind_array[i].second, resArr);
	}
	thread_calculation(0, dx, ind_array[0].first, ind_array[0].second, resArr);
	result += resArr[0];
	for (int i = 1; i < numThreads; i++)
	{
		threadArr[i-1].join();
		result += resArr[i];
	}

	return result;
}


int main()
{
	setlocale(LC_ALL, "Russian");
	double pi = 3.1415926535897932384626433;
	int numPoints;
	std::cout << "Введите количество точек разбиения" << std::endl;
	std::cin >> numPoints;

	auto start = std::chrono::high_resolution_clock::now();
	double answer = calculate_PI(numPoints);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " (микросекунды)" << std::endl;
	std::cout << "Полученный ответ: " << answer << std::endl;
	std::cout << "Погрешность: " << abs(pi - answer) << std::endl;

	auto startParOMP = std::chrono::high_resolution_clock::now();
	double answerParOMP = calculate_PI_par_OMP(numPoints);
	auto endParOMP = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(endParOMP - startParOMP).count() << " (микросекунды)" << std::endl;
	std::cout << "Полученный ответ: " << answerParOMP << std::endl;
	std::cout << "Погрешность: " << abs(pi - answerParOMP) << std::endl;

	auto startParCppPThr = std::chrono::high_resolution_clock::now();
	double answerCppPThr = calculate_PI_cpp_thread(numPoints, 8);
	auto endParCppPThr = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(endParCppPThr - startParCppPThr).count() << " (микросекунды)" << std::endl;
	std::cout << "Полученный ответ: " << answerCppPThr << std::endl;
	std::cout << "Погрешность: " << abs(pi - answerCppPThr) << std::endl;

	return 0;
}