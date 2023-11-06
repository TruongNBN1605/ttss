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
void DummyDataInitialization(double*& pMatrix, double*& pResult, int& size) {
    srand(unsigned(clock()));
	for (int i = 0; i < size; i++) {
        pResult[i] = 0.0;
		for (int j = 0; j < size; j++) pMatrix[i * size + j] = rand() / double(1000);
	}
}

/*************************************************************************************************/
/* PROCESS METHODs */
// Function for memory allocation and definition of object's elements
void ProcessInitialization(double*& pMatrix, double*& pResult, double& pAvgEven, double& pAvgOdd, int& size) {
	pMatrix = new double[size * size];
	pResult = new double[size];
	DummyDataInitialization(pMatrix, pResult, size);
    pAvgEven = 0.0;
    pAvgOdd = 0.0;
}
// Function for serial caculation
void ProcessSerialCalculation(double* pMatrix, double* pResult, double& pAvgEven, double& pAvgOdd, int& size) {
    for (int i = 0; i < size; i++) {
        pResult[i] = pMatrix[i * size];
        for (int j = 0; j < size; j++) {
            if (i % 2) {
                if (pResult[i] > pMatrix[i * size + j]) pResult[i] = pMatrix[i * size + j];
            } else {
                if (pResult[i] < pMatrix[i * size + j]) pResult[i] = pMatrix[i * size + j];
            }
        }
    }
    int countEven = 0, countOdd = 0;
    double sumEven = 0, sumOdd = 0;
    for (int i = 0; i < size; i++) {
        if (i % 2) {
            sumEven += pResult[i]; countEven++;
        } else {
            sumOdd += pResult[i]; countOdd++;
        }
    }
    pAvgEven = sumEven / countEven;
    pAvgOdd = sumOdd / countOdd;
}
// Function for parallel caculation
void ProcessParalleCalculation(double* pMatrix, double* pResult, double& pAvgEven, double& pAvgOdd, int& size) {
    #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            pResult[i] = pMatrix[i * size];
            for (int j = 0; j < size; j++) {
                if (i % 2) {
                    if (pResult[i] > pMatrix[i * size + j]) pResult[i] = pMatrix[i * size + j];
                } else {
                    if (pResult[i] < pMatrix[i * size + j]) pResult[i] = pMatrix[i * size + j];
                }
            }
        }

    int countEven = 0, countOdd = 0;
    double sumEven = 0, sumOdd = 0;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            for (int i = 0; i < size; i += 2) {
                sumEven += pResult[i]; countEven++;
            }
            pAvgEven = sumEven / countEven;
        }
        #pragma omp section
        {
            for (int i = 1; i < size; i += 2) {
                sumOdd += pResult[i]; countOdd++;
            }
            pAvgOdd = sumOdd / countOdd;
        }
    }
}
// Function for computational process termination
void ProcessTermination(double* pMatrix, double* pResult) {
	delete[] pMatrix;
	delete[] pResult;
}

/*************************************************************************************************/
/* PROGRAM METHODs */
double getDurationProcessSerialCalculation(double* pMatrix, double* pResult, double& pAvgEven, double& pAvgOdd, int& size) {
    // Cấp phát bộ nhớ cho dữ liệu
    ProcessInitialization(pMatrix, pResult, pAvgEven, pAvgOdd, size);
    // Thực hiện tính toán song song
    double start = GetTime();
    ProcessSerialCalculation(pMatrix, pResult, pAvgEven, pAvgOdd, size);
    double finish = GetTime();
    // Xóa bộ nhớ dữ liệu
    ProcessTermination(pMatrix, pResult);
    // Trả về thời gian tính toán
    return finish - start;
}
double getDurationProcessParalleCalculation(double* pMatrix, double* pResult, double& pAvgEven, double& pAvgOdd, int& size) {
    // Cấp phát bộ nhớ cho dữ liệu
    ProcessInitialization(pMatrix, pResult, pAvgEven, pAvgOdd, size);
    // Thực hiện tính toán song song
    double start = GetTime();
    ProcessParalleCalculation(pMatrix, pResult, pAvgEven, pAvgOdd, size);
    double finish = GetTime();
    // Xóa bộ nhớ dữ liệu
    ProcessTermination(pMatrix, pResult);
    // Trả về thời gian tính toán
    return finish - start;
}

int main() {
    double* pMatrix; // Ma trận ban đầu
    double* pResult; // Mảng các số cần tìm tại mỗi hàng
    double pAvgEven; // Giá trị trung bình số nhỏ nhất hàng chẵn
    double pAvgOdd; // Giá trị trung bình số lớn nhaasrt hàng lẻ
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

        double serialTime = getDurationProcessSerialCalculation(pMatrix, pResult, pAvgEven, pAvgOdd, size);
        double paralleTime = getDurationProcessParalleCalculation(pMatrix, pResult, pAvgEven, pAvgOdd, size);
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