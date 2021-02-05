#include "detectVmByServiceName.h"
#include <windows.h>
#include <string>
#include <algorithm>
#include <iostream>
bool IsServiceExist(const char* pName)
{
	if (nullptr == pName)
		return false;

	// 使用服务名称来读
	const char* list[] = {
		// 
		"vmvss",
		"vmtools",

		//virtuablBox
		"vboxservice",

		//virtualPC
		"1-vmsrvc"
	};

	std::string strProcessName(pName);
	std::transform(strProcessName.begin(), strProcessName.end(), strProcessName.begin(), ::tolower);
	for (int i = 0; i < sizeof(list) / sizeof(list[0]); i++)
	{
		if (0 == strProcessName.compare(list[i]))
			return true;
	}

	return false;
}

bool CheckVMByServerName()
{
	int menu = 0;
	SC_HANDLE SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);        //打开服务控制管理器
	if (SCMan == NULL)
	{
		std::cout << GetLastError() << std::endl;
		printf("OpenSCManager Eorror/n");
		return -1;
	}

	LPENUM_SERVICE_STATUSA service_status;
	DWORD cbBytesNeeded = NULL;
	DWORD ServicesReturned = NULL;
	DWORD ResumeHandle = NULL;
	service_status = (LPENUM_SERVICE_STATUSA)LocalAlloc(LPTR, 1024 * 64);
	bool ESS = EnumServicesStatusA(SCMan, //遍历服务
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
		// 此处加入各种虚拟机特征服务
		if (IsServiceExist(service_status[i].lpServiceName))
		{
			return TRUE;
		}
	}
	CloseServiceHandle(SCMan);
	return FALSE;
}
