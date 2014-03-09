// DebugData.h: interface for the CDebugData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGDATA_H__80BE350B_6F26_41DA_B22D_CA024071BE5A__INCLUDED_)
#define AFX_DEBUGDATA_H__80BE350B_6F26_41DA_B22D_CA024071BE5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//�λP�s�x�P�Q�ոնi�{�������H��
typedef struct  TargetProcess_info
{
	//�O�s�Q�ոնi�{���y�`
	HANDLE hProcess;
	//�O�s�Q�ոսu�{���y�`
	HANDLE hThread;
	//�O�s�Q�ոնi�{��ID
	DWORD dwProcessId;
	//�O�s�Q�ոսu�{��ID
	DWORD dwThreadId;
	//�J�f�I�a�}
	LPTHREAD_START_ROUTINE OepAddress;
	//�O�s�J�f�I���a�}�ƾ�
	BYTE OriginalCode;
	//�Τ_�]�mINT3�_�I��CC
	BYTE bCC;

}TARGET_PROCESS_INFO;

//INT3�_�I���c��
typedef struct INT3BREAKPOINT
{
	//�_�I�a�}
	LONGLONG dwAddress;
	//�_�I���r�`�ƾ�
	BYTE  bOriginalCode;
	//�O�_�O�ä[�_�I �ä[�_�I�ݭn��_ �@�����_�I�p�Ggo address ���ɬ��@�����_�I 
	//OEP�B���_�I�]�O�@�����_�I,���ݭn�b��_���_�I
	BOOL  isForever;

}INT3_BP;

//�O�s�ݭn�Q��_��INT3�_�I���a�}
typedef struct RECOVER_BREAKPOINT
{
	//�ݭn���s��_���_�I���a�}(�ä[�_�I)
	LONGLONG dwAddress;
	// �O�_�ݭn�Q��_���_�I
	BOOL  isNeedRecover;
	//��r�` //�Τ_��_�_�I
	BYTE  bOrginalCode;
}RECOVER_BP;

//dr7�ոձ���H�s��
typedef union _Tag_DR7
{
	struct __DRFlag
	{
		unsigned int L0 : 1;
		unsigned int G0 : 1;
		unsigned int L1 : 1;
		unsigned int G1 : 1;
		unsigned int L2 : 1;
		unsigned int G2 : 1;
		unsigned int L3 : 1;
		unsigned int G3 : 1;
		unsigned int Le : 1;
		unsigned int Ge : 1;
		unsigned int b : 3;
		unsigned int gd : 1;
		unsigned int a : 2;
		unsigned int rw0 : 2;
		unsigned int len0 : 2;
		unsigned int rw1 : 2;
		unsigned int len1 : 2;
		unsigned int rw2 : 2;
		unsigned int len2 : 2;
		unsigned int rw3 : 2;
		unsigned int len3 : 2;
	} DRFlag;
	DWORD dwDr7;
}DR7;

//DR0-DR3���ϥα��p
typedef struct _DR_USE
{
	BOOL Dr0;
	BOOL Dr1;
	BOOL Dr2;
	BOOL Dr3;

} DR_USE;

//�n��_���w���_�I���c��
typedef struct RECOVER_HARD_BREAKPOINT
{
	//�n��_���ոձH�s���s�� 0-3 //�p��-1��ܨS���n��_�� 
	//�Q�ӷQ�h�N�@�Ӧ���,�w
	DWORD dwIndex;

}RECOVER_HARDBP;

//���s�_�I���c��

typedef struct MEMORYBREAKPOINT
{
	//�a�}
	LONGLONG dwBpAddress;
	//����
	DWORD dwLength;
	//���� �O�X���_�I�٬O�g�J�_�I 
	DWORD dwAttribute;
	//���s���O�s�������a�}�Ʋ� �@���_�I��X�Ӥ��s��,�̤j5�Ӥ���!�b�h�N���ݤF
	DWORD dwMemPage[5];
	//�O���e��������
	DWORD dwNumPage;



}MEM_BP;

//���s�������c��(�ȭ����_�I������)

typedef struct MEMORYPAGE
{
	//���s�������a�}
	LONGLONG dwBaseAddress;
	//��X���ݩ�
	DWORD dwProtect;


}MEM_BP_PAGE;

//�n��_�����s���ݩ�

typedef struct _RECOVER_MEMPAGE
{
	//���s���a�}
	LONGLONG dwBaseAddress;
	//���s���_�I���O�@�ݩ�(���O��O�@�ݩ�)
	DWORD dwProtect;
	//�O�_�ݭn��_
	BOOL  isNeedRecover;
}RECOVER_MEMPAGE;


//�ɥX��Ʀa�}��
typedef struct _EXPORT_FUN_INFO
{
	//��Ʀa�}
	LONGLONG dwAddress;
	//DLL�W��
	char  szDLLName[40];

	char  szFunName[280];


}EXPORT_FUN_INFO;


//���O�O�����c��
typedef struct _OPCODE_RECORD
{
	//���O�a�}
	LONGLONG dwAddress;

}OPCODE_RECORD;

//�Ҷ��H�� �Τ_��ܼ��D�Y��e��_���ӼҶ�
typedef struct _MODULE_INFO
{
	//�Ҷ��W
	char szModuleName[200];
	//�Ҷ���}
	LONGLONG dwBaseAddress;
	//�Ҷ��j�p
	LONGLONG dwSize;

}MODULE_INFO;













#endif // !defined(AFX_DEBUGDATA_H__80BE350B_6F26_41DA_B22D_CA024071BE5A__INCLUDED_)
