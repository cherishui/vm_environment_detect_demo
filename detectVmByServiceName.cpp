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

	// ʹ�÷�����������
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
	// �򿪷�����ƹ�����
	SC_HANDLE SCMan = OpenSCManager(NULL,                       // �򿪱���������������
		SERVICES_ACTIVE_DATABASE,
		SC_MANAGER_ENUMERATE_SERVICE   // ö�ٷ���
	);
	if (NULL == SCMan)
	{
		printf("CheckVirtualEnvirByServiceName OpenSCManager fail. error:%d", GetLastError());
		return FALSE;
	}

	DWORD cbBytesNeeded = 0;
	DWORD ServicesReturned = 0;
	DWORD ResumeHandle = 0;
	// ��������,�����ʵ�����С
	BOOL ESS = EnumServicesStatus(SCMan,
		SERVICE_WIN32,                // ö��WIN32��������
		SERVICE_STATE_ALL,            // 
		NULL,                         // ö�ٽ���������
		0,                            // ö�ٽ����������С
		&cbBytesNeeded,             // ��ȡʣ��services�����ֽڸ���
		&ServicesReturned,          // ���ط������
		&ResumeHandle);
	if (NULL == ESS && ERROR_MORE_DATA == GetLastError())
	{
		int nRealBytes = cbBytesNeeded;
		cbBytesNeeded = 0;
		LPENUM_SERVICE_STATUSA service_status = (LPENUM_SERVICE_STATUSA)new char[nRealBytes];

		ESS = EnumServicesStatus(SCMan,
			SERVICE_WIN32,                // ö��WIN32��������
			SERVICE_STATE_ALL,            // ����״̬
			service_status,             // ö�ٽ���������
			nRealBytes,                 // ö�ٽ����������С
			&cbBytesNeeded,            // ��ȡʣ��services�����ֽڸ���
			&ServicesReturned,         // ���ط������
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