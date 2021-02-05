#include "detectVmByReg.h"
#include <string>
#include <windows.h>
#include <winreg.h>

// 思路5：基于注册表来检测
long registerOpenKey(char *value)
{
	HKEY hk;
	return RegOpenKeyExA(HKEY_LOCAL_MACHINE, value, 0, KEY_READ, &hk);
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