#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <tchar.h>
#include <IPHlpApi.h>
// 虚拟机检测运行

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
	};

	std::wstring strProcessName(pName);
	std::transform(strProcessName.begin(), strProcessName.end(), strProcessName.begin(), ::tolower);
	for (int i = 0; i < sizeof(list)/sizeof(list[0]); i++)
	{
		if (0 == strProcessName.compare(list[i]))
			return true;
	}

	return false;
}

BOOL CheckVMByProcessName()
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

// 思路2：通过检测硬盘名称
#include "test_wmidatahelper.h"
#include "WmiDataHelper.h"
BOOL CheckVMByDiskName()
{
	std::vector<std::wstring> vtBlockHardDisc({ L"vmware", L"vbox", L"virtual"});

	std::vector<std::wstring> vtDrivers;
	CWmiDataHelper querier;
	if (querier.ExecComQuery(L"select * from Win32_DiskDrive", L"Caption", vtDrivers) && vtDrivers.size())
	{
		std::transform(vtDrivers[0].begin(), vtDrivers[0].end(), vtDrivers[0].begin(), tolower);
		
		for (unsigned int i = 0; i < vtBlockHardDisc.size(); i++)
		{
			if (wcsstr(vtDrivers[0].c_str(), vtBlockHardDisc[i].c_str()))
			{				
				_tprintf(L"local disk name: %s. find key word:%s \n", vtDrivers[0].c_str(), vtBlockHardDisc[i].c_str());
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 思路3：基于特权指令的检测

// 思路4：基于网卡MAC地址的检测
// VMware默认网卡MAC地址前缀为 00-05-69、00-0C-29 或者 00-50-56
// Virtual Box默认网卡MAC地址前缀为 
#pragma comment(lib, "Iphlpapi.lib")
BOOL CheckVMByMacAddr()
{
	BOOL bVM = FALSE;
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	// 获取网卡信息
	DWORD dwStatus = ::GetAdaptersInfo(
		AdapterInfo,
		&dwBufLen);
	if (ERROR_SUCCESS != dwStatus)
	{
		printf("GetAdaptersInfo");
		return FALSE;
	}
	// 不对网卡进行遍历, 值获取第一个网卡(默认网卡)信息 
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	if (
		// 00-05-69    
		(0x00 == pAdapterInfo->Address[0] &&
			0x05 == pAdapterInfo->Address[1] &&
			0x69 == pAdapterInfo->Address[2]) ||
		// 00-0C-29
			(0x00 == pAdapterInfo->Address[0] &&
				0x0c == pAdapterInfo->Address[1] &&
				0x29 == pAdapterInfo->Address[2]) ||
		// 00-50-56
				(0x00 == pAdapterInfo->Address[0] &&
					0x50 == pAdapterInfo->Address[1] &&
					0x56 == pAdapterInfo->Address[2])
		)
	{
		// VMware 网卡 
		bVM = TRUE;
	}
	return bVM;
}

// 思路5：基于注册表来检测
LONG registerOpenKey(char *value)
{
	HKEY HK = 0;
	return RegOpenKeyExA(HKEY_LOCAL_MACHINE, value, 0, KEY_READ, &HK);
}

bool CheckVMByReg()
{
	std::string stringVmRegKeys[] =
	{
		//VMWare
		"SOFTWARE\\Clients\\StartMenuInternet\\VMWAREHOSTOPEN.EXE",
		"SOFTWARE\\VMware, Inc.\\VMware Tools",
		"SYSTEM\\CurrentControlSet\\Enum\\SCSI\\Disk&Ven_VMware_&Prod_VMware_Virtual_S",
		
		// Virtual PC or VirtualBox
		"SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers",
	};
	
	bool bCheckflag = false;
	for (size_t i = 0; i < sizeof(stringVmRegKeys) / sizeof(stringVmRegKeys[0]); i++)
	{
		if (ERROR_SUCCESS == registerOpenKey((char *)stringVmRegKeys[i].c_str()))
		{
			bCheckflag = true;
			printf("Analysis environment detected (%s)\n", stringVmRegKeys[i].c_str());
		}
	}

	return bCheckflag;
}

// 特权指令检测
bool CheckVMByInCmd()
{
	BOOL bVM = FALSE;
	__try
	{
		__asm
		{
			push    edx
			push    ecx
			push    ebx
			// 'VMXh' ---> 0x564d5868
			mov        eax, 'VMXh'
			// 将ebx清零
			xor        ebx, ebx
			// 指定功能号0x0a, 用于获取VMWare版本
			mov        ecx, 0x0A
			// 端口号: 'VX' ---> 0x5658
			mov        edx, 'VX'
			// 从端口dx读取VMWare版本到ebx
			in        eax, dx
			// 判断ebx中是否'VMXh',若是则在虚拟机中
			cmp        ebx, 'VMXh'
			je      _vm
			jmp     _exit
			_vm :
			mov     eax, TRUE
				mov     bVM, eax
				_exit :
			pop        ebx
				pop        ecx
				pop        edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//printf("EXCEPTION_EXECUTE_HANDLER because of IN.\n");
		bVM = FALSE;
	}
	return bVM;
}


int main()
{
	std::cout << "CheckVMByInCmd=======" << std::endl;
	if (CheckVMByInCmd())
	{
		std::cout << "运行在虚拟机中" << std::endl;
	}
	else
	{
		std::cout << "没有运行在虚拟机中" << std::endl;
	}

	std::cout << "CheckVMByReg=======" << std::endl;
	if (CheckVMByReg())
	{
		std::cout << "运行在虚拟机中" << std::endl;
	}
	else
	{
		std::cout << "没有运行在虚拟机中" << std::endl;
	}
	
	std::cout << "CheckVMByMacAddr=======" << std::endl;
	if (CheckVMByMacAddr())
	{
		std::cout << "运行在虚拟机中" << std::endl;
	}
	else
	{
		std::cout << "没有运行在虚拟机中" << std::endl;
	}

	std::cout << "CheckVMByDiskName=======" << std::endl;
	if (CheckVMByDiskName())
	{
		std::cout << "运行在虚拟机中" << std::endl;
	}
	else
	{
		std::cout << "没有运行在虚拟机中" << std::endl;
	}

	std::cout << "CheckVMByProcessName=======" << std::endl;
	if (CheckVMByProcessName())
	{
		std::cout << "运行在虚拟机中" << std::endl;
	}
	else
	{
		std::cout << "没有运行在虚拟机中" << std::endl;
	}

	getchar();
	return 0;
}
