// lab2.cpp: определяет точку входа для консольного приложения.
//
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <math.h>
#include <thread>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <fstream>
#include<future>
#include<omp.h>

int K, n;
int** matrix_1;
int** matrix_2;
int** matrix_mul_simple;
int** matrix_mul_multip;
long run_time = 0;

typedef struct MyData {
	int i;
	int j;
} MYDATA, * PMYDATA;

void input_data();
void generate_matrix();
void print_matrix(int**, int, std::ostream&);
void multiply_simple(int**, int**, int**&);
void multiply_multip();
int calculate(int);

std::ofstream single_mul("..//single_mul.txt");
std::ofstream multi_mul("..//multi_mul.txt");

HANDLE* threads;

int main()
{
	setlocale(LC_ALL, "Russian");
	input_data();
	generate_matrix();

	std::cout << "ожидайте результат..." << std::endl;

	auto begin_1 = std::chrono::steady_clock::now();
	multiply_simple(matrix_1, matrix_2, matrix_mul_simple);
	auto end_1 = std::chrono::steady_clock::now();
	auto elapsed_ms_1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_1 - begin_1);
	run_time = elapsed_ms_1.count();

	print_matrix(matrix_mul_simple, 6, single_mul);

	auto begin_2 = std::chrono::steady_clock::now();
	multiply_multip();
	auto end_2 = std::chrono::steady_clock::now();
	auto elapsed_ms_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - begin_2);
	run_time = elapsed_ms_2.count();

	print_matrix(matrix_mul_multip, 6, multi_mul);

	printf("Завершено.\n");
	system("pause");
	return 0;
}

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
	stream << "Время: " << run_time << " ms." << std::endl;
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
	std::cout << "Приложение для вычисления произведения двух матриц порядка n\n\n";
	std::cout << "Введите n: ";
	std::cin >> n;
	std::cout << "Введите К: ";
	std::cin >> K;
	if (K > 64)
		K = 64;
	threads = new HANDLE[K];
}

void multiply_multip() {
	int counter = 0;

	calculate(0);


}

int calculate(int lpParam) {
	
	int row;
	int col;
	int inner;
	#pragma omp parallel for private(row,inner,col) shared(matrix_mul_multip, matrix_1,matrix_2) num_threads(K)
	for (int i = 0; i < n * n; i++) {
		row = i / n;
		col = i % n;
		for (inner = 0; inner < n; inner++) {
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

