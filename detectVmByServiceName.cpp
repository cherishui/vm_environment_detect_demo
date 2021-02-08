#include "detectVmByServiceName.h"
#include <windows.h>
#include <string>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;;
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
		{
			return true;
		}
			
	}

	return false;
}

bool CheckVMByServerName()
{
	BOOL bInVirtual = FALSE;
	// 打开服务控制管理器
	SC_HANDLE SCMan = OpenSCManager(NULL,                       // 打开本机服务管理控制器
		SERVICES_ACTIVE_DATABASE,
		SC_MANAGER_ENUMERATE_SERVICE   // 枚举服务
	);
	if (NULL == SCMan)
	{
		printf("CheckVirtualEnvirByServiceName OpenSCManager fail. error:%d", GetLastError());
		return FALSE;
	}

	DWORD cbBytesNeeded = 0;
	DWORD ServicesReturned = 0;
	DWORD ResumeHandle = 0;
	// 遍历服务,获得真实服务大小
	BOOL ESS = EnumServicesStatus(SCMan,
		SERVICE_WIN32,                // 枚举WIN32服务类型
		SERVICE_STATE_ALL,            // 
		NULL,                         // 枚举结果输出缓存
		0,                            // 枚举结果输出缓存大小
		&cbBytesNeeded,             // 获取剩余services所需字节个数
		&ServicesReturned,          // 返回服务个数
		&ResumeHandle);
	if (NULL == ESS && ERROR_MORE_DATA == GetLastError())
	{
		int nRealBytes = cbBytesNeeded;
		cbBytesNeeded = 0;
		LPENUM_SERVICE_STATUSA service_status = (LPENUM_SERVICE_STATUSA)new char[nRealBytes];

		ESS = EnumServicesStatus(SCMan,
			SERVICE_WIN32,                // 枚举WIN32服务类型
			SERVICE_STATE_ALL,            // 所有状态
			service_status,             // 枚举结果输出缓存
			nRealBytes,                 // 枚举结果输出缓存大小
			&cbBytesNeeded,            // 获取剩余services所需字节个数
			&ServicesReturned,         // 返回服务个数
			&ResumeHandle);
		if (ESS && 0 == cbBytesNeeded)
		{
			for (DWORD i = 0; i < ServicesReturned; i++)
			{
				if (IsServiceExist(service_status[i].lpServiceName))
				{
					bInVirtual = TRUE;
					printf("CheckVirtualEnvirByServiceName detect: %s", service_status[i].lpServiceName);
					break;
				}
			}
		}
		else
		{
			printf(("CheckVirtualEnvirByServiceName fail: error:%d cbBytesNeeded:%d ServicesReturned:%d ResumeHandle:%d", GetLastError(), cbBytesNeeded, ServicesReturned, ResumeHandle);
		}

		delete[]service_status;
		service_status = NULL;
	}
	else
	{
		printf("CheckVirtualEnvirByServiceName EnumServicesStatus fail. error:%d", GetLastError());
	}

	CloseServiceHandle(SCMan);
	return bInVirtual;
}