// ˼·4����������MAC��ַ�ļ��
// VMwareĬ������MAC��ַǰ׺Ϊ 00-05-69��00-0C-29 ���� 00-50-56
// Virtual BoxĬ������MAC��ַǰ׺Ϊ 
#include <windows.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#include "detectVmByMacAddr.h"
#include <stdio.h>
#pragma comment(lib, "Iphlpapi.lib")
bool CheckVMByMacAddr()
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


	if (// 00-05-69    
		(0x08 == pAdapterInfo->Address[0] &&
			0x00 == pAdapterInfo->Address[1] &&
			0x27 == pAdapterInfo->Address[2])
		)
	{	// Virtual Box
		bVM = TRUE;
	}

	if (// 
		(0x00 == pAdapterInfo->Address[0] &&
			0x03 == pAdapterInfo->Address[1] &&
			0xff == pAdapterInfo->Address[2])
		)
	{	// Virtual PC
		bVM = TRUE;
	}

	return bVM;
}

