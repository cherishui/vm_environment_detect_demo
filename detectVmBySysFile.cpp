#include "detectVmBySysFile.h"
#include <tchar.h>
#include <io.h>

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
	};

	for (int i = 0; i < sizeof(list) / sizeof(list[0]); i++)
	{
		if (-1 != _access(list[i], 0))
		{
			return true;
		}
	}

	return false;

}

