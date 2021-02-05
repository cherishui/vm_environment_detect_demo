// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "TencentAPI.h"
#include <tlhelp32.h>
#include <shlwapi.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" BOOL _declspec(dllexport) CheckVMWare1();
extern "C" BOOL _declspec(dllexport) CheckVMWare2();
extern "C" BOOL _declspec(dllexport) CheckVMWare3();
extern "C" BOOL _declspec(dllexport) CheckVMWare4();
extern "C" BOOL _declspec(dllexport) CheckVMWare5();
extern "C" BOOL _declspec(dllexport) CheckVMWare6();
extern "C" BOOL _declspec(dllexport) CheckVMWare7();
extern "C" BOOL _declspec(dllexport) CheckVMWare8();


//1.��ѯI/Oͨ�Ŷ˿�
BOOL CheckVMWare1()
{
	BOOL bResult = TRUE;
	__try
	{
		__asm
		{
			push   edx
			push   ecx
			push   ebx				//���滷��
			mov    eax, 'VMXh'
			mov    ebx, 0			//��ebx����
			mov    ecx, 10			//ָ�����ܺţ����ڻ�ȡVMWare�汾��Ϊ0x14ʱ��ȡVM�ڴ��С
			mov    edx, 'VX'		//�˿ں�
			in     eax, dx			//�Ӷ˿�edx ��ȡVMware��eax
			cmp    ebx, 'VMXh'		//�ж�ebx���Ƿ����VMware�汾��VMXh�������������������
			setz [bResult]			//���÷���ֵ
			pop    ebx				//�ָ�����
			pop    ecx
			pop    edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)	//���δ����VMware�У��򴥷����쳣
	{
		bResult = FALSE;
	}
	return bResult;
}

//2.ͨ��MAC��ַ���
BOOL CheckVMWare2()
{
	string strMac;
	getMacAddr(strMac);
	if (strMac == "00-05-69" || strMac == "00-0c-29" || strMac == "00-50-56")
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//3.CPUID���
BOOL CheckVMWare3()
{
	DWORD dwECX = 0;
	bool b_IsVM = true;
	_asm
	{
		pushad;
		pushfd;
		mov eax, 1;
		cpuid;
		mov dwECX, ecx;
		and ecx, 0x80000000;//ȡ���λ
		test ecx, ecx;		//���ecx�Ƿ�Ϊ0
		setz[b_IsVM];		//Ϊ�� (ZF=1) ʱ�����ֽ�
		popfd;
		popad;
	}
	if (b_IsVM)				//������
	{
		return FALSE;
	}
	else					//�����
	{
		return TRUE;
	}
}

//4.ͨ���������кš��ͺš�ϵͳ�����ڴ������Ƶ�����Ӳ����Ϣ
BOOL CheckVMWare4()
{
	string table = "Win32_DiskDrive";
	wstring wcol = L"Caption";
	string ret;
	ManageWMIInfo(ret, table, wcol);
	if (ret.find("VMware") != string::npos)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//5.�����ض����̼��
BOOL CheckVMWare5()
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//�������
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	BOOL bMore = Process32First(hProcessSnap, &pe32);					  //������һ������
	while (bMore)
	{
		if (wcscmp(pe32.szExeFile, L"vmtoolsd.exe") == 0)				  //ע��˴�Ҫ��wcscmp��pe32.szExeFile�� WCHAR*��
		{
			return TRUE;
		}

		bMore = Process32Next(hProcessSnap, &pe32);					      //������һ������
	}
	CloseHandle(hProcessSnap);
	return FALSE;
}



//6.ͨ��ע�����
BOOL CheckVMWare6()
{
	HKEY hkey;
	if (RegOpenKey(HKEY_CLASSES_ROOT, L"\\Applications\\VMwareHostOpen.exe", &hkey) == ERROR_SUCCESS) 
	{
		return TRUE;	//RegOpenKey�����򿪸�����,������ڸü�����ERROR_SUCCESS
	}
	else
	{
		return FALSE;
	}
}




//7.ͨ��������
BOOL CheckVMWare7()
{
	int menu = 0;
	SC_HANDLE SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);	//�򿪷�����ƹ�����
	if (SCMan == NULL)
	{
		cout << GetLastError() << endl;
		printf("OpenSCManager Eorror/n");
		return -1;
	}
	LPENUM_SERVICE_STATUSA service_status;
	DWORD cbBytesNeeded = NULL;
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;
	service_status = (LPENUM_SERVICE_STATUSA)LocalAlloc(LPTR, 1024 * 64);
	bool ESS = EnumServicesStatusA(SCMan,										//��������
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		(LPENUM_SERVICE_STATUSA)service_status,
		1024 * 64,
		&cbBytesNeeded,
		&ServicesReturned,
		&ResumeHandle);
	if (ESS == NULL)
	{
		printf("EnumServicesStatus Eorror/n");
		return -1;
	}
	for (int i = 0; i < ServicesReturned; i++)
	{
		if (strstr(service_status[i].lpDisplayName, "Virtual Disk") != NULL || strstr(service_status[i].lpDisplayName, "VMware Tools") != NULL)
		{
			return TRUE;
		}
	}
	CloseServiceHandle(SCMan);
	return FALSE;
}

//�ļ�·�����
BOOL CheckVMWare8()
{
	if (PathIsDirectory(L"C:\\Program Files\\VMware\\") == 0)	
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}



//ͨ��ʱ�����
BOOL CheckVMWareTmp()
{
	__asm
	{
		rdtsc			//RDTSCָ����������������CPU������������ŵ�EDX��EAX���棬����EDX�Ǹ�λ����EAX�ǵ�λ��
		xchg ebx, eax	//���Դ���ָ������ʱ��
		rdtsc
		sub eax, ebx	//ʱ���
		cmp eax, 0xFF
		jg detected
	}
	return FALSE;
detected:
	return TRUE;
}