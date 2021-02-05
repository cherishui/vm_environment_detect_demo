// 思路4：基于网卡MAC地址的检测
// VMware默认网卡MAC地址前缀为 00-05-69、00-0C-29 或者 00-50-56
// Virtual Box默认网卡MAC地址前缀为 
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

