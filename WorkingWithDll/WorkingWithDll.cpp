#include <Windows.h>
#include <iostream>
#include <string>

#define OUTPUT_STRING "old string"
#define REPLACE_STRING "NEW STRING!!!"
#define MAX_STRING_SIZE 256

#define REMOTE_PROCESS_PATH L"D:\\5 семестр\\ОСиСП\\l3\\WorkingWithDll\\Debug\\Process.exe"
#define LIB_PATH "D:\\5 семестр\\ОСиСП\\l3\\WorkingWithDll\\Debug\\LibDLL.dll"

using namespace std;

extern "C" void __cdecl replaceString(string, string);
typedef void(_cdecl* importedFuncPointer)(string, string);
PROCESS_INFORMATION pi;

void processCleanup(PROCESS_INFORMATION pi) {
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

bool consoleCloseHandler(DWORD dwCtrlType) {
	TerminateProcess(pi.hProcess, 0);
	WaitForSingleObject(pi.hProcess, INFINITE);
	return true;
}

void staticFunc() {
	char outputString[MAX_STRING_SIZE];
	strcpy_s(outputString, OUTPUT_STRING);
	cout << outputString << endl;
	replaceString(OUTPUT_STRING, REPLACE_STRING);
	cout << outputString << endl;
}

void dynamicFunc() {
	HINSTANCE stringReplaceDll;
	importedFuncPointer replaceStringPointer;
	stringReplaceDll = LoadLibrary(L"LibDLL.dll");
	replaceStringPointer = (importedFuncPointer)GetProcAddress(stringReplaceDll, "replaceString");
	char outputString[MAX_STRING_SIZE];
	strcpy_s(outputString, OUTPUT_STRING);
	cout << outputString << endl;
	(replaceStringPointer)(OUTPUT_STRING, REPLACE_STRING);
	FreeLibrary(stringReplaceDll);
	cout << outputString << endl;
}

void remoteFunc() {
	TCHAR ProcessName[MAX_STRING_SIZE];
	wcscpy_s(ProcessName, REMOTE_PROCESS_PATH);
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	PROCESS_INFORMATION pinfo;
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleCloseHandler, true);
	CreateProcess(ProcessName, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	int pid;
	cout << "enter pid: ";
	cin >> pid;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	char libPath[MAX_STRING_SIZE];
	strcpy_s(libPath, LIB_PATH);
	void* pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(libPath), MEM_COMMIT, PAGE_READWRITE);
	bool i = WriteProcessMemory(hProcess, pLibRemote, libPath, sizeof(libPath), NULL);
	void* startAddr = GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryA");
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)startAddr, pLibRemote, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	DWORD loadedLib;
	GetExitCodeThread(hThread, &loadedLib);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, pLibRemote, sizeof(libPath), MEM_RELEASE);
	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"), "FreeLibrary"),(void*)loadedLib,0, NULL);

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	WaitForSingleObject(pi.hProcess, INFINITE);
	processCleanup(pi);
}



int main()
{
	while (true) {
		cout << "Print:\n1 - static DLL\n2 - dynamic DLL\n3 - remote thread" << endl;
		char n;
		cin >> n;
		switch (n) {
		case '1':
			staticFunc();
			return 0;
		case '2':
			dynamicFunc();
			return 0;
		case '3':
			remoteFunc();
			return 0;
		default:
			continue;
		}
	}
}
