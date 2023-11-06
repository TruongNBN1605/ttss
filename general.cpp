#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <omp.h>

/*************************************************************************************************/
/* SUPPORT METHODs */
// Function that converts numbers form LongInt type to double type
double LiToDouble(LARGE_INTEGER x) {
	double result = ((double)x.HighPart) * 4.294967296E9 + (double)((x).LowPart);
	return result;
}
// Function that gets the timestamp in seconds
double GetTime() {
	LARGE_INTEGER lpFrequency, lpPerfomanceCount;
	QueryPerformanceFrequency(&lpFrequency);
	QueryPerformanceCounter(&lpPerfomanceCount);
	return LiToDouble(lpPerfomanceCount) / LiToDouble(lpFrequency);
}
// Function for simple definition of matrix and vector elements
void DummyDataInitialization(double* pMatrix, double* pVector, int Size) {
    srand(unsigned(clock()));
	int i, j;
	for (i = 0; i < Size; i++) {
		pVector[i] = rand() / double(1000);
		for (j = 0; j < Size; j++)
			pMatrix[i * Size + j] = rand() / double(1000);
	}
}

/*************************************************************************************************/
/* PROCESS METHODs */
// Function for memory allocation and definition of object's elements
void ProcessInitialization(double*& pMatrix, double*& pVector, double*& pResult, int& Size) {
	pMatrix = new double[Size * Size];
	pVector = new double[Size];
	pResult = new double[Size];
	DummyDataInitialization(pMatrix, pVector, Size);
}
// Function for serial matrix-vector multiplication
void ProcessSerialCalculation(double* pMatrix, double* pVector, double* pResult, int Size) {
	int i, j;
	for (i = 0; i < Size; i++) {
		for (j = 0; j < Size; j++)
			pResult[i] += pMatrix[i * Size + j] * pVector[j];
	}
}
// Function for parallel matrix-vector multiplication
void ProcessParalleCalculation(double* pMatrix, double* pVector, double* pResult, int Size) {
	int i, j;
	#pragma omp paralell for private (j) 
	for (i = 0; i < Size; i++) {
		for (j = 0; j < Size; j++)
			pResult[i] += pMatrix[i * Size + j] * pVector[j];
	}
}
// Function for computational process termination
void ProcessTermination(double* pMatrix, double* pVector, double* pResult) {
	delete[] pMatrix;
	delete[] pVector;
	delete[] pResult;
}

/*************************************************************************************************/
/* PROGRAM METHODs */
double getDurationProcessSerialCalculation(double* pMatrix, double* pVector, double* pResult, int Size) {
    // Cấp phát bộ nhớ cho dữ liệu
    ProcessInitialization(pMatrix, pVector, pResult, Size);
    // Thực hiện tính toán song song
    double start = GetTime();
    ProcessSerialCalculation(pMatrix, pVector, pResult, Size);
    double finish = GetTime();
    // Xóa bộ nhớ dữ liệu
    ProcessTermination(pMatrix, pVector, pResult);
    // Trả về thời gian tính toán
    return finish - start;
}
double getDurationProcessParalleCalculation(double* pMatrix, double* pVector, double* pResult, int Size) {
    // Cấp phát bộ nhớ cho dữ liệu
    ProcessInitialization(pMatrix, pVector, pResult, Size);
    double start = GetTime();
    ProcessParalleCalculation(pMatrix, pVector, pResult, Size);
    double finish = GetTime();
    // Xóa bộ nhớ dữ liệu
    ProcessTermination(pMatrix, pVector, pResult);
    // Trả về thời gian tính toán
    return finish - start;
}

int main() {
    double* pMatrix; // Ma trận ban đầu
    double* pVector; // Vector ban đầu
    double* pResult; // Kết quả của phép nhân ma trận-vector
    int numThreads = 8;
    omp_set_num_threads(numThreads);
    int sizeData[12] = {10, 100, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};
    int sizeLength = sizeof(sizeData) / sizeof(sizeData[0]);

    std::cout << std::fixed << std::left << std::setprecision(15);
    std::cout << std::setw(20)
        << "Kích thước ma trận\t"
        << std::setw(30)
        << "Thời gian thực hiện tuần tự\t"
        << std::setw(30)
        << "Thời gian thực hiện song song\t"
        << std::setw(30)
        << "Hiệu suất\t"
        << std::endl;
    for (int i = 0; i < sizeLength; i++) {
        int size = sizeData[i];

        double serialTime = getDurationProcessSerialCalculation(pMatrix, pVector, pResult, size);
        double paralleTime = getDurationProcessParalleCalculation(pMatrix, pVector, pResult, size);
        double performance = serialTime / (paralleTime * numThreads);

        std::cout << std::setw(20)
            << sizeData[i] << "\t"
            << std::setw(30)
            << serialTime << "\t"
            << std::setw(30)
            << paralleTime << "\t"
            << std::setw(30)
            << performance << "\t"
            << std::endl;
    }

    return 0;
}
