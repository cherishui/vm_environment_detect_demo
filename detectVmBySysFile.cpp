#include "detectVmBySysFile.h"
#include <tchar.h>
#include <io.h>
#include <iostream>
using std::cout;
using std::endl;
bool CheckVMBySysFile()
{
	const char* list[] = {
		//Vmware
		"C:\\windows\\System32\\Drivers\\Vmmouse.sys",	
		"C:\\windows\\System32\\Drivers\\vmusbmouse.sys",
		"C:\\windows\\System32\\vmhgfs.dll",

		//virtuablBox
		"C:\\windows\\System32\\Drivers\\VBoxMouse.sys",
		"C:\\windows\\System32\\Drivers\\VBoxGuest.sys",
		"C:\\windows\\System32\\Drivers\\VBoxSF.sys",

		//virtualPC
		"C:\\windows\\System32\\Drivers\\vmsrvc.sys",

		// MAC Parallels Desktop
		"C:\\windows\\System32\\Drivers\\prl_boot.sys",
		"C:\\windows\\System32\\Drivers\\prl_eth5.sys",
		"C:\\windows\\System32\\Drivers\\prl_mouf.sys",
	};

	for (int i = 0; i < sizeof(list) / sizeof(list[0]); i++)
	{
		if (-1 != _access(list[i], 0))
		{
			std::cout << "CheckVMBySysFile: detect file " << list[i] << std::endl;
			return true;
		}
	}

	return false;

}

