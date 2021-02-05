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
// ������������

// ˼·3��������Ȩָ��ļ��
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
		and ecx, 0x80000000;        //ȡ���λ
		test ecx, ecx;              //���ecx�Ƿ�Ϊ0
		setz[b_IsVM];               //Ϊ�� (ZF=1) ʱ�����ֽ�
		popfd;
		popad;
	}
	if (b_IsVM)                          //������
	{
		return FALSE;
	}
	else                                 //�����
	{
		return TRUE;
	}
}


// ��Ȩָ����
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
			// 'VMXh'��ʮ�����Ʊ�ʾ ---> 0x564d5868
			mov        eax, 'VMXh'
			// ��ebx����
			xor        ebx, ebx
			// ָ�����ܺ�0x0a, ���ڻ�ȡVMWare�汾��
			mov        ecx, 0x0A
			// �˿ں�: 'VX' ---> 0x5658
			mov        edx, 'VX'
			// �Ӷ˿�dx��ȡVMWare�汾��ebx
			in        eax, dx
			// �ж�ebx���Ƿ�'VMXh',���������������
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
	// Vmware���⻷����IDT��ַλ�� 0xFFXXXXXX
	// VirtualBox��IDT��ַλ�� 
	// VirtualPC��IDT��ַλ�� 0xE8XXXXXX
	// ��ʵ������IDT��ַλ�� 0x80XXXXXX
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
		std::cout << "�������������" << std::endl;
	}
	else
	{
		std::cout << "�������������" << std::endl;
	}
	std::cout << "==================" << std::endl;
	return;
}

int main()
{
	// ��������԰汾��
	// VMware 10.0.1
	// Virtual Box 6.1.18
	// Virtual PC 6.1.7601

	testVirtualEnvironment(std::bind(CheckVMByProcessName), "�ض����������");
	testVirtualEnvironment(std::bind(CheckVMByDiskName), "Ӳ�����Ƽ��");
	testVirtualEnvironment(std::bind(CheckVMByMacAddr), "MAC��ַ���");
	testVirtualEnvironment(std::bind(CheckVMByCpuId), "CPUId���");
	testVirtualEnvironment(std::bind(CheckVMByServerName), "�ض�������");	
	testVirtualEnvironment(std::bind(CheckVMBySysFile), "�ض��ļ����");
	testVirtualEnvironment(std::bind(CheckVMByReg), "ע�����");
	testVirtualEnvironment(std::bind(CheckVMByInCmd), "IN��Ȩָ��");
	testVirtualEnvironment(std::bind(CheckVMByIDT), "IDT��ַ");
	testVirtualEnvironment(std::bind(IsInVmwareByLDT), "LDT��ַ");
	testVirtualEnvironment(std::bind(IsInVmwareByGdt), "GDT��ַ");
	testVirtualEnvironment(std::bind(IsInVmwareBySTR), "STR");


	getchar();
	return 0;
}
