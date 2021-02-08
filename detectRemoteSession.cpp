#include "detectRemoteSession.h"
#include <windows.h>
#include <TlHelp32.h>
#include <IPHlpApi.h>
#include <tchar.h>

#include <iostream>
using std::cout;
using std::endl;

BOOL IsCurrentSessionRemoteable()
{
	BOOL fIsRemoteable = FALSE;

	if (GetSystemMetrics(SM_REMOTESESSION))
	{
		fIsRemoteable = TRUE;
	}
	else
	{
#define TERMINAL_SERVER_KEY _T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\")
#define GLASS_SESSION_ID    _T("GlassSessionId")
		HKEY hRegKey = NULL;
		LONG lResult;

		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			TERMINAL_SERVER_KEY,
			0, // ulOptions
			KEY_READ,
			&hRegKey
		);

		if (lResult == ERROR_SUCCESS)
		{
			DWORD dwGlassSessionId;
			DWORD cbGlassSessionId = sizeof(dwGlassSessionId);
			DWORD dwType;

			lResult = RegQueryValueEx(
				hRegKey,
				GLASS_SESSION_ID,
				NULL, // lpReserved
				&dwType,
				(BYTE*)&dwGlassSessionId,
				&cbGlassSessionId
			);

			if (lResult == ERROR_SUCCESS)
			{
				DWORD dwCurrentSessionId;

				if (ProcessIdToSessionId(GetCurrentProcessId(), &dwCurrentSessionId))
				{
					fIsRemoteable = (dwCurrentSessionId != dwGlassSessionId);
				}
			}
		}

		if (hRegKey)
		{
			RegCloseKey(hRegKey);
		}
	}

	return fIsRemoteable;
}

BOOL IsCurrentSessionRemoteableByREMOTESESSION()
{
	return GetSystemMetrics(SM_REMOTESESSION);
}

BOOL IsCurrentSessionRemoteableByReg()
{
	BOOL fIsRemoteable = FALSE;

	{
#define TERMINAL_SERVER_KEY _T("SYSTEM\\CurrentControlSet\\Control\\Terminal Server\\")
#define GLASS_SESSION_ID    _T("GlassSessionId")
		HKEY hRegKey = NULL;
		LONG lResult;

		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			TERMINAL_SERVER_KEY,
			0, // ulOptions
			KEY_READ,
			&hRegKey
		);

		if (lResult == ERROR_SUCCESS)
		{
			std::cout << "open TERMINAL_SERVER_KEY success" << std::endl;

			DWORD dwGlassSessionId;
			DWORD cbGlassSessionId = sizeof(dwGlassSessionId);
			DWORD dwType;

			lResult = RegQueryValueEx(
				hRegKey,
				GLASS_SESSION_ID,
				NULL, // lpReserved
				&dwType,
				(BYTE*)&dwGlassSessionId,
				&cbGlassSessionId
			);

			if (lResult == ERROR_SUCCESS)
			{
				DWORD dwCurrentSessionId;

				if (ProcessIdToSessionId(GetCurrentProcessId(), &dwCurrentSessionId))
				{
					std::cout << "dwCurrentSessionId:" << dwCurrentSessionId << " dwGlassSessionId:" << dwGlassSessionId << std::endl;
					fIsRemoteable = (dwCurrentSessionId != dwGlassSessionId);
				}
			}
			else
			{
				std::cout << "RegQueryValueEx fail" << std::endl;
			}
		}
		else
		{
			std::cout << "open TERMINAL_SERVER_KEY fail" << std::endl;
		}

		if (hRegKey)
		{
			RegCloseKey(hRegKey);
		}
	}

	return fIsRemoteable;
}

BOOL ConbineTwo()
{
	if (IsCurrentSessionRemoteableByREMOTESESSION())
	{
		return TRUE;
	}

	return IsCurrentSessionRemoteableByReg();
}


void detectRemoteSession()
{
	std::cout << "IsCurrentSessionRemoteableByREMOTESESSION=======" << std::endl;
	if (IsCurrentSessionRemoteableByREMOTESESSION())
	{
		std::cout << "运行在远程连接中" << std::endl;
	}
	else
	{
		std::cout << "运行在本地连接中" << std::endl;
	}

	std::cout << "IsCurrentSessionRemoteableByReg=======" << std::endl;
	if (IsCurrentSessionRemoteableByReg())
	{
		std::cout << "运行在远程连接中" << std::endl;
	}
	else
	{
		std::cout << "运行在本地连接中" << std::endl;
	}

	std::cout << "ConbineTwo=======" << std::endl;
	if (ConbineTwo())
	{
		std::cout << "运行在远程连接中" << std::endl;
	}
	else
	{
		std::cout << "运行在本地连接中" << std::endl;
	}

}