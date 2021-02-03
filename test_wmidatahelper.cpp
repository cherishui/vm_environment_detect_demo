#include "test_wmidatahelper.h"
#include "WmiDataHelper.h"
#include "tchar.h"

void test_wmidatahelper()
{
	int                                 iIndex = 0;
	INT64                               int64Size = 0;
	INT64                               int64SizeCur = 0;
	size_t                              nSize = 0;
	size_t                              nSizeCur = 0;
	std::wstring                        strTmpW;
	std::vector<std::wstring>           vecResult;
	std::vector<std::wstring>::iterator it;
	CWmiDataHelper                      WmiData;

	/// ��װ�����б� Win32_Product
	if (WmiData.ExecComQuery(L"select * from  Win32_Product", L"Name", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_Product Name = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
			_tprintf(L"%s\r\n", (*it).c_str());
	}

	/// Win32_NetworkAdapterConfiguration Ip��ַ IPAddress
	if (WmiData.ExecComQuery(L"select * from  Win32_NetworkAdapterConfiguration", L"IPAddress", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_NetworkAdapterConfiguration IPXAddress = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
			_tprintf(L"%s\r\n", (*it).c_str());
	}

	/// �������� Win32_NetworkAdapter Name
	if (WmiData.ExecComQuery(L"select * from  Win32_NetworkAdapter", L"Name", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_NetworkAdapter Name = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
			_tprintf(L"%s\r\n", (*it).c_str());
	}

	/// ����Mac��ַ Win32_NetworkAdapter MACAddress
	if (WmiData.ExecComQuery(L"select * from  Win32_NetworkAdapter", L"MACAddress", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_NetworkAdapter MACAddress = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
			_tprintf(L"%s\r\n", (*it).c_str());
	}

	/// ���̷��� Win32_DiskPartition
	if (WmiData.ExecComQuery(L"select * from  Win32_DiskPartition", L"DeviceID", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_DiskPartition Capacity = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
			_tprintf(L"%s\r\n", (*it).c_str());
	}

	// ��������
	if (WmiData.ExecComQuery(L"select * from  Win32_DiskPartition", L"Type", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_DiskDrive Capacity = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
			_tprintf(L"%s\r\n", (*it).c_str());
	}

	if (WmiData.ExecComQuery(L"select * from  Win32_DiskPartition", L"Size", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_DiskDrive Capacity = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			strTmpW = (*it).c_str();
			int64SizeCur = _ttoi64(strTmpW.c_str());
			int64Size = int64SizeCur / 1024 / 1024 / 1024;
			_tprintf(L"%dGB\r\n", int64Size);
		}
	}

	/// Ӳ�� Win32_DiskDrive
	if (WmiData.ExecComQuery(L"select * from  Win32_DiskDrive", L"Capabilities", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_DiskDrive Capacity = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			strTmpW = (*it).c_str();
			int64SizeCur = _ttoi64(strTmpW.c_str());
			int64Size += int64SizeCur;
		}

		_tprintf(L"%dGB\r\n", int64Size);
	}

	/// �ڴ� Win32_PhysicalMemory, ������int64!
	if (WmiData.ExecComQuery(L"select * from  Win32_PhysicalMemory", L"Capacity", vecResult))
	{
		int64Size = 0;
		_tprintf(L"Win32_PhysicalMemory Capacity = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			strTmpW = (*it).c_str();
			int64SizeCur = _ttoi64(strTmpW.c_str());
			int64Size += int64SizeCur;
		}

		_tprintf(L"%dMB\r\n", int64Size / 1024 / 1024);
	}

	/// �����ڴ� Win32_PerfFormattedData_PerfOS_Memory
	if (WmiData.ExecComQuery(L"select * from Win32_PerfFormattedData_PerfOS_Memory", L"AvailableMBytes", vecResult))
	{
		nSize = 0;
		_tprintf(L"Win32_PerfFormattedData_PerfOS_Memory Capacity = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			nSizeCur = _ttol((*it).c_str());
			nSize += nSizeCur;
		}

		_tprintf(L"%dMB\r\n", nSize);
	}

	/// �ô�������Ϣ
	/// CPU ���� Manufacturer
	if (WmiData.ExecComQuery(L"select * from win32_Processor", L"Manufacturer", vecResult))
	{
		_tprintf(L"cpu Manufacturer = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			_tprintf(L"%s\r\n", (*it).c_str());
		}
	}

	/// CPU Name
	if (WmiData.ExecComQuery(L"select * from win32_Processor", L"Name", vecResult))
	{
		_tprintf(L"Name = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			_tprintf(L"%s\r\n", (*it).c_str());
		}
	}

	/// CPUռ����
	if (WmiData.ExecComQuery(L"select * from win32_Processor", L"LoadPercentage", vecResult))
	{
		_tprintf(L"cpu LoadPercentage = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			_tprintf(L"%s\r\n", (*it).c_str());
		}
	}

	/// ��������Ϣ
	/// �������� Manufacturer
	if (WmiData.ExecComQuery(L"select * from Win32_BaseBoard", L"Manufacturer", vecResult))
	{
		_tprintf(L"MainBoard = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			_tprintf(L"%s\r\n", (*it).c_str());
		}
	}

	/// �����ͺ�
	if (WmiData.ExecComQuery(L"select * from Win32_BaseBoard", L"Product", vecResult))
	{
		_tprintf(L"MainBoard = :\r\n");
		for (it = vecResult.begin(); it != vecResult.end(); it++)
		{
			_tprintf(L"%s\r\n", (*it).c_str());
		}
	}


}
