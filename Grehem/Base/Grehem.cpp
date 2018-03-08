#include <omp.h>
#include <iostream>
#include <random>
#include <chrono>
#include <math.h>

struct dot
{
	int x;
	int y;
};
const bool operator < (const dot &d1, const dot &d2)
{
	if (d2.y*d1.x - d2.x*d1.y > 0)
		return true;
	else if (d2.y*d1.x - d2.x*d1.y == 0)
		if (sqrt(pow(d1.y, 2) + pow(d1.x, 2)) < sqrt(pow(d2.y, 2) + pow(d2.x, 2)))
			return true;
	return false;
}
dot operator + (const dot &d1, const dot &d2)
{
	dot result;
	result.x = d1.x + d2.x;
	result.y = d1.y + d2.y;
	return result;
}
dot operator - (const dot &d1, const dot &d2)
{
	dot result;
	result.x = d1.x - d2.x;
	result.y = d1.y - d2.y;
	return result;
}

void genRandArray(dot* arr, int size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-1000000, 1000000);
	for (int i = 0; i < size; i++)
	{
		arr[i].x = dis(gen);
		arr[i].y = dis(gen);
	}
}

//
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

void copy(dot* copiedArr, dot* pasteArr, int size)
{
	for (auto i = 0; i < size; i++)
		pasteArr[i] = copiedArr[i];
}

void quickSortPar(dot* arr, int startInd, int endInd)
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
#pragma omp parallel sections num_threads(2)
		{
#pragma omp section
			quickSort(arr, startInd, middleInd);
#pragma omp section
			quickSort(arr, middleInd + 1, endInd);
		}
	}
}
//
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
		quickSort(arr, startInd, middleInd);
		quickSort(arr, middleInd + 1, endInd);
	}
}

void quickSortSearchMinPar(dot* arr, int startInd, int endInd)
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
#pragma omp parallel sections num_threads(2)
		{
#pragma omp section
			quickSort(arr, startInd, middleInd);
#pragma omp section
			quickSort(arr, middleInd + 1, endInd);
		}
	}
}

class stack
{
	int size;
	int maxSize;
	dot* Array;
public:
	stack(int inSize)
	{
		size = 0;
		maxSize = inSize;
		Array = new dot[maxSize];
	}

	~stack()
	{
		delete[] Array;
	}
	
	void push(dot element)
	{
		if (size + 1 == maxSize)
		{
			dot* tmpArr = Array;
			Array = new dot[maxSize * 2];
			copy(tmpArr, Array, maxSize);
			maxSize = maxSize * 2;
			delete[] tmpArr;
		}
		Array[size] = element;
		size++;
	}

	dot get()
	{
		return Array[size - 1];
	}

	dot pop()
	{
		size--;
		return Array[size];
	}

	int getSize()
	{
		return size;
	}

	dot* getArray()
	{
		dot* retArray = new dot[size];
		copy(Array, retArray, size);
		return retArray;
	}
};

void searchMinElement(dot* dotArray, int size)
{
	quickSortSearchMin(dotArray, 0, size);
}

void searchMinElementPar(dot* dotArray, int size)
{
	quickSortSearchMinPar(dotArray, 0, size);
}


 std::pair<dot*, int> grehemMethod(dot* dotArray, int size)
{
	searchMinElement(dotArray, size - 1);
	dot move = dotArray[0];
	for (auto i = 0; i < size - 1; i++)
		dotArray[i] = dotArray[i] - move;
	quickSort(dotArray, 1, size - 1);
	dotArray[size - 1] = dotArray[0];
	stack dotStack(size / 2);
	dot dotY, dotX;
	dotStack.push(dotArray[0]);
	dotStack.push(dotArray[1]);
	for (auto i = 2; i < size; i++)
	{
		while (true)
		{
			if (dotStack.getSize() > 1)
			{
				dotY = dotStack.pop();
				dotX = dotStack.get();
				if ((dotArray[i].x*(dotX.y - dotY.y) + dotArray[i].y*(dotY.x - dotX.x) + dotX.x*dotY.y - dotX.y*dotY.x) > 0)
				{
					dotStack.push(dotY);
					dotStack.push(dotArray[i]);
					break;
				}
				else
				{
					if (dotStack.getSize() == 1)
					{
						dotStack.push(dotArray[i]);
						break;
					}
				}
			}
		}
	}
	dot* resultArray = dotStack.getArray();
	for (auto i = 0; i < dotStack.getSize(); i++)
	{
		resultArray[i] = resultArray[i] + move;
	}
	return std::make_pair(resultArray, dotStack.getSize());
}

 std::pair<dot*, int> grehemMethodPar(dot* dotArray, int size)
 {
	 searchMinElementPar(dotArray, size - 1);
	 dot move = dotArray[0];
#pragma omp parallel for
		 for (auto i = 0; i < size - 1; i++)
			 dotArray[i] = dotArray[i] - move;
	 quickSortPar(dotArray, 1, size - 1);
	 dotArray[size - 1] = dotArray[0];
	 stack dotStack(size / 2);
	 dot dotY, dotX;
	 dotStack.push(dotArray[0]);
	 dotStack.push(dotArray[1]);
	 for (auto i = 2; i < size; i++)
	 {
		 while (true)
		 {
			 if (dotStack.getSize() > 1)
			 {
				 dotY = dotStack.pop();
				 dotX = dotStack.get();
				 if ((dotArray[i].x*(dotX.y - dotY.y) + dotArray[i].y*(dotY.x - dotX.x) + dotX.x*dotY.y - dotX.y*dotY.x) > 0)
				 {
					 dotStack.push(dotY);
					 dotStack.push(dotArray[i]);
					 break;
				 }
				 else
				 {
					 if (dotStack.getSize() == 1)
					 {
						 dotStack.push(dotArray[i]);
						 break;
					 }
				 }
			 }
		 }
	 }
	 dot* resultArray = dotStack.getArray();
#pragma omp parallel for
		 for (auto i = 0; i < dotStack.getSize(); i++)
			 resultArray[i] = resultArray[i] + move;
	 return std::make_pair(resultArray, dotStack.getSize());
 }

int main()
{
	setlocale(LC_ALL, "Russian");
	int size = 100000000;
	dot* arr = new dot[size + 1];
	genRandArray(arr, size);
	dot* parallelArr = new dot[size + 1];
	copy(arr, parallelArr, size);
	auto start = std::chrono::high_resolution_clock::now();
	std::pair<dot*,int> answer = grehemMethod(arr, size + 1);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " (микросекунды)" << std::endl;
	
	auto startPar = std::chrono::high_resolution_clock::now();
	answer = grehemMethodPar(parallelArr, size + 1);
	auto endPar = std::chrono::high_resolution_clock::now();

	std::cout << " Время выполнения " << std::chrono::duration_cast<std::chrono::microseconds>(endPar - startPar).count() << " (микросекунды)" << std::endl;
	
	return 0;
}