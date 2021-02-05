#include "detectVmByDiskName.h"
#include "test_wmidatahelper.h"
#include "WmiDataHelper.h"
#include <algorithm>
#include <tchar.h>

// 思路2：通过检测硬盘标题名
bool CheckVMByDiskName()
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

