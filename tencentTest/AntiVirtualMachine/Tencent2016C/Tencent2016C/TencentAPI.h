#pragma once
#include <string>
#include <iostream>
#include <windows.h>

using namespace std;

//��ȡMAC��ַ
extern void getMacAddr(string &strMac);

//ͨ��WMI��ȡ������Ϣ
extern BOOL ManageWMIInfo(string &result, string table, wstring wcol);


