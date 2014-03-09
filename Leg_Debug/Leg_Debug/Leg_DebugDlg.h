
// Leg_DebugDlg.h : ���Y��
//

#pragma once
#include "afxwin.h"
#include "DebugData.h"
#include "afxcmn.h"
#include "Decode2Asm.h"

// CLeg_DebugDlg ��ܤ��
class CLeg_DebugDlg : public CDialogEx
{
// �غc
public:
	CLeg_DebugDlg(CWnd* pParent = NULL);	// �зǫغc�禡
	
// ��ܤ�����
	enum { IDD = IDD_LEG_DEBUG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩


// �{���X��@
protected:
	HICON m_hIcon;

	// ���ͪ��T�������禡
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpen();
	//��l�ƽվA�u�{
	char m_szFilePath[MAX_PATH];
	//�P�_�O�_��f9�Ҧ�
	BOOL m_IsGo;
	//�O�_�O�۰�F7�Ҧ� �۰ʨB�J
	BOOL m_IsAutoF7;
	//debugger�O�_�B��debug���p
	BOOL m_isDebuging;
	//�Q�ոնi�{�H�����c��
	TARGET_PROCESS_INFO m_tpInfo;
	//�i�����󪺸��|
	//char m_SzFilePath[MAX_PATH];
	//�Ч� �P�_�O�_�n���s�T�|�Ҷ� ��DLL�[����,�m��TRUE ���ɻݭn��s�Ҷ��H����
	BOOL m_GetModule;
	//�ɥX��ƬM�g��  �Ψ�Ʀa�}������ �o�ˬd�ߧ�
	CMap<LONGLONG, LONGLONG&, EXPORT_FUN_INFO, EXPORT_FUN_INFO&> m_ExFunList;
	//��ƦW�P�a�}���� �Τ_�b��ƤJ�f�UAPI�_�I
	CMap<CString, LPCTSTR, LONGLONG, LONGLONG&> m_Fun_Address;
	//�άM�g��O���w�g�B��F�����O�a�} �o�˥i�H�L�o���_���O(�p�`����)
	CMap<LONGLONG, LONGLONG&, OPCODE_RECORD, OPCODE_RECORD&> m_Opcode;
	//�O�_OEP
	bool m_IsOepBP;
	//��DLL�ɥX��ƶi��϶׽s
	void DisassemblerExcFun(char* szFunName);
	//�B�zU�R�O �p�G�S���a�}�N�q�H�e���a�}����U �b��B�Ϊ��_�I���`���A��o�Ӧa�}�]����eEIP����
	void ON_U_COMMAND(LONGLONG dwAddress);
	//��o�n�Q�R�����w���_�I���ոձH�s���s�� ��^-1��ܨS���
	int GetDeletedDrIndex(LONGLONG dwAddress, CONTEXT ct);
	//�h���R�O������M�k�䪺�Ů�r��
	BOOL Kill_SPACE(char* szCommand);
	//�R���w���_�I
	void DeleteHardBP(LONGLONG dwAddress);
	//�B�zB�R�O
	void Handle_B_Command(char* szCommand);
	//�Τ�]�m�_�I
	void UserSetBP(HANDLE hProcess, LONGLONG dwBpAddress, BYTE bCCode);
	//�Τ�R�O���B�z���
	void Handle_User_Command(char* szCommand);
	//�P�_�O�_�O��B�B�L�Ҧ� �p�G�O�h�n�b�_�I���`����X�H�s���M���O
	BOOL m_IsF8;
	//�O�_�O�۰�F8�Ҧ� �۰ʳ�B�B�L
	BOOL m_IsAutoF8;
	//�R���ä[�_�I
	BOOL m_isDelete;
	//INT3�_�I���
	CList<INT3_BP, INT3_BP&> m_Int3BpList;
	//�ݭn�Q���s��_��INT3�_�I�����c��
	RECOVER_BP m_Recover_BP;
	//�R���Τ��_�I
	void DeleteUserBP(HANDLE hProcess, LONGLONG dwBpAddress);
	//�Ҷ��H���� �Τ_��ܼ��D�� �Y��e���O�b���ӼҶ�
	CList<MODULE_INFO, MODULE_INFO&> m_Module;
    //�O��U�R�O����e�a�}
	LONGLONG m_Uaddress;
public:
	//�o��i�����󪺸��|
	void GetExeFilePath(char* szFilePath);
	//�M�g��� �}�ˬdPE���ĩʥH�άO���OEXE���
	BOOL MapPEFile();
	//��X���~�H�� 
	void GetErrorMessage(DWORD dwErrorCode);
	//�B�z�[��DLL�ƥ�
	void ON_LOAD_DLL_DEBUG_EVENT(HANDLE hFile, LPVOID pBase);
	//�o��[��DLL�ɪ����|
	void GetFileNameFromHandle(HANDLE hFile, LPVOID pBase);
	//�B�zeb���
	void CLeg_DebugDlg::ChangeByte(HANDLE hProcess, LONGLONG dwAddress, byte chby);
	// ��o�ɤJ���Ʀa�}
	BOOL GetExportFunAddress(HANDLE hFile, char* pDll, LPVOID pBase);
	//�ѼƤ@ �ɤJ��RVA �Ѽ�2�϶����ƥ� �Ѽ�3�϶������a�}
	DWORD RvaToFileOffset(DWORD dwRva, DWORD dwSecNum, PIMAGE_SECTION_HEADER pSec);
	//���}����l��
	bool OnInitial(char* lpszFilename);
	//�B�z CREATE_PROCESS_DEBUG_EVENT �ƥ󪺨�� 
	DWORD ON_CREATE_PROCESS_DEBUG_EVENT(DWORD dwProcessId, DWORD dwThreadId, LPTHREAD_START_ROUTINE lpOepAddress);
	//��XASM
	void ShowAsm(LONGLONG dwAddress);
	//Ū���O����
	BOOL OnReadMemory(IN LONGLONG dwAdderss, OUT BYTE* lpBuffer, DWORD dwSize);
	//�ѪRASM
	void DisassembleCode(char* StartCodeSection, char* EndCodeSection, LONGLONG virtual_Address);
	//��ܼȦs��
	void ShowRegData();

	//���o�u�{
	BOOL OnGetThreadContext(CONTEXT *pctThreadContext);
	//��sDLL�C��
	void GetDllInfoFromHandle(HANDLE hFile, LPVOID pBase);
	//��ܰO���餺�e
	void ShowMemoryData(LONGLONG dwAddress);
	BOOL OnIsAddressIsValid(LONGLONG dwAddress);
	//F9
	void ON_VK_F9();
	//�B�z�_�I
	LONGLONG ON_EXCEPTION_BREAKPOINT(LONGLONG dwExpAddress);
	//�^�_�q�I
	void RecoverBP(HANDLE hProcess, LONGLONG dwBpAddress, BYTE bOrignalCode);
	//
	void ReduceEIP();
	//�P�_�O�_�O�Τ�]�m��INT3�_�I �q�L�d��INT3��� 
	BOOL isUserBP(LONGLONG dwBpAddress);
	//�T�|�_�I
	void ListBP();
	//F8�䪺�B�z��� ��B�B�L
	void ON_VK_F8();
	//�R�����s�_�I
	void DeleteMemBP(LONGLONG dwBpAddress);
	//F7�䪺�B�z��� ��B�B�J
	void ON_VK_F7();
	//���ŦX�����s�_�I�H���}��^ �Ѽ��������ޥ� �}�q����R��������
	BOOL FindMemBPInformation(MEM_BP& mBP, LONGLONG dwBpAddress);
	//���P�_���S���t�@�Ӥ��s�_�I�b�o�Ӥ����W,�p�G�s�b�N�קאּ�t�@���_�I�ҭn�D���ݩ�
	//�Ѽ� ���s�������a�}
	BOOL ModifyPageProtect(LONGLONG dwBaseAddress);
	//�]�m�_�I  �_�I�a�} 0xCC �Τ_�ä[�_�I���s��_���_�I
	void DebugSetBp(HANDLE hProcess, LONGLONG dwBpAddress, BYTE bCCode);
	//��B���`�B�z���  �ѼƲ��`�a�}
	LONGLONG ON_EXCEPTION_SINGLE_STEP(LONGLONG dwExpAddress);
	//�b�϶׽s���f��ܶ׽s�N�X  �Ѽ� �n���G�����O�a�}
	void ShowAsmInWindow(LONGLONG dwStartAddress);
	//����l���s����������ݩʦ}�ǥX �ޥ�����
	BOOL FindMemOriginalProtect(MEMORY_BASIC_INFORMATION& mbi);
	//���X���  �ȾA�Τ_MOV EBP ,ESP���O���Z POP EBP���e �Q�ΰ�̭�zŪ����^�a�}
	void StepOutFromFun();
	//�]�m�ով����D(�b�ոդ��\�{��,�H�η�e���O�b���ӼҶ�)
	//�ѼƬ���e���O�a�}
	void SetDebuggerTitle(LONGLONG dwAddress);
	//�R���Ҧ��_�I �Τ_�O�����O
	void DeleteAllBreakPoint();
	//�۰ʨB�J
	void OnAutostepinto();
	//F9����
	void OnRun();
	//����F6
	void OnAutostepout();
	//�P�_�a�}�O�_���_�U���s�_�I
	BOOL FindMemBP(LONGLONG dwBpAddress);
	//�]�m�w���_�I �Ѽ� �a�} �ݩ� ����
	//dwAttribute 0��ܰ����_�I 3��ܳX���_�I 1 ��ܼg�J�_�I
	//dwLength ���� 1 2 4
	void SetHardBP(LONGLONG dwBpAddress, LONGLONG dwAttribute, LONGLONG dwLength);
	//��o��e�[���Ҷ��H��
	//�I�����
	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL GetCurrentModuleList(HANDLE hProcess);
	//�ھڭn��ܪ����O�a�} �P�_��e���O�a�}�Ʋդ��O�_�� �Y���N��^��U�� �_�h��^-1
	LONGLONG IsFindAsmAddress(LONGLONG dwStartAddress);
	//�P�_�a�}�M���ץe�F�X�Ӥ����}�[�J���s������ �}���_�I�]�[�J�_�I���
	BOOL AddMemBpPage(LONGLONG dwBpAddress, LONGLONG dwLength, MEMORY_BASIC_INFORMATION mbi, LONGLONG dwAttribute, MEM_BP& mbp);
	//�P�_�ѪR���O������ƽե�
	BOOL IsExportFun(char* szBuffer, EXPORT_FUN_INFO& expFun);
	//�P�_�Y�@�����a�}�O�_�s�b�_�����
	BOOL FindMemPage(LONGLONG dwBaseAddress);
	//�]�m���s�_�I  dwAttribute 1��ܼg�J�_�I 3��ܳX���_�I
	void SetMemBP(LONGLONG dwBpAddress, LONGLONG dwAttribute, LONGLONG dwLength);
	//�ϵw���_�I�ȮɵL��
	void InvalidHardBP(LONGLONG dwBpAddress);
	//�P�_�a�}�O�_����
	BOOL IsAddressValid(LONGLONG dwAddress, MEMORY_BASIC_INFORMATION& mbi);
	//��ܰ��
	void ShowStack();
	//��^��e�i�Ϊ��ոձH�s��
	int FindFreeDebugRegister();
	//G�R�O�B�z
	void ON_G_COMMAND(LONGLONG dwAddress);
	//��_�w���_�I �ѼƬ� �ոձH�s�����s��
	void RecoverHBP(DWORD dwIndex);
	//�P�_��B���`�O�_�O�w���_�I�ް_�� �ǥX�Ѽ� �_�I�a�}
	BOOL IfStepHard(LONGLONG& dwBPAddress);
	//��O���g�J���  �Ѽ� ���O�a�} ���O�w�R ����ܾ����X �F,
	//��ܤF�b�奻��󤤤��n��� 
	void WriteOpcodeToFile(LONGLONG dwAddress, char* szAsm);
	//�϶׽s���f��ܪ����O�a�}
	LONGLONG m_AsmAddress[22];
	//�Ыت��Τ_���O�O�������y�`
	HANDLE m_hFile;
	//�n��_���w���_�I
	//�n��_�����s��
	RECOVER_MEMPAGE m_Recover_Mpage;
	BOOL m_isMoreMem;
	//�ոձH�s��Dr0-Dr3���ϥα��p
	DR_USE m_Dr_Use;
	RECOVER_HARDBP m_Recover_HBP;
	//���s�_�I���
	CList<MEM_BP, MEM_BP&> m_MemBpList;
	//�O�s�n�����ݩʪ��Ʋ� �� 1 3�Ӥ�����,��L�����N
	DWORD m_Attribute[4];
	//���s�����
	CList<MEM_BP_PAGE, MEM_BP_PAGE&> m_MemPageList;
	//
	CListBox m_Result;
	CListCtrl m_AsmList;
	afx_msg void OnBnClickedButton1();
	CListCtrl m_ctl_RegList;
	CListCtrl m_ctl_DllList;
	afx_msg void OnBnClickedButton2();
//	LONGLONG m_dwMemoryAddress;
	CListCtrl m_ctl_DataList;
	afx_msg void OnBnClickedButton3();
	CListCtrl m_Stack;
	afx_msg void OnBnClickedButton4();
	CEdit m_command;
	afx_msg void OnEnChangeEdit2();
	CEdit m_asm_adr;
	CEdit m_dwMemoryAddress;
};
