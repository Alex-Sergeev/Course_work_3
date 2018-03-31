#include <chrono>
#include <omp.h>
#include <iostream>
#include <stack>
#include <math.h>

double calculate_PI(int& numPoints)
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

double calculate_PI_par_OMP(int& numPoints)
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


int main()
{
	setlocale(LC_ALL, "Russian");
	double pi = 3.1415926535897932384626433;
	int numPoints;
	std::cout << "������� ���������� ����� ���������" << std::endl;
	std::cin >> numPoints;

	auto start = std::chrono::high_resolution_clock::now();
	double answer = calculate_PI(numPoints);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << " ����� ���������� " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " (������������)" << std::endl;
	std::cout << "���������� �����: " << answer << std::endl;
	std::cout << "�����������: " << abs(pi - answer) << std::endl;

	auto startParOMP = std::chrono::high_resolution_clock::now();
	double answerParOMP = calculate_PI_par_OMP(numPoints);
	auto endParMP = std::chrono::high_resolution_clock::now();
	std::cout << " ����� ���������� " << std::chrono::duration_cast<std::chrono::microseconds>(endParMP - startParOMP).count() << " (������������)" << std::endl;
	std::cout << "���������� �����: " << answerParOMP << std::endl;
	std::cout << "�����������: " << abs(pi - answerParOMP) << std::endl;

	return 0;
}