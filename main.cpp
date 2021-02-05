#include <iostream>
#include <algorithm>
#include <functional>

#include <windows.h>
#include "detectVmByServiceName.h"
#include "detectVmByProcessName.h"
#include "detectVmByReg.h"
#include "detectVmByDiskName.h"
#include "detectVmByMacAddr.h"
#include "detectVmBySysFile.h"
// 虚拟机检测运行

// 思路3：基于特权指令的检测
bool CheckVMByCpuId()
{
	DWORD dwECX = 0;
	bool b_IsVM = true;
	_asm
	{
		pushad;
		pushfd;
		mov eax, 1;
		cpuid;
		mov dwECX, ecx;
		and ecx, 0x80000000;        //取最高位
		test ecx, ecx;              //检测ecx是否为0
		setz[b_IsVM];               //为零 (ZF=1) 时设置字节
		popfd;
		popad;
	}
	if (b_IsVM)                          //宿主机
	{
		return FALSE;
	}
	else                                 //虚拟机
	{
		return TRUE;
	}
}


// 特权指令检测
bool CheckVMByInCmd()
{
	BOOL bVM = FALSE;
	__try
	{
		__asm
		{
			push    edx
			push    ecx
			push    ebx
			// 'VMXh'的十六进制表示 ---> 0x564d5868
			mov        eax, 'VMXh'
			// 将ebx清零
			xor        ebx, ebx
			// 指定功能号0x0a, 用于获取VMWare版本。
			mov        ecx, 0x0A
			// 端口号: 'VX' ---> 0x5658
			mov        edx, 'VX'
			// 从端口dx读取VMWare版本到ebx
			in        eax, dx
			// 判断ebx中是否'VMXh',若是则在虚拟机中
			cmp        ebx, 'VMXh'
			je			_vm
			jmp			_exit
			_vm :
				mov     eax, TRUE
				mov     bVM, eax
			_exit :
				pop        ebx
				pop        ecx
				pop        edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bVM = FALSE;
	}
	return bVM;
}

bool CheckVMByIDT()
{
	// Vmware虚拟环境的IDT地址位于 0xFFXXXXXX
	// VirtualBox的IDT地址位于 
	// VirtualPC的IDT地址位于 0xE8XXXXXX
	// 真实机器的IDT地址位于 0x80XXXXXX
typedef struct _IDTR
{
	unsigned  Limit : 16;
	unsigned  BaseLo : 16;
	unsigned  BaseHi : 16;
} IDTR;

bool        bRet = false;
IDTR        idt_reg = { 0 };
__asm
{
	SIDT        idt_reg
}

if (idt_reg.BaseHi > 0xd000)
{
	bRet = true;
}

return bRet;
}

bool IsInVmwareByLDT()
{
	unsigned short ldt_addr = 0;
	unsigned char ldtr[2] = { 0 };

	_asm {
		sldt ldtr
	}
	ldt_addr = *((unsigned short *)&ldtr);
	//printf("LDT BaseAddr: 0x%x\n", ldt_addr);

	if (ldt_addr == 0x0000)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IsInVmwareByGdt()
{
	unsigned int gdt_addr = 0;
	unsigned char gdtr[6] = { 0 };

	_asm sgdt gdtr
	gdt_addr = *((unsigned int *)&gdtr[2]);
	//printf("GDT BaseAddr:0x%x\n", gdt_addr);

	if ((gdt_addr >> 24) == 0xff)
	{
		//printf("Inside VMware\n");
		return true;
	}
	else
	{
		//printf("Native OS\n");
		return false;
	}
}

bool IsInVmwareBySTR()
{
	unsigned char mem[4] = { 0 };

	__asm {
		str mem;
	}
	//printf(" STR base: 0x");
	//for (int i = 0; i < 4; i++)
	//{
	//	printf("%02x", mem[i]);
	//}

	if ((mem[0] == 0x00) && (mem[1] == 0x40))
		return true;
	else
		return false;
}


void testVirtualEnvironment(std::function<bool()> fun, const char* pTip)
{
	std::cout << "==================" << std::endl;
	std::cout << pTip << std::endl;
	if (fun())
	{
		std::cout << "运行在虚拟机中" << std::endl;
	}
	else
	{
		std::cout << "运行在物理机中" << std::endl;
	}
	std::cout << "==================" << std::endl;
	return;
}

int main()
{
	// 虚拟机测试版本：
	// VMware 10.0.1
	// Virtual Box 6.1.18
	// Virtual PC 6.1.7601

	testVirtualEnvironment(std::bind(CheckVMByProcessName), "特定进程名检测");
	testVirtualEnvironment(std::bind(CheckVMByDiskName), "硬盘名称检测");
	testVirtualEnvironment(std::bind(CheckVMByMacAddr), "MAC地址检测");
	testVirtualEnvironment(std::bind(CheckVMByCpuId), "CPUId检测");
	testVirtualEnvironment(std::bind(CheckVMByServerName), "特定服务检测");	
	testVirtualEnvironment(std::bind(CheckVMBySysFile), "特定文件检测");
	testVirtualEnvironment(std::bind(CheckVMByReg), "注册表检测");
	testVirtualEnvironment(std::bind(CheckVMByInCmd), "IN特权指令");
	testVirtualEnvironment(std::bind(CheckVMByIDT), "IDT基址");
	testVirtualEnvironment(std::bind(IsInVmwareByLDT), "LDT基址");
	testVirtualEnvironment(std::bind(IsInVmwareByGdt), "GDT基址");
	testVirtualEnvironment(std::bind(IsInVmwareBySTR), "STR");


	getchar();
	return 0;
}
