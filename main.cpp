#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <tchar.h>
#include <IPHlpApi.h>
// ������������

// ˼·1�����ָ���������Ƿ����
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
			_tprintf(L"������: %s ����PID: %d", pe32.szExeFile, pe32.th32ProcessID);
			break;
		}
			
		bMore = Process32Next(hProcessSnap, &pe32);		
	}

	CloseHandle(hProcessSnap);
	return bMore;
}

// ˼·2��ͨ�����Ӳ������
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

// ˼·3��������Ȩָ��ļ��

// ˼·4����������MAC��ַ�ļ��
// VMwareĬ������MAC��ַǰ׺Ϊ 00-05-69��00-0C-29 ���� 00-50-56
// Virtual BoxĬ������MAC��ַǰ׺Ϊ 
#pragma comment(lib, "Iphlpapi.lib")
BOOL CheckVMByMacAddr()
{
	BOOL bVM = FALSE;
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);
	// ��ȡ������Ϣ
	DWORD dwStatus = ::GetAdaptersInfo(
		AdapterInfo,
		&dwBufLen);
	if (ERROR_SUCCESS != dwStatus)
	{
		printf("GetAdaptersInfo");
		return FALSE;
	}
	// �����������б���, ֵ��ȡ��һ������(Ĭ������)��Ϣ 
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
		// VMware ���� 
		bVM = TRUE;
	}
	return bVM;
}

// ˼·5������ע��������
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

// ��Ȩָ����
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
			// ��ebx����
			xor        ebx, ebx
			// ָ�����ܺ�0x0a, ���ڻ�ȡVMWare�汾
			mov        ecx, 0x0A
			// �˿ں�: 'VX' ---> 0x5658
			mov        edx, 'VX'
			// �Ӷ˿�dx��ȡVMWare�汾��ebx
			in        eax, dx
			// �ж�ebx���Ƿ�'VMXh',���������������
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
		std::cout << "�������������" << std::endl;
	}
	else
	{
		std::cout << "û���������������" << std::endl;
	}

	std::cout << "CheckVMByReg=======" << std::endl;
	if (CheckVMByReg())
	{
		std::cout << "�������������" << std::endl;
	}
	else
	{
		std::cout << "û���������������" << std::endl;
	}
	
	std::cout << "CheckVMByMacAddr=======" << std::endl;
	if (CheckVMByMacAddr())
	{
		std::cout << "�������������" << std::endl;
	}
	else
	{
		std::cout << "û���������������" << std::endl;
	}

	std::cout << "CheckVMByDiskName=======" << std::endl;
	if (CheckVMByDiskName())
	{
		std::cout << "�������������" << std::endl;
	}
	else
	{
		std::cout << "û���������������" << std::endl;
	}

	std::cout << "CheckVMByProcessName=======" << std::endl;
	if (CheckVMByProcessName())
	{
		std::cout << "�������������" << std::endl;
	}
	else
	{
		std::cout << "û���������������" << std::endl;
	}

	getchar();
	return 0;
}
