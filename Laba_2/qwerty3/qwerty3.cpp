#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <math.h>
#include <thread>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <fstream>

int K, n;
int** matrix_1;
int** matrix_2;
int** matrix_mul_simple;
int** matrix_mul_multip;
long run_time = 0;

std::ofstream simple_mul("..//OneThread.txt");
std::ofstream multip_mul("..//ManyThreaads.txt");

HANDLE* threads;



void generate_matrix() {
	matrix_1 = new int* [n];
	matrix_2 = new int* [n];
	matrix_mul_simple = new int* [n];
	matrix_mul_multip = new int* [n];

	for (int i = 0; i < n; i++) {
		matrix_1[i] = new int[n];
		matrix_2[i] = new int[n];
		matrix_mul_simple[i] = new int[n];
		matrix_mul_multip[i] = new int[n];
	}

	srand(time(0));

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			matrix_1[i][j] = 1 + rand() % 100;
			matrix_2[i][j] = 1 + rand() % 100;
			matrix_mul_simple[i][j] = 0;
			matrix_mul_multip[i][j] = 0;
		};
	};
}

void print_matrix(int** matrix, int border, std::ostream& stream) {
	stream << "Time: " << run_time << " ms." << std::endl;
	stream.setf(std::ios::right);
	stream << std::endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			stream << std::setw(border) << matrix[i][j] << " ";
		}
		stream << std::endl;
	}
	stream << std::endl;
}


void input_data() {
	std::cout << "Calculating the product of two matrices of order n" << std::endl;
	std::cout << "Enter n: ";
	std::cin >> n;
	std::cout << "Enter К: ";
	std::cin >> K;
	if (K > 64)
		K = 64;
	threads = new HANDLE[K];
}

DWORD WINAPI calculate(LPVOID lpParam) {
	int num = (int)lpParam;
	int row;	
	int col;
	for (int i = num; i < n * n; i += K) {
		row = i / n;
		col = i % n;
		for (int inner = 0; inner < n; inner++) {
			matrix_mul_multip[row][col] += matrix_1[row][inner] * matrix_2[inner][col];
		}
	}
	return NULL;
}

void multiply_simple(int** matrix1, int** matrix2, int**& result) {
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			for (int inner = 0; inner < n; inner++) {
				result[row][col] += matrix1[row][inner] * matrix2[inner][col];
			}
		}
	}
}

void multiply_multip() {
	int counter = 0;

	for (int j = 0; j < K; j++) {
		int p = j;
		threads[j] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)calculate, (LPVOID)p, NULL, NULL);
	}

	WaitForMultipleObjects(K, threads, TRUE, INFINITE);
}

int main() {
	setlocale(LC_ALL, "Russian");
	input_data();
	generate_matrix();

	auto begin_1 = std::chrono::steady_clock::now();
	multiply_simple(matrix_1, matrix_2, matrix_mul_simple);
	auto end_1 = std::chrono::steady_clock::now();
	auto elapsed_ms_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_1 - begin_1);
	run_time = elapsed_ms_1.count();

	print_matrix(matrix_mul_simple, 6, simple_mul);

	auto begin_2 = std::chrono::steady_clock::now();
	multiply_multip();
	auto end_2 = std::chrono::steady_clock::now();
	auto elapsed_ms_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - begin_2);
	run_time = elapsed_ms_2.count();

	print_matrix(matrix_mul_multip, 6, multip_mul);

	system("pause");
	return 0;
}