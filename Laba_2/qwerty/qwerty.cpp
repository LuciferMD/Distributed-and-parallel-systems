#include <iostream>
#include <Windows.h>
#include <vector>
#include <ctime>
#include <thread>

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
double func(double, double);

bool rectangular_integral();
DWORD WINAPI calculate(LPVOID);


bool rectangular_integral() {
	dx = (b - a) / n; //Count the legth of each element
	dy = (d - c) / m;
	double I = 0;
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			MYDATA data;
			data.xi = a + dx / 2 + i * dx;			// Method of medium rectangles
			data.yj = c + dy / 2 + j * dy;
			cells.push_back(data);
		}
	}

	HANDLE* threads = new HANDLE[K];
	for (int k = 0; k < K; k++) {
		int p = k;
		threads[k] = CreateThread(NULL, 0, calculate, (LPVOID)p, NULL, NULL);
	}

	WaitForMultipleObjects(K, threads, TRUE, INFINITE);

	return true;
}

DWORD WINAPI calculate(LPVOID lpParam) {
	double I = 0;
	for (unsigned int i = (int)lpParam; i < cells.size(); i += K) {
		MYDATA data = cells[i];
		I += dx * dy * func(data.xi, data.yj);
	}

	results.push_back(I);
	return (DWORD)1;
}


void console_data_input() {
	cout << "Calculating the double integral of a function f(x) by area D=[a,b]x[c,d]" << endl << endl;
	cout << "Count Threads (K) : ";
	cin >> K;
	cout << "Area D" << endl;
	cout << "(a) : ";
	cin >> a;
	cout << "(b) : ";
	cin >> b;
	cout << "(c) : ";
	cin >> c;
	cout << "(d) : ";
	cin >> d;
	cout << "How many divided parts [a,b] (n) : ";
	cin >> n;
	cout << "How many divided parts [c,d] (m) : ";
	cin >> m;
	cout << "\tChoose functions : " << endl;
	cout << "1. e^(x) * sin(y)" << endl;
	cout << "2. sin(x + 2 * y)" << endl;
	cout << "3. cos(x * e^(y)" << endl;
	cout << "4. exp(cos(x^(2) + y )" << endl;
	cin >> f;
}


double func(double x, double y) {
	switch (f) {
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
	

int main() {
	
	console_data_input();

	auto begin = chrono::steady_clock::now();

	rectangular_integral();
	for (int i = 0; i < results.size(); i++)
		Result += results[i];

	auto end = chrono::steady_clock::now();
	auto elapsed_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);

	cout << "Result: " << Result << std::endl;
	cout << "Time: " << elapsed_ms.count() << " ms\n";
	system("pause");

	return 0;
}