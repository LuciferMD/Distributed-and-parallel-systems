#include <iostream>
#include <Windows.h>
#include <vector>
#include <ctime>
#include <thread>
#include <future>
#include<omp.h>

using namespace std;
int K, f, n, m;
double a, b, c, d, dx, dy;
double Result = 0;

typedef struct MyData {
	double xi;
	double yj;
} MYDATA, * PMYDATA;

vector<MYDATA> cells;
vector<double> results;

void console_data_input();
double function_one(double, double);
double function_two(double, double);
double function_three(double, double);
double function_four(double, double);
double function(double, double);
double func(double x, double y);
double mulmatr(double, double);

bool rectangular_integral();
int calculate(int);

int main()
{
	setlocale(LC_ALL, "rus");
	console_data_input();

	auto begin = chrono::steady_clock::now();

	rectangular_integral();
	for (int i = 0; i < results.size(); i++)
		Result += results[i];

	auto end = chrono::steady_clock::now();
	auto elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);

	cout << "Полученный результат: " << Result << std::endl;
	cout << "Затраты по времени: " << elapsed_ms.count() << " мс\n";
	system("pause");

	return 0;
}


bool rectangular_integral() {
	dx = (b - a) / n;
	dy = (d - c) / m;
	double I = 0;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			MYDATA data;
			data.xi = a + dx / 2 + i * dx;
			data.yj = c + dy / 2 + j * dy;
			cells.push_back(data);
		}
	}
	double result = 0;
	int* threads = new int[K];
	
	calculate(0);


	return true;
}

int calculate(int lpParam) {
	double I = 0;
	lpParam = K;
#pragma omp parallel num_threads(K) reduction(+: I) 
	for (int i = (int)lpParam; i < cells.size(); i += K) {
		MYDATA data = cells[i];
		I += dx * dy * func(data.xi, data.yj);
	}

	results.push_back(I);
	return 1;
}


void console_data_input() {
	cout << "двойной интеграл функции f(x) по области D=[a,b]x[c,d]\n\n";
	cout << "Кол-во потоков (K) : ";
	cin >> K;
	cout << "Область D\n";
	cout << "(a) : ";
	cin >> a;
	cout << "(b) : ";
	cin >> b;
	cout << "(c) : ";
	cin >> c;
	cout << "(d) : ";
	cin >> d;
	cout << "Частей разбиения [a,b] (n) : ";
	cin >> n;
	cout << "Частей разбиения [c,d] (m) : ";
	cin >> m;
	cout << "Выберите функцию : \n";
	cout << "1. e^(x) * sin(y)\n";
	cout << "2. sin(x + 2 * y)\n";
	cout << "3. cos(x * e^(y)\n";
	cout << "4. exp(cos(x^(2) + y )\n";
	cin >> f;
}

double func(double x, double y) {
	switch (f)
	{
	case 1:
		return function_one(x, y);
	case 2:
		return function_two(x, y);
	case 3:
		return function_three(x, y);
	case 4:
		return function_four(x, y);
	default:
		return 0;
	}
}


double function_one(double x, double y) {
	return exp(x) * sin(y);
};

double function_two(double x, double y) {
	return sin(x + 2 * y);
};

double function_three(double x, double y) {
	return cos(x * exp(y));
}

double function_four(double x, double y) {
	return exp(cos(pow(x, 2) + y));
}
double mulmatr(double, double) {
	return 0;
}