#include "detectVmByProcessName.h"
#include <windows.h>
#include <algorithm>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>
using std::cout;
using std::endl;

// 思路1：检测指定进程名是否存在
bool IsExist(const wchar_t* pName)
{
	if (nullptr == pName)
		return false;

	const wchar_t* list[] = {
		//Vmware
		L"vmtoolsd.exe",
		L"vmacthlp.exe",
		L"vmwaretray.exe",
		L"vmwareuser.exe",

		//virtuablBox
		L"vboxserivce.exe",
		L"vboxtray.exe",

		//virtualPC
		L"vmsrvc.exe",
		L"vmusrvc.exe",
		L"vpcmap.exe",

		// Mac Parallels Desktop	
	};

	std::wstring strProcessName(pName);
	std::transform(strProcessName.begin(), strProcessName.end(), strProcessName.begin(), ::tolower);
	for (int i = 0; i < sizeof(list) / sizeof(list[0]); i++)
	{
		if (0 == strProcessName.compare(list[i]))
		{
			cout << "CheckVMByProcessName detect " << list[i] << endl;
			return true;
		}
			
	}

	return false;
}

bool CheckVMByProcessName()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
		return false;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		char szName[MAX_PATH] = { 0 };

		if (IsExist(pe32.szExeFile))
		{
			_tprintf(L"进程名: %s 进程PID: %d", pe32.szExeFile, pe32.th32ProcessID);
			break;
		}

		bMore = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);
	return bMore;
}



