#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <processthreadsapi.h>

#define PERIOD_S 1
#define OUTPUT_STRING "old string"
#define MAX_STRING_SIZE 256

using namespace std;

int main()
{
	cout << "pid: " << GetCurrentProcessId() << endl;
	char outputString[MAX_STRING_SIZE];
	strcpy_s(outputString, OUTPUT_STRING);
	while (true) {
		this_thread::sleep_for(chrono::seconds(PERIOD_S));
		cout << outputString << endl;
	}
}