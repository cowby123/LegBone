
// Leg_DebugDlg.cpp : ��@��
//

#include "stdafx.h"
#include "Leg_Debug.h"
#include "Leg_DebugDlg.h"
#include "afxdialogex.h"
#include "DebugData.h"
//#include "Decode2Asm.cpp"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

using namespace std;
#pragma warning(disable: 4800) 
#pragma warning(disable: 4806) 
#pragma warning(disable: 4996) 
#define BEA_ENGINE_STATIC  // �����ϥ��R�ALib�w
#define BEA_USE_STDCALL    // �����ϥ�stdcall�եά��w
#define MoveMemory RtlMoveMemory
#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory

#ifdef __cplusplus


extern "C"{
#endif


#include "beaengine-win64/headers/BeaEngine.h"
#pragma comment(lib, "beaengine-win64\\Win64\\Lib\\BeaEngine64.lib")


#ifdef __cplusplus
};
#endif
DISASM MyDisasm;
int len, i = 0;
int aError = 0;

//void DisassembleCode(char* StartCodeSection,char* EndCodeSection,int (*virtual_Address)(int argc, _TCHAR* argv[]));

PVOID pBuffer = NULL;


//�O�s�M�g����}
extern char* pFile = NULL;
#define BUFSIZE 512
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
//�ոսu�{���
DWORD WINAPI DebugThreadProc(LPVOID lpParameter);


extern HANDLE g_hProcess = NULL;

extern HANDLE g_hThread = NULL;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ܤ�����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpen();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_COMMAND(ID_32771, &CAboutDlg::OnOpen)
END_MESSAGE_MAP()


// CLeg_DebugDlg ��ܤ��

CLeg_DebugDlg::CLeg_DebugDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CLeg_DebugDlg::IDD, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



void CLeg_DebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Result);
	DDX_Control(pDX, IDC_LIST2, m_AsmList);
	DDX_Control(pDX, IDC_LIST_REG, m_ctl_RegList);
	DDX_Control(pDX, IDC_LIST_DLL, m_ctl_DllList);
	//  DDX_Text(pDX, IDC_EDIT1, m_dwMemoryAddress);
	DDX_Control(pDX, IDC_LIST_DATA, m_ctl_DataList);
	DDX_Control(pDX, IDC_LIST_ST, m_Stack);
	DDX_Control(pDX, IDC_EDIT2, m_command);
	DDX_Control(pDX, IDC_EDIT3, m_asm_adr);
	DDX_Control(pDX, IDC_EDIT1, m_dwMemoryAddress);
}

BEGIN_MESSAGE_MAP(CLeg_DebugDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_32771, &CLeg_DebugDlg::OnOpen)
	ON_BN_CLICKED(IDC_BUTTON1, &CLeg_DebugDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLeg_DebugDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLeg_DebugDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CLeg_DebugDlg::OnBnClickedButton4)
	ON_EN_CHANGE(IDC_EDIT2, &CLeg_DebugDlg::OnEnChangeEdit2)
END_MESSAGE_MAP()

enum{
	REGLIST_RAX = 0,
	REGLIST_RBX,
	REGLIST_RCX,
	REGLIST_RDX,

	REGLIST_RSP,
	REGLIST_RBP,
	REGLIST_RSI,
	REGLIST_RDI,
	REGLIST_RIP,

	REGLIST_R8,
	REGLIST_R9,
	REGLIST_R10,
	REGLIST_R11,
	REGLIST_R12,
	REGLIST_R13,
	REGLIST_R14,
	REGLIST_R15,

	REGLIST_CS,
	REGLIST_SS,
	REGLIST_DS,
	REGLIST_ES,
	REGLIST_FS,
	REGLIST_GS,

	REGLIST_CF,
	REGLIST_PF,
	REGLIST_AF,
	REGLIST_ZF,
	REGLIST_SF,
	REGLIST_TF,
	REGLIST_IF,
	REGLIST_DF,
	REGLIST_OF,

};
// CLeg_DebugDlg �T���B�z�`��

BOOL CLeg_DebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �N [����...] �\���[�J�t�Υ\���C

	// IDM_ABOUTBOX �����b�t�ΩR�O�d�򤧤��C
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO:  �b���[�J�B�~����l�]�w
	
	memset(m_szFilePath, 0, sizeof(m_szFilePath));
	memset(&m_tpInfo, 0, sizeof(m_tpInfo));
	memset(&m_tpInfo, 0, sizeof(m_tpInfo));
	memset(&m_Recover_BP, 0, sizeof(m_Recover_BP));
	memset(&m_Dr_Use, 0, sizeof(m_Dr_Use));
	m_isDebuging = FALSE;
	m_GetModule = FALSE;
	m_IsGo = FALSE;
	m_IsOepBP = TRUE;
	m_IsF8 = FALSE;
	m_IsAutoF8 = FALSE;
	m_isDelete = FALSE;
	m_IsAutoF7 = FALSE;
	m_tpInfo.bCC = 0xcc;
	m_Uaddress = 0;
	m_Recover_HBP.dwIndex = -1;
	memset(&m_Recover_Mpage, 0, sizeof(m_Recover_Mpage));
	m_isMoreMem = FALSE;
	m_Attribute[0] = 0;//���e��� ��ڦ��Ϊ��O 1 3
	m_Attribute[1] = PAGE_EXECUTE_READ;
	m_Attribute[2] = 0;
	m_Attribute[3] = PAGE_NOACCESS;
	//=====================================
	m_ctl_DataList.InsertColumn(0, "�a�}", LVCFMT_LEFT, 110);
	m_ctl_DataList.InsertColumn(1, "HEX�ƭ�", LVCFMT_LEFT, 140);
	m_ctl_DataList.InsertColumn(2, "ASCII", LVCFMT_LEFT, 90);
	m_ctl_DataList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//=====================================
	m_ctl_DllList.InsertColumn(0, "DLL������|", LVCFMT_LEFT, 700);
	m_ctl_DllList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//=====================================
	m_ctl_RegList.InsertColumn(0, "�Ȧs��", LVCFMT_LEFT, 80);
	m_ctl_RegList.InsertColumn(1, "�ƭ�", LVCFMT_LEFT, 130);
	m_ctl_RegList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//=====================================
	m_Stack.InsertColumn(0, "���|�a�}", LVCFMT_LEFT, 80);
	m_Stack.InsertColumn(1, "�ƭ�", LVCFMT_LEFT, 130);
	m_Stack.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_ctl_RegList.InsertItem(REGLIST_RAX, "RAX");
	m_ctl_RegList.InsertItem(REGLIST_RBX, "RBX");
	m_ctl_RegList.InsertItem(REGLIST_RCX, "RCX");
	m_ctl_RegList.InsertItem(REGLIST_RDX, "RDX");

	m_ctl_RegList.InsertItem(REGLIST_RSP, "RSP");
	m_ctl_RegList.InsertItem(REGLIST_RBP, "RBP");
	m_ctl_RegList.InsertItem(REGLIST_RSI, "RSI");
	m_ctl_RegList.InsertItem(REGLIST_RDI, "RDI");

	m_ctl_RegList.InsertItem(REGLIST_RIP, "RIP");

	m_ctl_RegList.InsertItem(REGLIST_R8, "R8");
	m_ctl_RegList.InsertItem(REGLIST_R9, "R9");
	m_ctl_RegList.InsertItem(REGLIST_R10, "R10");
	m_ctl_RegList.InsertItem(REGLIST_R11, "R11");
	m_ctl_RegList.InsertItem(REGLIST_R12, "R12");
	m_ctl_RegList.InsertItem(REGLIST_R13, "R13");
	m_ctl_RegList.InsertItem(REGLIST_R14, "R14");
	m_ctl_RegList.InsertItem(REGLIST_R15, "R15");

	m_ctl_RegList.InsertItem(REGLIST_CS, "CS");
	m_ctl_RegList.InsertItem(REGLIST_SS, "SS");
	m_ctl_RegList.InsertItem(REGLIST_DS, "DS");
	m_ctl_RegList.InsertItem(REGLIST_ES, "ES");
	m_ctl_RegList.InsertItem(REGLIST_FS, "FS");
	m_ctl_RegList.InsertItem(REGLIST_GS, "GS");

	m_ctl_RegList.InsertItem(REGLIST_CF, "CF");
	m_ctl_RegList.InsertItem(REGLIST_PF, "PF");
	m_ctl_RegList.InsertItem(REGLIST_AF, "AF");
	m_ctl_RegList.InsertItem(REGLIST_ZF, "ZF");
	m_ctl_RegList.InsertItem(REGLIST_SF, "SF");
	m_ctl_RegList.InsertItem(REGLIST_TF, "TF");
	m_ctl_RegList.InsertItem(REGLIST_IF, "IF");
	m_ctl_RegList.InsertItem(REGLIST_DF, "DF");
	m_ctl_RegList.InsertItem(REGLIST_OF, "OF");
	//=====================================
	m_AsmList.InsertColumn(0, "�a�}", LVCFMT_LEFT, 90);
	m_AsmList.InsertColumn(1, "HEX�ƭ�", LVCFMT_LEFT, 140);
	m_AsmList.InsertColumn(2, "�ϽsĶ", LVCFMT_LEFT, 400);
	m_AsmList.SetExtendedStyle(m_AsmList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

void CLeg_DebugDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CLeg_DebugDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CLeg_DebugDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CAboutDlg::OnOpen()
{
	
	// TODO:  �b���[�J�z���R�O�B�z�`���{���X
}


//�o��i�����󪺸��|
void CLeg_DebugDlg::GetExeFilePath(char* szFilePath)
{
	OPENFILENAME file = { 0 };
	file.lpstrFile = szFilePath;
	file.lStructSize = sizeof(OPENFILENAME);
	file.nMaxFile = 256;
	file.lpstrFilter = "Executables\0*.exe\0All Files\0*.*\0\0";
	file.nFilterIndex = 1;

	if (!::GetOpenFileName(&file))
	{
		//�I�F�������s�N�h�X���
		return;
	}
}
//��X���~�H�� 
void CLeg_DebugDlg::GetErrorMessage(DWORD dwErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
		);

	::MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK | MB_ICONINFORMATION);
	//�����Ŷ�
	LocalFree(lpMsgBuf);
}
//�M�g��� �}�ˬdPE���ĩʥH�άO���OEXE���
BOOL CLeg_DebugDlg::MapPEFile()
{
	HANDLE hFile = NULL;
	//���}�����o���y�`
	hFile = CreateFile(m_szFilePath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString("EasyDbgDlg.cpp 3424��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//��X���~�H��
		GetErrorMessage(dwErrorCode);

		return FALSE;
	}
	HANDLE hFileMap = NULL;
	//�Ыؤ��M�g
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hFileMap == NULL)
	{
		OutputDebugString("EasyDbgDlg.cpp 3437��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		GetErrorMessage(dwErrorCode);
		CloseHandle(hFile);
		return FALSE;
	}
	//�M�g���
	pFile = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pFile == NULL)
	{
		OutputDebugString("EasyDbgDlg.cpp 3448��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		GetErrorMessage(dwErrorCode);

		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}

	//�P�_PE���ĩ�
	PIMAGE_DOS_HEADER pDos = NULL;
	pDos = (PIMAGE_DOS_HEADER)pFile;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFile + pDos->e_lfanew);

	//�ˬdMZ PE ��ӼЧ�
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE || pNt->Signature != IMAGE_NT_SIGNATURE)
	{
		AfxMessageBox("���O���Ī�PE���");
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}
	if (pNt->FileHeader.Characteristics&IMAGE_FILE_DLL)
	{
		AfxMessageBox("�Ӥ��ODLL,EXE���");
		CloseHandle(hFile);
		CloseHandle(hFileMap);
		return FALSE;
	}


	CloseHandle(hFile);
	CloseHandle(hFileMap);

	return TRUE;

}






void CLeg_DebugDlg::OnOpen()
{
	// TODO: Add your command handler code here

	if (m_isDebuging == TRUE)
	{
		AfxMessageBox("�ով����b�ոդ�!����b�ոեt�@�ӵ{��");
		return;
	}

	//GetExeFilePath(m_SzFilePath);
	CFileDialog filedlg(TRUE, "exe", "", OFN_OVERWRITEPROMPT, "���(*.exe)|*.exe|(*.dll)|*.dll||", this);
	if (filedlg.DoModal() != IDOK){
		return;
	}
	OnInitial(filedlg.GetPathName().GetBuffer(0));



	//�p�G�Τ��I���F�������s m_SzFilePath�S����
	/*if (m_SzFilePath[0] == 0x00){
		return;
	}
	if (!MapPEFile())
	{
		return;
	}
	m_isDebuging = TRUE;*/


	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DebugThreadProc, this, NULL, NULL);

	// TODO:  �b���[�J�z���R�O�B�z�`���{���X
}

//�ոսu�{���
DWORD WINAPI DebugThreadProc(
	LPVOID lpParameter   // thread data
	)
{
	STARTUPINFO si = { 0 };
	//�n��l�Ʀ�����
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	char szFilePath[256] = { 0 };
	CLeg_DebugDlg* pDebug = (CLeg_DebugDlg*)lpParameter;
	//�n�Τu�@�u�{ �Ыؽոնi�{
	if (CreateProcess(pDebug->m_szFilePath, NULL, NULL, NULL, false, DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi) == 0)
	{
		OutputDebugString("�Ыؽոնi�{�X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//��o�X���H���}��X
		pDebug->GetErrorMessage(dwErrorCode);
		return FALSE;

	}

	BOOL isExit = FALSE;//�Q�ոնi�{�O�_�h�X���Ч�
	//�ոըƥ�
	DEBUG_EVENT de = { 0 };
	//�@���t�βĤ@���_�I���Ч�
	BOOL bFirstBp = FALSE;
	//�Ч� �Q�ոսu�{�H��˪��覡��_
	LONGLONG  dwContinueStatus = DBG_CONTINUE;
	//�ոմ`��
	while (!isExit&&WaitForDebugEvent(&de, INFINITE))//�p�G���[�WisExit�h�Q�ոնi�{�h�X��,�ով��ٷ|�@�����ݥ�
	{
		switch (de.dwDebugEventCode)
		{
		case EXCEPTION_DEBUG_EVENT:
			switch (de.u.Exception.ExceptionRecord.ExceptionCode)
			{
			case EXCEPTION_ACCESS_VIOLATION:
			{
											   DWORD dwAccessAddress = 0;
											   //���`�X�ݪ��a�}
											   dwAccessAddress = (LONGLONG)de.u.Exception.ExceptionRecord.ExceptionInformation[1];
											   /*dwContinueStatus = pDebug->ON_EXCEPTION_ACCESS_VIOLATION(
												   (DWORD)de.u.Exception.ExceptionRecord.ExceptionAddress,
												   dwAccessAddress
												   );*/
											   break;
			}
			case EXCEPTION_BREAKPOINT:
				if (bFirstBp)
				{

					dwContinueStatus = pDebug->ON_EXCEPTION_BREAKPOINT((LONGLONG)de.u.Exception.ExceptionRecord.ExceptionAddress);

				}
				else
				{
					//�B�z�t�βĤ@���_�I

					bFirstBp = TRUE;
				}

				break;
			case EXCEPTION_SINGLE_STEP:

				dwContinueStatus = pDebug->ON_EXCEPTION_SINGLE_STEP(
					(LONGLONG)de.u.Exception.ExceptionRecord.ExceptionAddress
					);

				break;


			}

			break;
		case CREATE_THREAD_DEBUG_EVENT:


			//�D�u�{�Ыؤ��|�����ƥ�
			// AfxMessageBox("�u�{�Ы�");
			break;
		case CREATE_PROCESS_DEBUG_EVENT:

			//�D�u�{�Ы�
			dwContinueStatus = pDebug->ON_CREATE_PROCESS_DEBUG_EVENT(de.dwProcessId,
				de.dwThreadId,
				de.u.CreateProcessInfo.lpStartAddress);
			break;

		case EXIT_THREAD_DEBUG_EVENT:
			//�D�u�{�h�X���|���ͦ��ƥ�
			//AfxMessageBox("�u�{�h�X");
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			//�D�u�{�h�X
			AfxMessageBox("�i�{�h�X");

			isExit = TRUE;

			AfxMessageBox("�Q�ոնi�{�h�X");

			break;

		case LOAD_DLL_DEBUG_EVENT:
			//�[��DLL�ƥ�
			pDebug->ON_LOAD_DLL_DEBUG_EVENT(de.u.LoadDll.hFile, de.u.LoadDll.lpBaseOfDll);


			break;
		case UNLOAD_DLL_DEBUG_EVENT:

			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			break;
		}

		//��_�Q�ոսu�{���B��
		if (!ContinueDebugEvent(de.dwProcessId, de.dwThreadId, (LONGLONG)dwContinueStatus))
		{
			OutputDebugString("EasyDbgDlg.cpp 442��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			pDebug->GetErrorMessage(dwErrorCode);

			return DBG_EXCEPTION_NOT_HANDLED;


		}
		//���m���Ч�
		dwContinueStatus = DBG_CONTINUE;


	}



	return 0;

}

	//�B�z�[��DLL�ƥ�
	void CLeg_DebugDlg::ON_LOAD_DLL_DEBUG_EVENT(HANDLE hFile, LPVOID pBase)
	{
		if (hFile == NULL || pBase == NULL)
		{
			return;
		}

		GetDllInfoFromHandle(hFile, pBase);
		GetFileNameFromHandle(hFile, pBase);

	}


	//�o��[��DLL�ɪ����|
	void CLeg_DebugDlg::GetFileNameFromHandle(HANDLE hFile, LPVOID pBase)
	{
		//��DLL�[�� ���ܼҶ��H����n��s
		m_GetModule = TRUE;

		//�ǤJ�Ѽƪ����ĩʧP�_
		if (hFile == NULL)
		{
			AfxMessageBox("�y�`�L��");
			return;
		}

		TCHAR pszFilename[MAX_PATH + 1];
		HANDLE hFileMap;

		// Get the file size.
		DWORD dwFileSizeHi = 0;
		DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

		if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
		{
			_tprintf(TEXT("Cannot map a file with a length of zero.\n"));
			return;
		}

		// Create a file mapping object.

		hFileMap = CreateFileMapping(hFile,
			NULL,
			PAGE_READONLY,
			0,
			0,
			NULL);

		if (hFileMap)
		{
			// Create a file mapping to get the file name.
			void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);

			if (pMem)
			{
				//��o�ɥX��ƫH��
				GetExportFunAddress(hFile, (char*)pMem, pBase);
				if (GetMappedFileName(GetCurrentProcess(),
					pMem,
					pszFilename,
					MAX_PATH))
				{

					// Translate path with device name to drive letters.
					TCHAR szTemp[BUFSIZE];
					szTemp[0] = '\0';

					if (GetLogicalDriveStrings(BUFSIZE - 1, szTemp))
					{
						TCHAR szName[MAX_PATH];
						TCHAR szDrive[3] = TEXT(" :");
						BOOL bFound = FALSE;
						TCHAR* p = szTemp;

						do
						{
							// Copy the drive letter to the template string
							*szDrive = *p;

							// Look up each device name
							if (QueryDosDevice(szDrive, szName, MAX_PATH))
							{
								size_t uNameLen = _tcslen(szName);

								if (uNameLen < MAX_PATH)
								{
									bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0;

									if (bFound && *(pszFilename + uNameLen) == _T('\\'))
									{
										// Reconstruct pszFilename using szTempFile
										// Replace device path with DOS path
										TCHAR szTempFile[MAX_PATH];
										sprintf_s(szTempFile,

											TEXT("%s%s"),
											szDrive,
											pszFilename + uNameLen);
										_tcsncpy_s(pszFilename, szTempFile, MAX_PATH);
									}
								}
							}

							// Go to the next NULL character.
							while (*p++);
						} while (!bFound && *p); // end of string
					}
				}

				UnmapViewOfFile(pMem);
				pMem = NULL;
			}

			CloseHandle(hFileMap);
			hFileMap = NULL;
		}


		m_Result.AddString(pszFilename);

		m_Result.SetTopIndex(m_Result.GetCount() - 1);



	}



	// ��o�ɤJ���Ʀa�}
	
	BOOL CLeg_DebugDlg::GetExportFunAddress(HANDLE hFile, char* pDll, LPVOID pBase)
	{


		PIMAGE_DOS_HEADER pDos = NULL;
		PIMAGE_FILE_HEADER pFileHeader = NULL;
		PIMAGE_OPTIONAL_HEADER pOption = NULL;
		PIMAGE_SECTION_HEADER pSec = NULL;

		//����U���c�����w
		pDos = (PIMAGE_DOS_HEADER)pDll;

		pFileHeader = (PIMAGE_FILE_HEADER)(pDll + pDos->e_lfanew + 4);
		pOption = (PIMAGE_OPTIONAL_HEADER)((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
		pSec = (PIMAGE_SECTION_HEADER)((char*)pOption + pFileHeader->SizeOfOptionalHeader);
		//�`��ƥ�
		DWORD dwSecNum = 0;
		dwSecNum = pFileHeader->NumberOfSections;
		//�ɥX����
		DWORD dwExportRva = 0;

		dwExportRva = pOption->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;


		DWORD dwExportOffset = 0;
		//��o�ɤJ����󰾲�
		dwExportOffset = RvaToFileOffset(dwExportRva, dwSecNum, pSec);
		PIMAGE_EXPORT_DIRECTORY pExp = NULL;
		pExp = (PIMAGE_EXPORT_DIRECTORY)(pDll + dwExportOffset);

		EXPORT_FUN_INFO ExFun = { 0 };


		DWORD dwNameOffset = 0;
		dwNameOffset = RvaToFileOffset(pExp->Name, dwSecNum, pSec);
		char*pName = NULL;
		//DLL�W
		pName = (char*)(pDll + dwNameOffset);
		strcpy_s(ExFun.szDLLName, pName);

		DWORD dwBase = 0;
		dwBase = pExp->Base;
		DWORD dwFunNum = 0;
		dwFunNum = pExp->NumberOfFunctions;
		for (DWORD j = 0; j<dwFunNum; j++)
		{
			//���M����Ʀa�}�Ʋ�
			PDWORD pAddr = (PDWORD)(pDll + RvaToFileOffset(pExp->AddressOfFunctions, dwSecNum, pSec));
			//�a�}����
			if (pAddr[j] != 0)
			{
				//�q�L�Ǹ��o�������ƦW�ƲդU��
				//�Ǹ��Ʋ�
				PWORD pNum = (PWORD)(pDll + RvaToFileOffset(pExp->AddressOfNameOrdinals, dwSecNum, pSec));
				for (WORD k = 0; k<pExp->NumberOfNames; k++)
				{
					//�b�Ǹ��Ʋը���Ǹ��ۦP�� ���U�еM�ZŪ��ƦW
					if (j == pNum[k])
					{
						//�ɥX��ƦW(���ܶq�W�Ʋ�) �o�쪺�ORVA
						PDWORD pName = (PDWORD)(pDll + RvaToFileOffset(pExp->AddressOfNames, dwSecNum, pSec));

						char *pszName = (char*)(pDll + RvaToFileOffset(pName[k], dwSecNum, pSec));

						memcpy(&ExFun.szFunName, pszName, strlen(pszName) + 1);


						if (pBase)
						{
							ExFun.dwAddress = (LONGLONG)pBase + pAddr[j];
							//�[�JCMAP��
							m_ExFunList.SetAt(ExFun.dwAddress, ExFun);
							//�[�J��ƦW�P�a�}������
							m_Fun_Address.SetAt(pszName, ExFun.dwAddress);
						}


						break;
					}
				}


			}


		}


		return TRUE;

	}


	//�ѼƤ@ �ɤJ��RVA �Ѽ�2�϶����ƥ� �Ѽ�3�϶������a�}
	DWORD CLeg_DebugDlg::RvaToFileOffset(DWORD dwRva, DWORD dwSecNum, PIMAGE_SECTION_HEADER pSec)
	{
		if (dwSecNum == 0)
		{
			return 0;
		}

		for (DWORD i = 0; i<dwSecNum; i++)
		{

			if (dwRva >= pSec[i].VirtualAddress&&dwRva<pSec[i].VirtualAddress + pSec[i].SizeOfRawData)
			{

				return dwRva - pSec[i].VirtualAddress + pSec[i].PointerToRawData;

			}
		}
		return 0;

	}

	bool CLeg_DebugDlg::OnInitial(char* lpszFilename)
	{
		if (lpszFilename == NULL){
			return false;
		}
		strcpy_s(m_szFilePath, lpszFilename);//��l�ƭnDBG���ɮצW��
		m_GetModule = FALSE;
		return true;
	}



	//�B�z CREATE_PROCESS_DEBUG_EVENT �ƥ󪺨�� 
	DWORD CLeg_DebugDlg::ON_CREATE_PROCESS_DEBUG_EVENT(DWORD dwProcessId, DWORD dwThreadId, LPTHREAD_START_ROUTINE lpOepAddress){
		HMODULE hDll = GetModuleHandle("Kernel32.dll");
		if (hDll == NULL)
		{

			hDll = LoadLibrary("Kernel32.dll");
			if (hDll == NULL)
			{
				DWORD dwErrorCode = 0;
				dwErrorCode = GetLastError();
				GetErrorMessage(dwErrorCode);
				return DBG_EXCEPTION_NOT_HANDLED;
			}
		}
		m_tpInfo.hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
		if (m_tpInfo.hThread == INVALID_HANDLE_VALUE){
			return DBG_EXCEPTION_NOT_HANDLED;
		}
		m_tpInfo.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if (m_tpInfo.hThread == INVALID_HANDLE_VALUE){
			return DBG_EXCEPTION_NOT_HANDLED;
		}
		m_tpInfo.OepAddress = lpOepAddress;

		//�����y�`���
		g_hProcess = m_tpInfo.hProcess;
		g_hThread = m_tpInfo.hThread;

		m_tpInfo.dwProcessId = dwProcessId;
		m_tpInfo.dwThreadId = dwThreadId;
		m_tpInfo.OepAddress = lpOepAddress;
		if (!ReadProcessMemory(m_tpInfo.hProcess, m_tpInfo.OepAddress, &m_tpInfo.OriginalCode, 1, NULL))
		{
			OutputDebugString("EasyDbgDlg.cpp 946��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			GetErrorMessage(dwErrorCode);
			return DBG_EXCEPTION_NOT_HANDLED;
		}

		if (!WriteProcessMemory(m_tpInfo.hProcess, m_tpInfo.OepAddress, &m_tpInfo.bCC, 1, NULL))
		{
			OutputDebugString("EasyDbgDlg.cpp 954��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			GetErrorMessage(dwErrorCode);
			return DBG_EXCEPTION_NOT_HANDLED;

		}

		return DBG_CONTINUE;




	}


	/*void CLeg_DebugDlg::ShowAsm(LONGLONG dwAddress){
		BYTE lpBuffer[500] = { 0 };
		OnReadMemory(dwAddress, lpBuffer, 500);
		DisassembleCode((char*)lpBuffer, (char*)lpBuffer + 500, dwAddress);
		return;
	}*/

	BOOL CLeg_DebugDlg::OnReadMemory(IN LONGLONG dwAdderss, OUT BYTE* lpBuffer, DWORD dwSize){
		DWORD dwRet = 0;
		DWORD dwOldProtect = 0;
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAdderss, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		if (!ReadProcessMemory(m_tpInfo.hProcess, (LPVOID)dwAdderss, lpBuffer, dwSize, NULL)){
			return DBG_EXCEPTION_HANDLED;
		}
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAdderss, dwSize, dwOldProtect, &dwRet);
		return true;
	}


	void CLeg_DebugDlg::DisassembleCode(char* StartCodeSection, char* EndCodeSection, LONGLONG virtual_Address){
		LONGLONG aa = 0;
		int i = 0;
		string b;
		BYTE lpBuffer2[50] = { 0 };
		BYTE lpBuffer[16] = { 0 };
		BYTE lpBuffer1[16] = { 0 };
		char szTemp[MAX_PATH] = { 0 };
		/*��l��DISASM���c*/
		(void)memset(&MyDisasm, 0, sizeof(DISASM));
		/*��l��EIP*/
		MyDisasm.EIP = (LONGLONG)StartCodeSection;
		/*��l�Ƶ����a�}*/
		MyDisasm.VirtualAddr = (LONGLONG)virtual_Address;

		/*�]�m��64�줸*/
		MyDisasm.Archi = 64;

		/*DISASM�`���ѪR�N�X*/

		while (!aError){

			//�]�m�w����
			MyDisasm.SecurityBlock = (long)(EndCodeSection - StartCodeSection);
			len = Disasm(&MyDisasm);

			if (len == OUT_OF_BLOCK){
				//(void)printf("Disasm Engine is not allowed to read more memory\n");
				aError = true;
			}
			else if (len == UNKNOWN_OPCODE){
				//(void)printf("unknow opcode\n");
				aError = true;
			}
			else{
				sprintf_s(szTemp, "%16p", MyDisasm.VirtualAddr);
				m_AsmList.InsertItem(i, szTemp);
				OnReadMemory(MyDisasm.VirtualAddr, lpBuffer, len);
				for (int x = 0; x<len; x++){
					aa = aa * 256 + (int)lpBuffer[x];
				}
				sprintf_s(szTemp, "%8X", aa);
				m_AsmList.SetItemText(i, 1, szTemp);

				aa = 0;

				sprintf_s(szTemp, "%s", MyDisasm.CompleteInstr);
				m_AsmList.SetItemText(i, 2, szTemp);

				i = i + 1;

				MyDisasm.EIP = MyDisasm.EIP + len;
				MyDisasm.VirtualAddr = MyDisasm.VirtualAddr + len;

				if (MyDisasm.EIP >= (int)EndCodeSection){
					//(void)printf("End of buffer reached!\n");
					aError = true;
				}

			}


		}

	}

	void CLeg_DebugDlg::OnBnClickedButton1()
	{
		ON_VK_F9();
		// TODO:  �b���[�J����i���B�z�`���{���X
	}


	void CLeg_DebugDlg::ShowRegData(){
		CONTEXT ctThreadContext;
		char szTemp[MAX_PATH] = { 0 };
		ctThreadContext.ContextFlags = CONTEXT_FULL;
		if (OnGetThreadContext(&ctThreadContext) == FALSE){
			return;
		}


		sprintf_s(szTemp, "%16p", ctThreadContext.Rax);
		m_ctl_RegList.SetItemText(REGLIST_RAX, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.Rbx);
		m_ctl_RegList.SetItemText(REGLIST_RBX, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.Rcx);
		m_ctl_RegList.SetItemText(REGLIST_RCX, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.Rdx);
		m_ctl_RegList.SetItemText(REGLIST_RDX, 1, szTemp);

		sprintf_s(szTemp, "%16p", ctThreadContext.Rsp);
		m_ctl_RegList.SetItemText(REGLIST_RSP, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.Rbp);
		m_ctl_RegList.SetItemText(REGLIST_RBP, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.Rsi);
		m_ctl_RegList.SetItemText(REGLIST_RSI, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.Rdi);
		m_ctl_RegList.SetItemText(REGLIST_RDI, 1, szTemp);

		sprintf_s(szTemp, "%16p", ctThreadContext.Rip);
		m_ctl_RegList.SetItemText(REGLIST_RIP, 1, szTemp);

		sprintf_s(szTemp, "%16p", ctThreadContext.R8);
		m_ctl_RegList.SetItemText(REGLIST_R8, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R9);
		m_ctl_RegList.SetItemText(REGLIST_R9, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R10);
		m_ctl_RegList.SetItemText(REGLIST_R10, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R11);
		m_ctl_RegList.SetItemText(REGLIST_R11, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R12);
		m_ctl_RegList.SetItemText(REGLIST_R12, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R13);
		m_ctl_RegList.SetItemText(REGLIST_R13, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R14);
		m_ctl_RegList.SetItemText(REGLIST_R14, 1, szTemp);
		sprintf_s(szTemp, "%16p", ctThreadContext.R15);
		m_ctl_RegList.SetItemText(REGLIST_R15, 1, szTemp);


		sprintf_s(szTemp, "%08X", ctThreadContext.SegCs);
		m_ctl_RegList.SetItemText(REGLIST_CS, 1, szTemp);
		sprintf_s(szTemp, "%08X", ctThreadContext.SegSs);
		m_ctl_RegList.SetItemText(REGLIST_SS, 1, szTemp);
		sprintf_s(szTemp, "%08X", ctThreadContext.SegDs);
		m_ctl_RegList.SetItemText(REGLIST_DS, 1, szTemp);
		sprintf_s(szTemp, "%08X", ctThreadContext.SegEs);
		m_ctl_RegList.SetItemText(REGLIST_ES, 1, szTemp);
		sprintf_s(szTemp, "%08X", ctThreadContext.SegFs);
		m_ctl_RegList.SetItemText(REGLIST_FS, 1, szTemp);
		sprintf_s(szTemp, "%08X", ctThreadContext.SegGs);
		m_ctl_RegList.SetItemText(REGLIST_GS, 1, szTemp);

		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0001);
		m_ctl_RegList.SetItemText(REGLIST_CF, 1, szTemp);
		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0004);
		m_ctl_RegList.SetItemText(REGLIST_PF, 1, szTemp);
		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0010);
		m_ctl_RegList.SetItemText(REGLIST_AF, 1, szTemp);

		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0040);
		m_ctl_RegList.SetItemText(REGLIST_ZF, 1, szTemp);
		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0080);
		m_ctl_RegList.SetItemText(REGLIST_SF, 1, szTemp);
		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0100);
		m_ctl_RegList.SetItemText(REGLIST_TF, 1, szTemp);

		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0200);
		m_ctl_RegList.SetItemText(REGLIST_IF, 1, szTemp);
		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0400);
		m_ctl_RegList.SetItemText(REGLIST_DF, 1, szTemp);
		sprintf_s(szTemp, "%.1X", (bool)ctThreadContext.EFlags & 0x0800);
		m_ctl_RegList.SetItemText(REGLIST_OF, 1, szTemp);

	}
	BOOL CLeg_DebugDlg::OnGetThreadContext(CONTEXT *pctThreadContext){

		if (GetThreadContext(m_tpInfo.hThread, pctThreadContext)){
			return TRUE;
		}
		else{
			return FALSE;
		}

	}

	void CLeg_DebugDlg::GetDllInfoFromHandle(HANDLE hFile, LPVOID pBase){

		//��DLL�[�� DLL�y��n��s
		m_GetModule = TRUE;
		if (hFile == NULL){
			return;
		}

		HANDLE hFileMap;
		char lpszFileName[MAX_PATH + 1] = {};
		hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

		if (hFileMap)
		{
			void *pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
			if (pMem)
			{
				//
				if (GetMappedFileName(GetCurrentProcess(), pMem, lpszFileName, MAX_PATH))
				{
					TCHAR szTemp[MAX_PATH];
					szTemp[0] = '\0';

					if (GetLogicalDriveStrings(MAX_PATH - 1, szTemp))
					{
						TCHAR szName[MAX_PATH];
						TCHAR szDrive[3] = TEXT(" :");
						BOOL bFound = FALSE;
						TCHAR* p = szTemp;

						do
						{
							*szDrive = *p;
							if (QueryDosDevice(szDrive, szName, MAX_PATH))
							{
								size_t uNameLen = _tcslen(szName);

								if (uNameLen < MAX_PATH)
								{
									bFound = _tcsnicmp(lpszFileName, szName, uNameLen) == 0;

									if (bFound && *(lpszFileName + uNameLen) == _T('\\'))
									{
										TCHAR szTempFile[MAX_PATH];
										sprintf_s(szTempFile,

											TEXT("%s%s"),
											szDrive,
											lpszFileName + uNameLen);
										_tcsncpy_s(lpszFileName, szTempFile, MAX_PATH);
									}
								}
							}

							while (*p++);
						} while (!bFound && *p);
					}
					m_ctl_DllList.InsertItem(0, lpszFileName, 0);
				}
			}
		}
	}


	void CLeg_DebugDlg::OnBnClickedButton2()
	{

		this->UpdateData(TRUE);
		char buffer[100] = { 0 };
		m_dwMemoryAddress.GetWindowText(buffer, 200);


		LONGLONG dwAddress = 0;
		//����U�Z�����a�}
		sscanf(buffer, "%16p", &dwAddress);
		this->ShowMemoryData(dwAddress);
		this->ShowStack();

		// TODO:  �b���[�J����i���B�z�`���{���X
	}

	void CLeg_DebugDlg::ShowMemoryData(LONGLONG dwAddress){

		char szTemp[MAX_PATH] = { 0 };
		BYTE lpBuffer[16] = { 0 };
		m_ctl_DataList.DeleteAllItems();
		for (int i = 0; i<100; i++){
			if (OnIsAddressIsValid(dwAddress + i * 8)){
				ZeroMemory(szTemp, MAX_PATH);
				ZeroMemory(lpBuffer, 10);
				sprintf_s(szTemp, "%16p", dwAddress + i * 8);
				m_ctl_DataList.InsertItem(i, szTemp, 0);
				OnReadMemory(dwAddress + i * 8, lpBuffer, 8);
				sprintf_s(szTemp, "%02X %02X %02X %02X %02X %02X %02X %02X ", lpBuffer[0], lpBuffer[1], lpBuffer[2], lpBuffer[3], lpBuffer[4], lpBuffer[5], lpBuffer[6], lpBuffer[7], lpBuffer[8]);
				m_ctl_DataList.SetItemText(i, 1, szTemp);
				m_ctl_DataList.SetItemText(i, 2, (char*)lpBuffer);
			}
			else{
				break;
			}
		}
	}

	BOOL CLeg_DebugDlg::OnIsAddressIsValid(LONGLONG dwAddress){
		MEMORY_BASIC_INFORMATION mbi;
		LONGLONG dwRet = 0;
		dwRet = VirtualQueryEx(m_tpInfo.hProcess, (LPCVOID)dwAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		if (dwRet = !sizeof(MEMORY_BASIC_INFORMATION)){
			return true;
		}

		if (mbi.State == MEM_COMMIT){
			return true;
		}
		return false;
	}

	void CLeg_DebugDlg::ON_VK_F9()
	{
		SetDlgItemTextA(IDC_STATIC1, "");
		m_IsGo = TRUE;
		SetEvent(hEvent);
		SetDlgItemTextA(IDC_STATIC1, TEXT("�B�椤"));
		//m_pFatherDlg->SetDlgItemTextA(IDC_STATE,"�Q�ոյ{�ǹB�榨�\");
	}

	//�B�z�_�I
	LONGLONG CLeg_DebugDlg::ON_EXCEPTION_BREAKPOINT(LONGLONG dwExpAddress)
	{

		//�P�_�O�_�OOEP�_�I
		if (m_IsOepBP){
			SetDlgItemTextA(IDC_STATIC1, TEXT("�ثe���boep"));
			//��_�_�I
			RecoverBP(m_tpInfo.hProcess, (LONGLONG)m_tpInfo.OepAddress, m_tpInfo.OriginalCode);
			//EIP--
			ReduceEIP();

			ShowRegData();
			ShowAsm((LONGLONG)dwExpAddress);
			//ShowAsm(dwExpAddress); 
			//�]�mU�R�O���q�{�a�}
			m_Uaddress = dwExpAddress;

			//�]�m��FALSE
			m_IsOepBP = FALSE;
			ShowMemoryData(dwExpAddress);
			WaitForSingleObject(hEvent, INFINITE);

			return DBG_CONTINUE;

		}
		
		//�p�G����L�_�I�N��������L�h�N��OD WINDBG�@��
		//�P�_�O�Τ�]�m���_�I�٬O�Q�ոյ{�ǥ��ӴN�s�b���_�I���O
		if (isUserBP(dwExpAddress))
		{
			SetDlgItemTextA(IDC_STATIC1, TEXT("INT3�_�I��F"));

			RecoverBP(m_tpInfo.hProcess, dwExpAddress, m_Recover_BP.bOrginalCode);
		//EIP--
		ReduceEIP();
		//�p�G�O�۰ʳ�B�Ҧ���INT3
		if (m_IsAutoF8)
		{
		ShowRegData();
		ShowAsm((LONGLONG)dwExpAddress);
		m_Uaddress=dwExpAddress;
		//�R���o�����_�I �D�ä[���_�I
		DeleteUserBP(m_tpInfo.hProcess, dwExpAddress);

		ON_VK_F8();

		return DBG_CONTINUE;

		}


		if (m_IsF8 || m_IsGo)
		{
		if(m_IsGo)
		{
		//�M�ŦC���
		m_Result.ResetContent();

		}
		//�R���o�����_�I �D�ä[���_�I
		DeleteUserBP(m_tpInfo.hProcess,dwExpAddress);


		ShowRegData();

		ShowAsm(dwExpAddress);
		m_Uaddress=dwExpAddress;

		m_IsF8=FALSE;
		m_IsGo=FALSE;


		WaitForSingleObject(hEvent,INFINITE);
		}


		return DBG_CONTINUE;
		}
		
		//���O�Τ��_�I �N���B�z
		return DBG_EXCEPTION_NOT_HANDLED;

	}


	void CLeg_DebugDlg::RecoverBP(HANDLE hProcess, LONGLONG dwBpAddress, BYTE bOrignalCode)
	{
		DWORD dwOldProtect = 0;
		DWORD dwRet = 0;
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (!WriteProcessMemory(hProcess, (LPVOID)dwBpAddress, &bOrignalCode, sizeof(bOrignalCode), NULL))
		{
			OutputDebugString("EasyDbgDlg.cpp 1694��X��");
			DWORD dwErrcode = 0;
			dwErrcode = GetLastError();
			//�V�Τ��X���~�H��
			GetErrorMessage(dwErrcode);
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, dwOldProtect, &dwRet);
			return;
		}
	}

	void CLeg_DebugDlg::ReduceEIP()
	{

		CONTEXT ct = { 0 };
		ct.ContextFlags = CONTEXT_FULL;
		GetThreadContext(m_tpInfo.hThread, &ct);
		ct.Rip--;
		SetThreadContext(m_tpInfo.hThread, &ct);

	}

	//�P�_�O�_�O�Τ�]�m��INT3�_�I �q�L�d��INT3��� 
	BOOL CLeg_DebugDlg::isUserBP(LONGLONG dwBpAddress){
		POSITION pos = NULL;
		//�ЧӬO�_���
		BOOL isYes = FALSE;
		pos = m_Int3BpList.GetHeadPosition();
		while (pos != NULL)
		{
			INT3_BP bp = m_Int3BpList.GetNext(pos);
			//�P�_���_�I�a�}�O�_�b�a�}�C��
			if (bp.dwAddress == dwBpAddress)
			{
				//�p�G���,�P�_�O�_�O�ä[�_�I �O�h�ݭn�b�������`���b�]�m���_�I
				//�b��B���`�����]�_�I�Z�b���]m_Recover_BP.isNeedRecover��FALSE
				m_Recover_BP.isNeedRecover = bp.isForever;
				m_Recover_BP.dwAddress = bp.dwAddress;
				m_Recover_BP.bOrginalCode = bp.bOriginalCode;


				isYes = TRUE;

				break;
			}
		}

		return isYes;

	}
	//�R���Τ��_�I
	void CLeg_DebugDlg::DeleteUserBP(HANDLE hProcess, LONGLONG dwBpAddress)
	{
		//�P�_�n�R���_�I�a�}�b���b�_�I���
		POSITION pos = NULL;
		INT3_BP bp = { 0 };
		BOOL isFind = FALSE; 
		pos = m_Int3BpList.GetHeadPosition();
		while (pos != NULL)
		{
			bp = m_Int3BpList.GetNext(pos);
			if (bp.dwAddress == dwBpAddress)
			{
				//�Ҽ{�즳�P�@�a�}�U����_�I �Y�{���_�I�M�ä[�_�I�pG�R�O������continue
				if (bp.isForever)
				{
					isFind = TRUE;
					//��_����Ӫ��r�`
					RecoverBP(hProcess, dwBpAddress, bp.bOriginalCode);


					if (m_isDelete)
					{

						if (m_Int3BpList.GetCount() == 1)
						{
							m_Int3BpList.RemoveHead();
							m_isDelete = FALSE;
							SetDlgItemText(IDC_STATIC1, "�_�I�R�����\");
							return;
						}

						if (pos == NULL)
						{
							m_Int3BpList.RemoveTail();
							m_isDelete = FALSE;
							SetDlgItemText(IDC_STATIC1, "�_�I�R�����\");
							return;


						}

						m_Int3BpList.GetPrev(pos);

						m_Int3BpList.RemoveAt(pos);
						SetDlgItemText(IDC_STATIC1, "�_�I�R�����\");
					}
					m_isDelete = FALSE;

					continue;;
				}
				else
				{
					//�b�o���R���D�ä[�_�I

					if (m_Int3BpList.GetCount() == 1)
					{
						m_Int3BpList.RemoveHead();
						m_isDelete = FALSE;
						SetDlgItemText(IDC_STATIC1, "�_�I�R�����\");
						return;
					}

					if (pos == NULL)
					{
						m_Int3BpList.RemoveTail();
						m_isDelete = FALSE;
						SetDlgItemText(IDC_STATIC1, "�_�I�R�����\");
						return;


					}

					m_Int3BpList.GetPrev(pos);

					m_Int3BpList.RemoveAt(pos);
					SetDlgItemText(IDC_STATIC1, "�_�I�R�����\");

					continue;
				}
				//���

			}
		}
		//�p�G�S���b�_�I�����즹�a�}
		if (!isFind)
		{
			AfxMessageBox("�n�R�����_�I�O�L���_�I");
			return;

		}





	}


	void CLeg_DebugDlg::OnBnClickedButton3()
	{
		ON_VK_F8();
		// TODO:  �b���[�J����i���B�z�`���{���X
	}

	//F8�䪺�B�z��� ��B�B�L
	void CLeg_DebugDlg::ON_VK_F8()
	{

		SetDlgItemText(IDC_STATIC1, "");
		CONTEXT ct = { 0 };
		ct.ContextFlags = CONTEXT_FULL;
		if (!GetThreadContext(m_tpInfo.hThread, &ct))
		{
			OutputDebugString("EasyDbgDlg.cpp 1178��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			GetErrorMessage(dwErrorCode);
			return;
		}
		BYTE szCodeBuffer[40] = { 0 };

		DWORD dwOldProtect = 0;
		DWORD dwRet = 0;

		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)ct.Rip, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		//��o��eEIP�B�����O
		if (!ReadProcessMemory(m_tpInfo.hProcess, (LPCVOID)ct.Rip, szCodeBuffer, sizeof(szCodeBuffer), NULL))
		{
			OutputDebugString("EasyDbgDlg.cpp 1193��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			GetErrorMessage(dwErrorCode);
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)ct.Rip, 4, dwOldProtect, &dwRet);
			return;
		}
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)ct.Rip, 4, dwOldProtect, &dwRet);

		char szAsm[120] = { 0 };
		char szOpCode[120] = { 0 };
		UINT CodeSize = 0;
		//�϶׽s�}�P�_��e���O�O���Ocall���O
		Decode2AsmOpcode(szCodeBuffer, (char*)szAsm, szOpCode,(UINT*)&CodeSize, ct.Rip);
		if (szAsm[0] == 'c' && szAsm[1] == 'a' && szAsm[2] == 'l' && szAsm[3] == 'l')
		{
			//�p�G��e���O�Ocall���O,���\�N�b�U�@�����O�W�U�{���_�I

			//�P�_�p�G�U�@�����O�w�g���_�I�F,�h���ݭn�b�U
			POSITION pos = NULL;
			pos = m_Int3BpList.GetHeadPosition();
			INT3_BP bp = { 0 };

			while (pos != NULL)
			{
				bp = m_Int3BpList.GetNext(pos);
				//�p�G����_�I�h ���ݭn�b�U�_�I
				if (bp.dwAddress == ct.Rip + CodeSize)
				{
					//�]�m�ЧӦ�
					m_IsF8 = TRUE;

					SetEvent(hEvent);
					return;

				}

			}
			//�D�ä[�_�I
			bp.dwAddress = ct.Rip + CodeSize;
			bp.isForever = FALSE;

			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)bp.dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			if (!ReadProcessMemory(m_tpInfo.hProcess, (LPCVOID)bp.dwAddress, &bp.bOriginalCode, sizeof(BYTE), NULL))
			{
				OutputDebugString("EasyDbgDlg.cpp 1239��X��");
				DWORD dwErrorCode = 0;
				dwErrorCode = GetLastError();
				GetErrorMessage(dwErrorCode);
				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)bp.dwAddress, 4, dwOldProtect, &dwRet);
				return;
			}
			if (!WriteProcessMemory(m_tpInfo.hProcess, (LPVOID)bp.dwAddress, &m_tpInfo.bCC, sizeof(BYTE), NULL))
			{
				OutputDebugString("EasyDbgDlg.cpp 1248��X��");
				DWORD dwErrorCode = 0;
				dwErrorCode = GetLastError();
				GetErrorMessage(dwErrorCode);
				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)bp.dwAddress, 4, dwOldProtect, &dwRet);
				return;
			}
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)bp.dwAddress, 4, dwOldProtect, &dwRet);
			FlushInstructionCache(m_tpInfo.hProcess, (LPCVOID)bp.dwAddress, sizeof(BYTE));
			//���_�I�[�J���
			m_Int3BpList.AddTail(bp);
			//�]�m�ЧӦ�
			m_IsF8 = TRUE;


			SetEvent(hEvent);

		}
		else
		{
			//�p�G��e���O���OCALL���O,���\�N�m��B
			ON_VK_F7();
		}


	}


	//F7�䪺�B�z��� ��B�B�J
	void CLeg_DebugDlg::ON_VK_F7()
	{
		//�m��B
		SetDlgItemText(IDC_STATIC1, "");
		CONTEXT ct;
		ct.ContextFlags = CONTEXT_FULL;
		GetThreadContext(m_tpInfo.hThread, &ct);
		ct.EFlags |= 0x100;
		SetThreadContext(m_tpInfo.hThread, &ct);

		SetEvent(hEvent);
	}

	//vu���ASM
	void CLeg_DebugDlg::ShowAsm(LONGLONG dwStartAddress)
	{

		ShowAsmInWindow(dwStartAddress);

		//��ܰ�� �q�{�۰ʳ�B�Ҧ��U����ܰ�̥H�W�[�t��
		if (!m_IsAutoF7 && !m_IsAutoF8)
		{
			ShowStack();
		}

		BYTE pCode[40] = { 0 };

		DWORD dwOldProtect = 0;
		DWORD dwRet = 0;
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwStartAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		if (!ReadProcessMemory(m_tpInfo.hProcess, (LPCVOID)dwStartAddress, pCode, sizeof(pCode), NULL))
		{
			OutputDebugString("EasyDbgDlg.cpp 594��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			//�V�Τ��X���~�H��
			GetErrorMessage(dwErrorCode);
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwStartAddress, 4, dwOldProtect, &dwRet);
			return;
		}
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwStartAddress, 4, dwOldProtect, &dwRet);
		//�P�_�O�_���_�I�R���b�_�I��� �Y�R���h�N�b�w�R�Ϥ���_ 
		for (int i = 0; i<16; i++)
		{

			POSITION pos = NULL;
			pos = m_Int3BpList.GetHeadPosition();
			while (pos != NULL)
			{
				INT3_BP bp = m_Int3BpList.GetNext(pos);
				//�P�_�_�I�a�}�O�_�R�W�b�o�q�w�R�Ϥ�
				//�٭�ä[�_�I���F��

				if (bp.dwAddress == dwStartAddress + i)
				{
					//�p�G�R�� �h���������Τ��_�I�h���r�`�٭�
					pCode[i] = bp.bOriginalCode;
				}



			}


		}

		char szAsm[120] = { 0 };
		char szOpCode[120] = { 0 };
		UINT CodeSize = 0;

		Decode2AsmOpcode(pCode, szAsm, szOpCode, &CodeSize, dwStartAddress);
		EXPORT_FUN_INFO expFun = { 0 };
		//�p�G��������ܤ覡
		if (IsExportFun(szAsm, expFun))
		{
			//��ܦb�C��ر���
			/*char szResult[200] = { 0 };
			sprintf(szResult, "%16p    %s        %s <%s.%s>", dwStartAddress, szOpCode, szAsm, expFun.szDLLName, expFun.szFunName);
			m_Result.AddString(szResult);

			m_Result.SetTopIndex(m_Result.GetCount() - 1);*/


			//�p�G�b�۰�F8�Ҧ�
			if (m_IsAutoF8)
			{
				OPCODE_RECORD op = { 0 };
				//�p�G�ӫ��O�b�M�g���w�s�b �N���A�g��� (�P�_�a�})
				if (m_Opcode.Lookup(dwStartAddress, op))
				{
					return;
				}
				//�p�G�S���N�[�J�M�g��}�g���
				op.dwAddress = dwStartAddress;
				m_Opcode.SetAt(dwStartAddress, op);
				//���ɤ]�n������ܤ覡
				char szNowShow[100] = { 0 };
				sprintf(szNowShow, "%s <%s.%s>", szAsm, expFun.szDLLName, expFun.szFunName);
				WriteOpcodeToFile(dwStartAddress, szNowShow);
			}

			return;
		}
		//��ܦb�C��ر���
		/*char szResult[200] = { 0 };
		sprintf(szResult, "%16p    %s        %s", dwStartAddress, szOpCode, szAsm);
		m_Result.AddString(szResult);

		m_Result.SetTopIndex(m_Result.GetCount() - 1);*/

		//�p�G�b�۰�F8�Ҧ�
		if (m_IsAutoF8)
		{
			OPCODE_RECORD op = { 0 };
			//�p�G�ӫ��O�b�M�g���w�s�b �N���A�g��� (�P�_�a�})
			if (m_Opcode.Lookup(dwStartAddress, op))
			{
				return;
			}
			//�p�G�S���N�[�J�M�g��}�g���
			op.dwAddress = dwStartAddress;
			m_Opcode.SetAt(dwStartAddress, op);
			WriteOpcodeToFile(dwStartAddress, szAsm);

		}




	}


	//�b�϶׽s���f��ܶ׽s�N�X  �Ѽ� �n���G�����O�a�}
	void CLeg_DebugDlg::ShowAsmInWindow(LONGLONG dwStartAddress)
	{
		//�]�m���D
		SetDebuggerTitle(dwStartAddress);

		//�P�_�Ӧa�}�O�_�b��e��ܪ����O�a�}�Ʋդ�
		LONGLONG dwRet = 0;
		dwRet = IsFindAsmAddress(dwStartAddress);
		if (dwRet != -1)
		{

			m_AsmList.SetItemState(dwRet, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			return;

		}
		//�p�G���b�ƲմN���sŪ
		m_AsmList.DeleteAllItems();
		CString szText;
		for (int k = 0; k<20; k++)
		{
			BYTE pCode[40] = { 0 };

			DWORD dwOldProtect = 0;
			DWORD dwRet = 0;
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwStartAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			if (!ReadProcessMemory(m_tpInfo.hProcess, (LPCVOID)dwStartAddress, pCode, sizeof(pCode), NULL))
			{
				OutputDebugString("EasyDbgDlg.cpp 4296��X��");
				DWORD dwErrorCode = 0;
				dwErrorCode = GetLastError();
				//�V�Τ��X���~�H��
				GetErrorMessage(dwErrorCode);
				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwStartAddress, 4, dwOldProtect, &dwRet);
				return;
			}
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwStartAddress, 4, dwOldProtect, &dwRet);

			for (int i = 0; i<16; i++)
			{

				POSITION pos = NULL;
				pos = m_Int3BpList.GetHeadPosition();
				while (pos != NULL)
				{
					INT3_BP bp = m_Int3BpList.GetNext(pos);
					//�P�_�_�I�a�}�O�_�R�W�b�o�q�w�R�Ϥ�
					if (bp.dwAddress == dwStartAddress + i)
					{
						//�p�G�R�� �h���������Τ��_�I�h���r�`�٭�
						pCode[i] = bp.bOriginalCode;
					}
				}


			}

			char szAsm[120] = { 0 };
			char szOpCode[120] = { 0 };
			UINT CodeSize = 0;
			Decode2AsmOpcode(pCode, szAsm, szOpCode, &CodeSize, dwStartAddress);
			EXPORT_FUN_INFO expFun = { 0 };
			//�p�G��������ܤ覡
			if (IsExportFun(szAsm, expFun))
			{
				//��ܦb�C��ر���
				szText.Format("%16p", dwStartAddress);
				m_AsmList.InsertItem(k, szText);
				m_AsmList.SetItemText(k, 1, szOpCode);
				szText.Format("%s <%s.%s>", szAsm, expFun.szDLLName, expFun.szFunName);
				m_AsmList.SetItemText(k, 2, szText);

				m_AsmAddress[k] = dwStartAddress;
				dwStartAddress = CodeSize + dwStartAddress;
				continue;
			}
			//��ܦb�C��ر���
			szText.Format("%16p", dwStartAddress);
			m_AsmList.InsertItem(k, szText);
			m_AsmList.SetItemText(k, 1, szOpCode);
			m_AsmList.SetItemText(k, 2, szAsm);

			m_AsmAddress[k] = dwStartAddress;
			dwStartAddress = CodeSize + dwStartAddress;

		}


		m_AsmList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);







	}


	//�]�m�ով����D(�b�ոդ��\�{��,�H�η�e���O�b���ӼҶ�)
	//�ѼƬ���e���O�a�}
	void CLeg_DebugDlg::SetDebuggerTitle(LONGLONG dwAddress)
	{

		//�p�G�Ҷ��ݭn��s  �u�n��DLL�[���N�ݭn��s
		if (m_GetModule)
		{

			if (!GetCurrentModuleList(m_tpInfo.hProcess))
			{
				return;

			}

		}

		//�P�_��e�a�}�b���ӼҶ�

		POSITION pos = NULL;
		pos = m_Module.GetHeadPosition();
		CString szText;
		while (pos != NULL)
		{
			MODULE_INFO mem = { 0 };
			mem = m_Module.GetNext(pos);
			if (dwAddress >= mem.dwBaseAddress && dwAddress <= (mem.dwSize + mem.dwBaseAddress))
			{

				MODULE_INFO mFirst = { 0 };
				mFirst = m_Module.GetHead();
				//�]�m���D
				szText.Format("Leg_Dbg -%s- [CPU - �D�u�{,�Ҳ� - %s]", mFirst.szModuleName, mem.szModuleName);
				SetWindowText(szText);
				break;

			}


		}



	}
	//��o��e�[���Ҷ��H��
	BOOL CLeg_DebugDlg::GetCurrentModuleList(HANDLE hProcess)
	{
		if (hProcess == NULL)
		{
			return FALSE;
		}
		//�R���Ҧ�����
		m_Module.RemoveAll();


		HMODULE  hModule[500];
		//������^���r�`��
		DWORD nRetrun = 0;
		//�T�|
		BOOL isSuccess = EnumProcessModules(hProcess, hModule, sizeof(hModule), &nRetrun);
		if (isSuccess == 0)
		{
			OutputDebugString(TEXT("EasyDbgDlg.cpp 4538��X��"));
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);

			return FALSE;

		}

		TCHAR ModuleName[500];
		//�Ҷ��H�����c��
		MODULEINFO minfo;
		//�}�l�K�[
		for (DWORD i = 0; i<(nRetrun / sizeof(HMODULE)); i++)
		{
			MODULE_INFO mem = { 0 };
			//����Ҷ��W
			DWORD nLength = GetModuleBaseName(hProcess, hModule[i], ModuleName, sizeof(ModuleName));
			if (nLength == 0)
			{
				OutputDebugString(TEXT("EasyDbgDlg.cpp 4559��X��"));
				DWORD dwErrorCode = 0;
				dwErrorCode = GetLastError();
				//��X���~�H��
				GetErrorMessage(dwErrorCode);

				return FALSE;
			}

			strncpy(mem.szModuleName, ModuleName, strlen(ModuleName) + 1);
			//�榡�ƼҶ���}
			mem.dwBaseAddress = (LONGLONG)hModule[i];
			//����Ҷ��H��
			nLength = GetModuleInformation(g_hProcess, hModule[i], &minfo, sizeof(minfo));
			if (nLength == 0)
			{
				OutputDebugString(TEXT("EasyDbgDlg.cpp 4575��X��"));
				DWORD dwErrorCode = 0;
				dwErrorCode = GetLastError();
				//��X���~�H��
				GetErrorMessage(dwErrorCode);

				return FALSE;

			}

			mem.dwSize = minfo.SizeOfImage;
			//�K�[�����
			m_Module.AddTail(mem);


		}
		//��Чӳ]��FALSE
		m_GetModule = FALSE;
		return TRUE;


	}

	//�ھڭn��ܪ����O�a�} �P�_��eIsExportFun���O�a�}�Ʋդ��O�_�� �Y���N��^��U�� �_�h��^-1
	LONGLONG CLeg_DebugDlg::IsFindAsmAddress(LONGLONG dwStartAddress)
	{
		for (DWORD i = 0; i<25; i++)
		{
			if (dwStartAddress == m_AsmAddress[i])
			{
				return i;
			}
		}
		return -1;
	}

	//�P�_�ѪR���O������ƽե�
	BOOL CLeg_DebugDlg::IsExportFun(char* szBuffer, EXPORT_FUN_INFO& expFun)
	{
		if (szBuffer == NULL)
		{
			return FALSE;
		}
		//���O����
		int nLength = 0;
		nLength = strlen(szBuffer);
		char szCall[5] = { 0 };
		char szJmp[4] = { 0 };
		//�ݬO���OCALL JMP������ //��CALL �H�s���n�B�z
		//�� call [00400000] call dword ptr[00400000]  jmp [00400000]�i��ѪR �`�N�@�w�n��[]�_�h�O�b�եΦۨ����
		memcpy(szCall, szBuffer, 4);
		memcpy(szJmp, szBuffer, 3);


		//�Ȯɤ��B�zCALL reg�����p
		if (szBuffer[5] == 'e')
		{
			return FALSE;
		}



		if (strcmp(szCall, "call") == 0 || strcmp(szJmp, "jmp") == 0)
		{
			//�p�G�����O[]�h�����ѪR
			if (nLength != 0 && szBuffer[nLength - 1] == ']')
			{
				//���[]�����a�}�� �}�ѪR��ƦW
				char Address[20] = { 0 };
				for (int i = 0; i<16; i++)
				{
					Address[i] = szBuffer[nLength - 16 + i ];

				}
				LONGLONG dwAddress = 0;

				sscanf(Address, "%16p", &dwAddress);
				//Ū���a�}�ȳB�����e
				LONGLONG dwActualAddress = 0;
				//�ק�O�@�ݩ� 
				DWORD dwOldProtect = 0;
				DWORD dwRet = 0;
				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 8, PAGE_READONLY, &dwOldProtect);
				if (!ReadProcessMemory(m_tpInfo.hProcess, (LPVOID)dwAddress, &dwActualAddress, sizeof(LONGLONG), NULL))
				{
					OutputDebugString("EasyDbgDlg.cpp 3669��X��");
					DWORD dwErrorCode = 0;
					dwErrorCode = GetLastError();


					VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
					return FALSE;
				}


				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
				//�d�ߦ��S���ŦX����Ʀa�}
				if (m_ExFunList.Lookup(dwActualAddress, expFun))
				{
					return TRUE;
				}



			}


			//�p�G���O[]�N�ݨ�U�@���O���O[]
			else if (nLength != 0 && szBuffer[nLength - 1] != ']')
			{
				//���ѪR�h�Ÿ��F ���ѪR����

				//���a�}�� �϶׽s�U�@�����O
				char Address[20] = { 0 };


				if (szBuffer[2] == 'p'){
					for (int j = 0; j<14; j++)
					{

						Address[j] = szBuffer[nLength - 16 + j + 1];
					}
				}
				/*else if ((szBuffer[0] = 'c')&&(szBuffer[4] = ' ')){
					for (int j = 0; j<8; j++)
					{
						Address[j] = szBuffer[nLength - 9 + j];
					}
				}*/
				else{
					for (int j = 0; j<14; j++)
					{
						Address[j] = szBuffer[nLength - 16 + j + 1];
					}
				}
				

				LONGLONG dwAddress = 0;
				//error
				sscanf(Address, "%16p", &dwAddress);
				//Ū���a�}�ȳB�����e
				DWORD dwActualAddress = 0;
				//�ק�O�@�ݩ� 
				DWORD dwOldProtect = 0;
				DWORD dwRet = 0;
				//�o���N���P�_�F�P�_Read�O�@�˪��ĪG
				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
				BYTE pCode[40] = { 0 };
				if (!ReadProcessMemory(m_tpInfo.hProcess, (LPVOID)dwAddress, &pCode, sizeof(pCode), NULL))
				{
					OutputDebugString("EasyDbgDlg.cpp 3717��X��");
					DWORD dwErrorCode = 0;
					dwErrorCode = GetLastError();

					//GetErrorMessage(dwErrorCode);
					return FALSE;
				}

				VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);


				for (int i = 0; i<16; i++)
				{

					POSITION pos = NULL;
					pos = m_Int3BpList.GetHeadPosition();
					while (pos != NULL)
					{
						INT3_BP bp = m_Int3BpList.GetNext(pos);
						//�P�_�_�I�a�}�O�_�R�W�b�o�q�w�R�Ϥ�
						if (bp.dwAddress == dwAddress + i)
						{
							//�p�G�R�� �h���������Τ��_�I�h���r�`�٭�
							pCode[i] = bp.bOriginalCode;
						}
					}
				}

				char szAsm[120] = { 0 };
				char szOpCode[120] = { 0 };
				UINT CodeSize = 0;
				Decode2AsmOpcode(pCode, szAsm, szOpCode, &CodeSize, dwAddress);
				//�P�_�������O

				//���O����
				int nLength = 0;
				nLength = strlen(szAsm);
				char szCall[5] = { 0 };
				char szJmp[4] = { 0 };
				//�ݬO���OCALL JMP������
				//�� call [00400000] call dword ptr[00400000]  jmp [00400000]�i��ѪR �`�N�@�w�n��[]�_�h�O�b�եΦۨ����
				memcpy(szCall, szBuffer, 4);
				memcpy(szJmp, szBuffer, 3);
				if (strcmp(szCall, "call") == 0 || strcmp(szJmp, "jmp") == 0)
				{
					//�p�G�����O[]�h�����ѪR
					if (nLength != 0 && szAsm[nLength - 1] == ']')
					{
						//���[]�����a�}�� �}�ѪR��ƦW
						char Address[20] = { 0 };
						//���D�I

						for (int i = 0; i<16; i++)
						{
							Address[i] = szAsm[nLength - 18 + i];

						}

						LONGLONG dwAddress = 0;

						sscanf(Address, "%16p", &dwAddress);
						//Ū���a�}�ȳB�����e
						DWORD dwActualAddress = 0;
						//�ק�O�@�ݩ� 
						DWORD dwOldProtect = 0;
						DWORD dwRet = 0;
						VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 8, PAGE_READONLY, &dwOldProtect);
						if (!ReadProcessMemory(m_tpInfo.hProcess, (LPVOID)dwAddress, &dwActualAddress, sizeof(DWORD), NULL))
						{
							OutputDebugString("EasyDbgDlg.cpp 3783��X��");
							DWORD dwErrorCode = 0;
							dwErrorCode = GetLastError();
							// GetErrorMessage(dwErrorCode);

							VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
							return FALSE;
						}


						VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
						//�d�ߦ��S���ŦX����Ʀa�}
						


					}
				}

			}
		}
		return FALSE;

	}


	//��ܰ��
	void CLeg_DebugDlg::ShowStack()
	{
		m_Stack.DeleteAllItems();
		BYTE lpBuffer[16] = { 0 };
		CONTEXT ct = { 0 };
		ct.ContextFlags = CONTEXT_FULL;
		if (GetThreadContext(m_tpInfo.hThread, &ct) == 0)
		{
			return;
		}
		CString szText;
		for (int i = 0; i<50; i++){
			ZeroMemory(lpBuffer, 16);
			szText.Format("0x%16p", ct.Rsp + i * 8);
			m_Stack.InsertItem(i, szText);
			OnReadMemory(ct.Rsp + i * 8, lpBuffer, 8);
			szText.Format("%02X%02X%02X%02X%02X%02X%02X%02X", lpBuffer[7], lpBuffer[6], lpBuffer[5], lpBuffer[4], lpBuffer[3], lpBuffer[2], lpBuffer[1], lpBuffer[0] );
			m_Stack.SetItemText(i, 1, szText);


		}


	}




	//��O���g�J���  �Ѽ� ���O�a�} ���O�w�R ����ܾ����X �F,
	//��ܤF�b�奻��󤤤��n��� 
	void CLeg_DebugDlg::WriteOpcodeToFile(LONGLONG dwAddress, char* szAsm)
	{
		if (m_hFile == INVALID_HANDLE_VALUE || szAsm == NULL || dwAddress == 0)
		{
			return;
		}

		DWORD dwLength = 0;
		dwLength = strlen(szAsm);
		//�^������
		szAsm[dwLength] = '\r';
		szAsm[dwLength + 1] = '\n';
		char szBuffer[16] = { 0 };

		sprintf(szBuffer, "%08X", dwAddress);

		WriteFile(m_hFile, (LPVOID)szBuffer, sizeof(szBuffer), &dwLength, NULL);

		WriteFile(m_hFile, (LPVOID)szAsm, strlen(szAsm), &dwLength, NULL);






	}


	void CLeg_DebugDlg::OnBnClickedButton4()
	{
		ON_VK_F7();
		// TODO:  �b���[�J����i���B�z�`���{���X
	}

	//��B���`�B�z���  �ѼƲ��`�a�}
	LONGLONG CLeg_DebugDlg::ON_EXCEPTION_SINGLE_STEP(LONGLONG dwExpAddress)
	{


		//�O�_�O�۰ʨB�L�Ҧ�
		if (m_IsAutoF8)
		{
			//�m��B

			ShowAsm(dwExpAddress);
			ShowRegData();
			ON_VK_F8();


			return DBG_CONTINUE;

		}
		//�O�_���۰ʨB�J�Ҧ�
		if (m_IsAutoF7)
		{
			ShowAsm(dwExpAddress);
			ShowRegData();
			ON_VK_F7();

			return DBG_CONTINUE;

		}
		//���P�_���S���n���s��_��INT3�_�I
		if (m_Recover_BP.isNeedRecover)
		{

			DebugSetBp(m_tpInfo.hProcess, m_Recover_BP.dwAddress, m_tpInfo.bCC);

			//���s�m��FALSE
			m_Recover_BP.isNeedRecover = FALSE;
		}
		//�p�G���w���_�I�n��_
		if (m_Recover_HBP.dwIndex != -1)
		{
			//��_�w���_�I
			RecoverHBP(m_Recover_HBP.dwIndex);


			m_Recover_HBP.dwIndex = -1;
		}
		//�p�G�����s�_�I�n��_
		if (m_Recover_Mpage.isNeedRecover)
		{
			DWORD dwOldProtect = 0;
			if (!VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)m_Recover_Mpage.dwBaseAddress, 4,
				m_Recover_Mpage.dwProtect, &dwOldProtect)
				)
			{

				OutputDebugString("EasyDbgDlg.cpp 1595��X��");
				DWORD dwErrorCode = 0;

				dwErrorCode = GetLastError();
				//��X���~�H��
				GetErrorMessage(dwErrorCode);
			}
			//���s�m��FALSE
			m_Recover_Mpage.isNeedRecover = FALSE;
		}


		//�P�_��B���`�O���O�]���w���_�I �YDR6���C�|�즳�S���m��
		LONGLONG dwBpAddress = 0;
		if (IfStepHard(dwBpAddress))
		{
			SetDlgItemText(IDC_STATIC1, TEXT("�w���_�I��F"));
			//���w���_�I�L��
			InvalidHardBP(dwBpAddress);

		}
		//�p�G�O�]���@��G�Ҧ������s�_�I�N���n���ݦP�B�ƥ�
		if (m_isMoreMem)
		{

			if (m_IsF8)
			{
				m_IsGo = FALSE;
				m_isMoreMem = FALSE;
				//ShowAsm(dwExpAddress);
				// ShowReg(m_tpInfo.hThread);
				//�]�mU�R�O���_�l�a�}
				m_Uaddress = dwExpAddress;

				//WaitForSingleObject(hEvent,INFINITE);
				//m_IsF8=FALSE;
				return DBG_CONTINUE;


			}
			//�h��G��F�h��N���s�_�I�����B����_�I
			if (m_IsGo)
			{
				m_isMoreMem = FALSE;

				return DBG_CONTINUE;


			}

			m_isMoreMem = FALSE;
			ShowAsm(dwExpAddress);
			ShowRegData();

			m_Uaddress = dwExpAddress;

			WaitForSingleObject(hEvent, INFINITE);

			return DBG_CONTINUE;


		}


		ShowAsm(dwExpAddress);
		ShowRegData();
		//�]�mU�R�O���_�l�a�}
		m_Uaddress = dwExpAddress;

		WaitForSingleObject(hEvent, INFINITE);

		return DBG_CONTINUE;
	}


	//�]�m�_�I  �_�I�a�} 0xCC �Τ_�ä[�_�I���s��_���_�I
	void CLeg_DebugDlg::DebugSetBp(HANDLE hProcess, LONGLONG dwBpAddress, BYTE bCCode)
	{
		DWORD dwOldProtect = 0;
		DWORD dwRet = 0;
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		if (!WriteProcessMemory(hProcess, (LPVOID)dwBpAddress, &bCCode, sizeof(bCCode), NULL))
		{
			OutputDebugString("EasyDbgDlg.cpp 1724��X��");
			DWORD dwErrcode = 0;
			dwErrcode = GetLastError();
			//�V�Τ��X���~�H��
			GetErrorMessage(dwErrcode);
			VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, dwOldProtect, &dwRet);
			return;
		}

		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, dwOldProtect, &dwRet);
		//��s
		FlushInstructionCache(hProcess, (LPCVOID)dwBpAddress, sizeof(BYTE));

	}

	//��_�w���_�I �ѼƬ� �ոձH�s�����s��
	void CLeg_DebugDlg::RecoverHBP(DWORD dwIndex)
	{
		//�ǤJ�Ѽƪ��P�_
		if (dwIndex == -1)
		{
			AfxMessageBox("��_�w���_�I�X��");
			return;
		}

		CONTEXT ct = { 0 };
		ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
		if (!GetThreadContext(m_tpInfo.hThread, &ct))
		{
			OutputDebugString("EasyDbgDlg.cpp 2857��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);
			return;
		}

		DR7 tagDr7 = { 0 };
		tagDr7.dwDr7 = ct.Dr7;

		switch (dwIndex)
		{

		case 0:
			//�]�mL��
			tagDr7.DRFlag.L0 = 1;
			m_Recover_HBP.dwIndex = -1;
			break;
		case 1:
			tagDr7.DRFlag.L1 = 1;
			m_Recover_HBP.dwIndex = -1;
			break;
		case 2:
			tagDr7.DRFlag.L2 = 1;
			m_Recover_HBP.dwIndex = -1;
			break;
		case 3:
			tagDr7.DRFlag.L3 = 1;
			m_Recover_HBP.dwIndex = -1;
			break;

		}
		//�g �^CONTEXT
		ct.Dr7 = tagDr7.dwDr7;

		if (!SetThreadContext(m_tpInfo.hThread, &ct))
		{
			OutputDebugString("EasyDbgDlg.cpp 2895��X��");

			DWORD dwErrorCode = 0;

			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);
			return;
		}





	}

	//�P�_��B���`�O�_�O�w���_�I�ް_�� �ǥX�Ѽ� �_�I�a�}
	BOOL CLeg_DebugDlg::IfStepHard(LONGLONG& dwBPAddress)
	{

		CONTEXT ct = { 0 };
		ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
		if (!GetThreadContext(m_tpInfo.hThread, &ct))
		{
			OutputDebugString("EasyDbgDlg.cpp 2705��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);
			return FALSE;
		}
		//�P�_Dr6���C4��O�_��0
		int nIndex = ct.Dr6 & 0xf;
		if (nIndex == 0)
		{
			return FALSE;
		}

		switch (nIndex)
		{
		case 0x1:
			//�O�s��쪺�_�I�a�}
			dwBPAddress = ct.Dr0;
			break;
		case 0x2:
			dwBPAddress = ct.Dr1;
			break;
		case 0x4:
			dwBPAddress = ct.Dr2;
			break;
		case 0x8:
			dwBPAddress = ct.Dr3;
			break;
		}
		return TRUE;


	}

	//�ϵw���_�I�ȮɵL��
	void CLeg_DebugDlg::InvalidHardBP(LONGLONG dwBpAddress)
	{
		//�ǤJ�Ѽƪ��P�_
		if (dwBpAddress == 0)
		{
			AfxMessageBox("�_�I��0�L�ĭ�");
			return;
		}

		CONTEXT ct = { 0 };
		ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
		if (!GetThreadContext(m_tpInfo.hThread, &ct))
		{
			OutputDebugString("EasyDbgDlg.cpp 2754��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);
			return;
		}
		//�P�_���_�a�}�b���ӽոձH�s����
		DR7 tagDr7 = { 0 };
		tagDr7.dwDr7 = ct.Dr7;
		//     DWORD dwDr0=ct.Dr0;
		//     DWORD dwDr1=ct.Dr1;
		//     DWORD dwDr2=ct.Dr2;
		//     DWORD dwDr3=ct.Dr3;
		//     switch (dwBpAddress)
		//     {
		// 
		//     case dwDr0:
		//         //�ML�����_�I�L�� ���M�a�}�F,�K�o�ٱo�O�s�a�}
		//         tagDr7.DRFlag.L0=0;
		//         //�]�m�n��_���ոձH�s���s��
		//         m_Recover_HBP.dwIndex=0;
		// 
		//         break;
		//     case dwDr1:
		//         tagDr7.DRFlag.L1=0;
		//         m_Recover_HBP.dwIndex=1;
		// 
		//         break;
		//     case dwDr2:
		//         tagDr7.DRFlag.L2=0;
		//         m_Recover_HBP.dwIndex=2;
		// 
		//         break;
		//     case dwDr3:
		//         tagDr7.DRFlag.L3=0;
		//         m_Recover_HBP.dwIndex=3;
		//         break;
		// 
		//     }
		if (ct.Dr0 == dwBpAddress)
		{
			//�ML�����_�I�L�� ���M�a�}�F,�K�o�ٱo�O�s�a�}
			tagDr7.DRFlag.L0 = 0;
			//�]�m�n��_���ոձH�s���s��
			m_Recover_HBP.dwIndex = 0;

		}

		if (ct.Dr1 == dwBpAddress)
		{
			tagDr7.DRFlag.L1 = 0;
			m_Recover_HBP.dwIndex = 1;

		}

		if (ct.Dr2 == dwBpAddress)
		{
			tagDr7.DRFlag.L2 = 0;
			m_Recover_HBP.dwIndex = 2;

		}

		if (ct.Dr3 == dwBpAddress)
		{
			tagDr7.DRFlag.L3 = 0;
			m_Recover_HBP.dwIndex = 3;
		}

		//��Ȧ^�h
		ct.Dr7 = tagDr7.dwDr7;
		//�]�m�u�{�W�U��
		if (!SetThreadContext(m_tpInfo.hThread, &ct))
		{
			OutputDebugString("EasyDbgDlg.cpp 2828��X��");

			DWORD dwErrorCode = 0;

			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);
			return;
		}


	}

	//�I�����
BOOL CLeg_DebugDlg::PreTranslateMessage(MSG* pMsg)
	{
		// TODO: Add your specialized code here and/or call the base class

		//�B�z ��u��J�R�O������
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		{

			if (m_command.GetFocus()->GetDlgCtrlID() == IDC_EDIT2)
			{

				char buffer[100] = { 0 };
				m_command.GetWindowText(buffer, 200);

				//�B�z�R�O
				Handle_User_Command(buffer);


			}


		}
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){
			if (m_asm_adr.GetFocus()->GetDlgCtrlID() == IDC_EDIT3)
			{

				char buffer[100] = { 0 };
				m_asm_adr.GetWindowText(buffer, 200);


				LONGLONG dwAddress = 0;
				//����U�Z�����a�}
				sscanf(buffer, "%16p", &dwAddress);

				ShowAsm(dwAddress);


			}
		}

		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){
			if (m_dwMemoryAddress.GetFocus()->GetDlgCtrlID() == IDC_EDIT1)
			{

				char buffer[100] = { 0 };
				m_dwMemoryAddress.GetWindowText(buffer, 200);


				LONGLONG dwAddress = 0;
				//����U�Z�����a�}
				sscanf(buffer, "%16p", &dwAddress);

				ShowMemoryData(dwAddress);


			}
		}

		//�B�zF7�ֱ���
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F7)
		{

			ON_VK_F7();
		}
		//�B�zF8�ֱ���
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F8)
		{
			ON_VK_F8();
		}
		//�B�zF9�ֱ���
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F9)
		{
			OnRun();
		}
		//�B�zF6�ֱ���  �۰ʨB�L
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F6)
		{
			OnAutostepout();
		}
		//�B�zF5�ֱ��� �۰ʨB�J
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5)
		{
			OnAutostepinto();
		}

		return CDialog::PreTranslateMessage(pMsg);



	}
	
//�۰ʨB�J
void CLeg_DebugDlg::OnAutostepinto()
{
	DeleteAllBreakPoint();
	m_IsAutoF7 = TRUE;
	ON_VK_F7();
	// TODO: Add your command handler code here

}


void CLeg_DebugDlg::OnAutostepout()
{
	// TODO: Add your command handler code here
	m_hFile = INVALID_HANDLE_VALUE;

	char            szFileName[MAX_PATH] = "Record";
	OPENFILENAME    ofn = { 0 };
	char            CodeBuf[24] = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = szFileName;
	ofn.lpstrDefExt = "txt";
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "��J�n�O�s�����O�O�����W(*.txt)\0*.txt\0";
	ofn.nFilterIndex = 1;
	if (GetSaveFileName(&ofn) == FALSE)
	{
		return;
	}
	//�Ыؤ��
	m_hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//�۰ʳ�B �R���Ҧ��_�I
	DeleteAllBreakPoint();
	m_IsAutoF8 = TRUE;
	ON_VK_F8();


}

//�R���Ҧ��_�I �Τ_�O�����O
void CLeg_DebugDlg::DeleteAllBreakPoint()
{


	POSITION pos = NULL;

	//�R���Ҧ������s�_�I
	pos = m_MemBpList.GetHeadPosition();
	MEM_BP memBP = { 0 };
	while (pos != NULL)
	{
		memBP = m_MemBpList.GetNext(pos);
		DeleteMemBP(memBP.dwBpAddress);


	}
	//�R���Ҧ���INT3�_�I
	INT3_BP bp = { 0 };
	pos = NULL;
	pos = m_Int3BpList.GetHeadPosition();
	while (pos != NULL)
	{
		bp = m_Int3BpList.GetNext(pos);
		//��_����Ӫ��r�`
		RecoverBP(m_tpInfo.hProcess, bp.dwAddress, bp.bOriginalCode);


	}

	m_Int3BpList.RemoveAll();

	//�R���Ҧ����w���_�I

	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_tpInfo.hThread, &ct);
	if (ct.Dr0 != 0)
	{
		DeleteHardBP(ct.Dr0);

	}
	if (ct.Dr1 != 0)
	{
		DeleteHardBP(ct.Dr1);

	}
	if (ct.Dr2 != 0)
	{
		DeleteHardBP(ct.Dr2);

	}
	if (ct.Dr3 != 0)
	{
		DeleteHardBP(ct.Dr3);

	}

}


void CLeg_DebugDlg::OnRun()
{
	// TODO: Add your command handler code here
	ON_VK_F9();

}

//�Τ�R�O���B�z���
void CLeg_DebugDlg::Handle_User_Command(char* szCommand)
{
	//�h���e�Z���Ů�
	if (!Kill_SPACE(szCommand))
	{
		AfxMessageBox("�R�O��J���~");
		return;
	}
	//�ھکR�O�B�z
	switch (szCommand[0])
	{
	case 't':
	case 'T':
		ON_VK_F7();
		break;
	case 'p':
	case 'P':
		ON_VK_F8();
		break;
	case 'u':
	case 'U':
	{
				//uf  ��� ���Ƥ϶׽s
				if (szCommand[1] == 'F' || szCommand[1] == 'f')
				{
					char szName[100] = { 0 };
					sscanf(szCommand, "%s%s", stderr, &szName);
					DisassemblerExcFun(szName);


				}
				else
				{
					LONGLONG dwAddress = 0;
					//����U�Z�����a�}
					sscanf(szCommand, "%s%16p", stderr, &dwAddress);
					ON_U_COMMAND(dwAddress);
				}
				break;
	}
	case 'b':
	case 'B':
		Handle_B_Command(szCommand);

		break;
	case 'g':
	case 'G':
	{
				unsigned int dwAddress = 0;
				//����U�Z�����a�}
				sscanf(szCommand, "%s%x", stderr, &dwAddress);
				ON_G_COMMAND(dwAddress);
				break;

	}
	case 's':
	case 'S':

		//�۰ʨB�L
		OnAutostepout();
		break;
	case 'o':
	case 'O':
		//���X���
		StepOutFromFun();
		break;
	case 'e':
	case 'E':
		if (szCommand[1] == 'B' || szCommand[1] == 'b')
		{
			LONGLONG dwAddress = 0;
			int inn = 0;
			//����U�Z�����a�}
			sscanf(szCommand, "%s%16p%2X", stderr, &dwAddress,&inn);
			ChangeByte(m_tpInfo.hProcess,dwAddress, inn);


		}
		break;
	default:
		AfxMessageBox(TEXT("�R�O���~"));
	}



}


//�B�zeb���
void CLeg_DebugDlg::ChangeByte(HANDLE hProcess, LONGLONG dwAddress, byte chby){
	DWORD dwOldProtect = 0;
	DWORD dwRet = 0;
	int bOriginalCode;
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	//Ū�����r�`
	if (!ReadProcessMemory(hProcess, (LPVOID)dwAddress, &bOriginalCode, sizeof(BYTE), NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 1927��X��");
		DWORD dwErrcode = 0;
		dwErrcode = GetLastError();
		//�V�Τ��X���~�H��
		GetErrorMessage(dwErrcode);

		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
		return;

	}

	//�g�J0xCC
	if (!WriteProcessMemory(hProcess, (LPVOID)dwAddress, &chby, sizeof(chby), NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 1942��X��");
		DWORD dwErrcode = 0;
		dwErrcode = GetLastError();
		//�V�Τ��X���~�H��
		GetErrorMessage(dwErrcode);
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
		return;
	}
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);


	return ;
}


//���X���  �ȾA�Τ_MOV EBP ,ESP���O���Z POP EBP���e �Q�ΰ�̭�zŪ����^�a�}
void CLeg_DebugDlg::StepOutFromFun()
{
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_FULL;
	if (GetThreadContext(m_tpInfo.hThread, &ct) == 0)
	{
		return;
	}

	DWORD dwBpAddress = 0;
	DWORD dwOldProtect = 0;
	DWORD dwRet = 0;
	//Ū����ƪ���^�a�}
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)(ct.Rbp + 4), 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	if (!ReadProcessMemory(m_tpInfo.hProcess, (LPVOID)(ct.Rbp + 4), &dwBpAddress, 4, NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 3961��X��");
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)(ct.Rbp + 4), 4, dwOldProtect, &dwRet);
		return;

	}
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)(ct.Rbp + 4), 4, dwOldProtect, &dwRet);
	ON_G_COMMAND(dwBpAddress);




}


//�h���R�O������M�k�䪺�Ů�r��
BOOL CLeg_DebugDlg::Kill_SPACE(char* szCommand)
{
	//�p��r�ż� ���]�A�̦Z���פ��
	int nSize = strlen(szCommand);
	//�S��J�R�O�N���^����
	if (*szCommand == 0)
	{
		AfxMessageBox("�S����J�R�O");
		return FALSE;
	}
	//�h���e�����Ů�
	for (int i = 0; i<nSize; i++)
	{
		if (szCommand[i] != 0x20)
		{
			//�h���e�����Ů椧�Z���r�Ŧ�j�p
			int  nNowSize = nSize - i;
			for (int j = 0; j<nNowSize; j++)
			{
				//�V�e����
				szCommand[j] = szCommand[i];
				i++;
			}
			szCommand[nNowSize] = 0;

		}
	}
	//���Z�A�h���Z�����Ů�
	for (int i = strlen(szCommand) - 1; i>0; i--)
	{
		//�q�Z�V�e�M��,�J��Ĥ@�Ӥ��O�Ů檺�r�ŧY�i
		if (szCommand[i] != 0x20)
		{
			//�Z���m���פ��
			szCommand[i + 1] = 0;
			break;
		}
	}

	return TRUE;

}

//��DLL�ɥX��ƶi��϶׽s
void CLeg_DebugDlg::DisassemblerExcFun(char* szFunName)
{

	LONGLONG dwAddress = 0;
	if (!m_Fun_Address.Lookup(szFunName, dwAddress))
	{
		AfxMessageBox("�L�����");
		return;
	}

	m_Result.ResetContent();

	//�p�G�Ҷ��ݭn��s  �u�n��DLL�[���N�ݭn��s
	if (m_GetModule)
	{

		if (!GetCurrentModuleList(m_tpInfo.hProcess))
		{
			return;

		}

	}

	//��o��e�϶׽s��Ʀb���ӼҶ�

	POSITION pos = NULL;
	pos = m_Module.GetHeadPosition();
	CString szText;
	while (pos != NULL)
	{
		MODULE_INFO mem = { 0 };
		mem = m_Module.GetNext(pos);
		if (dwAddress >= mem.dwBaseAddress && dwAddress <= (mem.dwSize + mem.dwBaseAddress))
		{

			//��ܭn�϶׽s����ƦW�H�Ψ�Ҧb�Ҷ�
			szText.Format("%s!%s:", mem.szModuleName, szFunName);
			m_Result.AddString(szText);
			m_Result.SetTopIndex(m_Result.GetCount() - 1);
			break;

		}


	}

	//�϶׽s

	ON_U_COMMAND(dwAddress);

}

//�B�zU�R�O �p�G�S���a�}�N�q�H�e���a�}����U �b��B�Ϊ��_�I���`���A��o�Ӧa�}�]����eEIP����
void CLeg_DebugDlg::ON_U_COMMAND(LONGLONG dwAddress)
{
	//�q�{���8�����O
	//�p�G�����F�a�}�h���m_Uaddress
	if (dwAddress)
	{
		m_Uaddress = dwAddress;
	}



	BYTE pCode[120] = { 0 };
	DWORD dwOldProtect = 0;
	DWORD dwRet = 0;
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)m_Uaddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	if (!ReadProcessMemory(m_tpInfo.hProcess, (LPCVOID)m_Uaddress, pCode, sizeof(pCode), NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 1804��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		GetErrorMessage(dwErrorCode);

		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)m_Uaddress, 4, dwOldProtect, &dwRet);
		return;

	}

	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)m_Uaddress, 4, dwOldProtect, &dwRet);


	//�n�P�_�O�_�I�����p �p�G�O�_�I�h������ܥXCC�ӭn�b�w�R�Ϥ��٭쥦
	for (int j = 0; j<120; j++)
	{
		POSITION pos = NULL;
		pos = m_Int3BpList.GetHeadPosition();
		while (pos != NULL)
		{
			INT3_BP bp = m_Int3BpList.GetNext(pos);
			//�P�_�_�I�a�}�O�_�R�W�b�o�q�w�R�Ϥ�
			if (bp.dwAddress == m_Uaddress + j)
			{
				//�p�G�R�� �h���������Τ��_�I�h���r�`�٭�
				pCode[j] = bp.bOriginalCode;
			}
		}

	}


	char szAsm[120] = { 0 };
	char szOpCode[120] = { 0 };
	UINT CodeSize = 0;
	int nIndex = 0;
	//�}�l�϶׽s
	for (int i = 0; i<8; i++)
	{

		Decode2AsmOpcode(&pCode[nIndex], szAsm, szOpCode, &CodeSize, m_Uaddress);
		//��ܦb�C��ر���
		char szResult[200] = { 0 };
		EXPORT_FUN_INFO expFun = { 0 };
		//�p�G�O�ɥX��ƫh�ѪR�X��
		if (IsExportFun(szAsm, expFun))
		{
			sprintf(szResult, "%16p    %s       %s <%s.%s>", m_Uaddress, szOpCode, szAsm, expFun.szDLLName, expFun.szFunName);
			m_Result.AddString(szResult);

			m_Result.SetTopIndex(m_Result.GetCount() - 1);
			m_Uaddress += CodeSize;
			nIndex += CodeSize;
			continue;


		}
		sprintf(szResult, "%16p    %s        %s", m_Uaddress, szOpCode, szAsm);
		m_Result.AddString(szResult);

		m_Result.SetTopIndex(m_Result.GetCount() - 1);
		m_Uaddress += CodeSize;
		nIndex += CodeSize;

	}



}


//�B�zB�R�O
void CLeg_DebugDlg::Handle_B_Command(char* szCommand)
{
	switch (szCommand[1])
	{
	case 'p':
	case 'P':
	{
				LONGLONG dwAddress = 0;

				//����P�Z�����a�}
				sscanf(szCommand, "%s%16p", stderr, &dwAddress);
				if (dwAddress == 0)
				{
					AfxMessageBox("��J���~,�п�J�_�I�a�}");
					break;
				}
				//�]�m�_�I
				UserSetBP(m_tpInfo.hProcess, dwAddress, m_tpInfo.bCC);

				break;
	}
	case 'l':
	case 'L':
		//�C�X��e�_�I
		ListBP();

		break;
	case 'c':
	case 'C':
	{
				unsigned int dwAddress = 0;
				//����C�Z�����a�}
				sscanf(szCommand, "%s%x", stderr, &dwAddress);
				if (dwAddress == 0)
				{
					AfxMessageBox("��J���~,�п�J�_�I�a�}");
					return;
				}
				//�R���ä[���_�I �Чӳ]��TRUE
				m_isDelete = TRUE;
				DeleteUserBP(m_tpInfo.hProcess, dwAddress);

				break;
	}
	case 'h':
		//�]�m�w���_�I�ΧR��
	case 'H':
	{
				//��Τ��J�����O�u��²�檺�P�_
				if (szCommand[2] == 'C' || szCommand[2] == 'c')
				{
					unsigned int dwAddress = 0;
					//����C�Z�����a�}
					sscanf(szCommand, "%s%x", stderr, &dwAddress);
					if (dwAddress == 0)
					{
						AfxMessageBox("��J���~,�п�J�_�I�a�}");
						return;
					}
					DeleteHardBP(dwAddress);

				}
				else
				{

					DWORD dwAddress = 0;
					DWORD dwAttribute = 0;
					DWORD dwLength = 0;
					//�����U�ӭ�
					sscanf(szCommand, "%s%x%x%x", stderr, &dwAddress, &dwAttribute, &dwLength);
					//�]�m�w���_�I
					SetHardBP(dwAddress, dwAttribute, dwLength);
				}

				break;
	}
	case 'm':
	case 'M':
	{
				//�M�����s�_�I
				if (szCommand[2] == 'C' || szCommand[2] == 'c')
				{
					unsigned int dwAddress = 0;
					//����C�Z�����a�}
					sscanf(szCommand, "%s%x", stderr, &dwAddress);
					if (dwAddress == 0)
					{
						AfxMessageBox("��J���~,�п�J�_�I�a�}");
						return;
					}
					DeleteMemBP(dwAddress);

				}
				else
				{

					DWORD dwAddress = 0;
					DWORD dwAttribute = 0;
					DWORD dwLength = 0;
					//�����U�ӭ�
					sscanf(szCommand, "%s%x%x%x", stderr, &dwAddress, &dwAttribute, &dwLength);
					SetMemBP(dwAddress, dwAttribute, dwLength);
				}

				break;
	}
	}


}


//�Τ�]�m�_�I
void CLeg_DebugDlg::UserSetBP(HANDLE hProcess, LONGLONG dwBpAddress, BYTE bCCode)
{
	//�P�_�Ӧa�}�O�_�w�g�O�_�I
	POSITION pos = NULL;
	INT3_BP bp = { 0 };
	while (pos != NULL)
	{
		bp = m_Int3BpList.GetNext(pos);
		if (bp.dwAddress == dwBpAddress)
		{
			AfxMessageBox("���a�}�w�g�]�m�_�I,�]�m�L��");
			return;
		}
	}
	memset(&bp, 0, sizeof(INT3_BP));
	bp.dwAddress = dwBpAddress;
	bp.isForever = TRUE;

	DWORD dwOldProtect = 0;
	DWORD dwRet = 0;
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//Ū�����r�`
	if (!ReadProcessMemory(hProcess, (LPVOID)dwBpAddress, &bp.bOriginalCode, sizeof(BYTE), NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 1927��X��");
		DWORD dwErrcode = 0;
		dwErrcode = GetLastError();
		//�V�Τ��X���~�H��
		GetErrorMessage(dwErrcode);

		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, dwOldProtect, &dwRet);
		return;

	}


	//�g�J0xCC
	if (!WriteProcessMemory(hProcess, (LPVOID)dwBpAddress, &bCCode, sizeof(bCCode), NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 1942��X��");
		DWORD dwErrcode = 0;
		dwErrcode = GetLastError();
		//�V�Τ��X���~�H��
		GetErrorMessage(dwErrcode);
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, dwOldProtect, &dwRet);
		return;
	}
	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBpAddress, 4, dwOldProtect, &dwRet);
	//��s
	SetDlgItemText(IDC_STATIC1, "�]�m�_�I���\");
	FlushInstructionCache(hProcess, (LPCVOID)dwBpAddress, sizeof(BYTE));
	m_Int3BpList.AddTail(bp);


}

//�R���w���_�I
void CLeg_DebugDlg::DeleteHardBP(LONGLONG dwAddress)
{

	if (dwAddress == 0)
	{
		AfxMessageBox("�S��J�_�I�a�}");
		return;
	}
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	if (!GetThreadContext(m_tpInfo.hThread, &ct))
	{
		OutputDebugString("EasyDbgDlg.cpp 2583��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//��X���~�H��
		GetErrorMessage(dwErrorCode);
		return;
	}
	DR7 tagDr7 = { 0 };
	tagDr7.dwDr7 = ct.Dr7;
	//�������_�I���ոձH�s��
	int nIndex = GetDeletedDrIndex(dwAddress, ct);
	if (nIndex == -1)
	{
		AfxMessageBox("�_�I�L��");
		return;
	}
	//�M0�}�]�m�����ЧӦ쬰FALSE
	switch (nIndex)
	{
	case 0:
		//�a�}
		ct.Dr0 = 0;
		//�ݩ�
		tagDr7.DRFlag.rw0 = 0;
		//�����_�I
		tagDr7.DRFlag.L0 = 0;
		//����
		tagDr7.DRFlag.len0 = 0;

		m_Dr_Use.Dr0 = FALSE;
		break;
	case 1:

		ct.Dr1 = 0;

		tagDr7.DRFlag.rw1 = 0;

		tagDr7.DRFlag.L1 = 0;

		tagDr7.DRFlag.len1 = 0;

		m_Dr_Use.Dr1 = FALSE;
		break;
	case 2:

		ct.Dr2 = 0;

		tagDr7.DRFlag.rw2 = 0;

		tagDr7.DRFlag.L2 = 0;

		tagDr7.DRFlag.len2 = 0;

		m_Dr_Use.Dr2 = FALSE;
		break;
	case 3:

		ct.Dr3 = 0;

		tagDr7.DRFlag.rw3 = 0;

		tagDr7.DRFlag.L3 = 0;

		tagDr7.DRFlag.len3 = 0;

		m_Dr_Use.Dr3 = FALSE;
		break;
	}
	//���
	ct.Dr7 = tagDr7.dwDr7;

	if (!SetThreadContext(m_tpInfo.hThread, &ct))
	{
		OutputDebugString("EasyDbgDlg.cpp 2656��X��");

		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//��X���~�H��
		GetErrorMessage(dwErrorCode);

		return;

	}
	SetDlgItemText(IDC_STATIC1, TEXT("�R���w���_�I���\"));


}


//��o�n�Q�R�����w���_�I���ոձH�s���s�� ��^-1��ܨS���
int CLeg_DebugDlg::GetDeletedDrIndex(LONGLONG dwAddress, CONTEXT ct)
{
	if (dwAddress == ct.Dr0)
	{
		return 0;
	}
	if (dwAddress == ct.Dr1)
	{
		return 1;
	}
	if (dwAddress == ct.Dr2)
	{
		return 2;
	}
	if (dwAddress == ct.Dr3)
	{
		return 3;
	}

	return -1;

}

//�T�|�_�I
void CLeg_DebugDlg::ListBP()
{

	POSITION pos = NULL;
	pos = m_Int3BpList.GetHeadPosition();
	INT3_BP bp = { 0 };
	CString szText;
	if (m_Int3BpList.GetCount() != 0)
	{
		//�C�|INT3�_�I
		while (pos != NULL)
		{
			bp = m_Int3BpList.GetNext(pos);
			szText.Format("INT3�_�I �_�I�a�}:%08X   �_�I�B��ƾ�:%2X �O�_�O�ä[�_�I: %d", bp.dwAddress, bp.bOriginalCode, bp.isForever);
			m_Result.AddString(szText);
			m_Result.SetTopIndex(m_Result.GetCount() - 1);

		}
	}
	else
	{
		szText.Format("��e�LINT3�_�I");
		m_Result.AddString(szText);
		m_Result.SetTopIndex(m_Result.GetCount() - 1);


	}


	//�C�|�w���_�I
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	if (!GetThreadContext(m_tpInfo.hThread, &ct))
	{
		OutputDebugString("EasyDbgDlg.cpp 2251��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//��X���~�H��
		GetErrorMessage(dwErrorCode);
		return;
	}

	DR7 tagDr7 = { 0 };
	tagDr7.dwDr7 = ct.Dr7;


	if (m_Dr_Use.Dr0)
	{
		szText.Format("�w���_�I �_�I�a�}:%08X �_�I����:%d  �_�I����:%d", ct.Dr0, tagDr7.DRFlag.rw0, tagDr7.DRFlag.len0 + 1);
		m_Result.AddString(szText);
		m_Result.SetTopIndex(m_Result.GetCount() - 1);

	}
	if (m_Dr_Use.Dr1)
	{
		szText.Format("�w���_�I �_�I�a�}:%08X �_�I����:%d  �_�I����:%d", ct.Dr1, tagDr7.DRFlag.rw1, tagDr7.DRFlag.len1 + 1);
		m_Result.AddString(szText);
		m_Result.SetTopIndex(m_Result.GetCount() - 1);

	}
	if (m_Dr_Use.Dr2)
	{
		szText.Format("�w���_�I �_�I�a�}:%08X �_�I����:%d  �_�I����:%d", ct.Dr2, tagDr7.DRFlag.rw2, tagDr7.DRFlag.len2 + 1);
		m_Result.AddString(szText);
		m_Result.SetTopIndex(m_Result.GetCount() - 1);

	}
	if (m_Dr_Use.Dr3)
	{
		szText.Format("�w���_�I �_�I�a�}:%08X �_�I����:%d  �_�I����:%d", ct.Dr3, tagDr7.DRFlag.rw3, tagDr7.DRFlag.len3 + 1);
		m_Result.AddString(szText);
		m_Result.SetTopIndex(m_Result.GetCount() - 1);

	}

	//�C�|���s�_�I
	pos = NULL;
	pos = m_MemBpList.GetHeadPosition();
	MEM_BP mBP = { 0 };
	if (m_MemBpList.GetCount() != 0)
	{
		while (pos != NULL)
		{
			mBP = m_MemBpList.GetNext(pos);
			switch (mBP.dwNumPage)
			{
			case 1:
			{
					  szText.Format("���s�_�I �_�I�a�}:%08X �_�I���� %d �_�I����:%d  �_�I�Ҹ����:%08X",
						  mBP.dwBpAddress, mBP.dwAttribute, mBP.dwLength, mBP.dwMemPage[0]);
					  m_Result.AddString(szText);
					  m_Result.SetTopIndex(m_Result.GetCount() - 1);
					  break;
			}
			case 2:
			{
					  szText.Format("���s�_�I �_�I�a�}:%08X �_�I���� %d �_�I����:%d  �_�I�Ҹ����:%08X %08X",
						  mBP.dwBpAddress, mBP.dwAttribute, mBP.dwLength, mBP.dwMemPage[0], mBP.dwMemPage[1]);
					  m_Result.AddString(szText);
					  m_Result.SetTopIndex(m_Result.GetCount() - 1);
					  break;
			}
			case 3:
			{
					  szText.Format("���s�_�I �_�I�a�}:%08X �_�I���� %d �_�I����:%d  �_�I�Ҹ����:%08X %08X %08X",
						  mBP.dwBpAddress, mBP.dwAttribute, mBP.dwLength, mBP.dwMemPage[0], mBP.dwMemPage[1],
						  mBP.dwMemPage[2]);
					  m_Result.AddString(szText);
					  m_Result.SetTopIndex(m_Result.GetCount() - 1);


					  break;
			}
			case 4:
			{
					  szText.Format("���s�_�I �_�I�a�}:%08X �_�I���� %d �_�I����:%d  �_�I�Ҹ����:%08X %08X %08X %08X",
						  mBP.dwBpAddress, mBP.dwAttribute, mBP.dwLength, mBP.dwMemPage[0], mBP.dwMemPage[1],
						  mBP.dwMemPage[2], mBP.dwMemPage[3]);
					  m_Result.AddString(szText);
					  m_Result.SetTopIndex(m_Result.GetCount() - 1);
					  break;
			}
			case 5:
			{
					  szText.Format("���s�_�I �_�I�a�}:%08X �_�I���� %d �_�I����:%d  �_�I�Ҹ����:%08X %08X %08X %08X %08X",
						  mBP.dwBpAddress, mBP.dwAttribute, mBP.dwLength, mBP.dwMemPage[0], mBP.dwMemPage[1],
						  mBP.dwMemPage[2], mBP.dwMemPage[3], mBP.dwMemPage[4]);
					  m_Result.AddString(szText);
					  m_Result.SetTopIndex(m_Result.GetCount() - 1);

					  break;
			}
			}



		}

	}
	else
	{
		szText.Format("��e�L���s�_�I");
		m_Result.AddString(szText);
		m_Result.SetTopIndex(m_Result.GetCount() - 1);

	}






}

//�R�����s�_�I
void CLeg_DebugDlg::DeleteMemBP(LONGLONG dwBpAddress)
{
	MEM_BP mBP = { 0 };
	//��줺�s�_�I�}�q�������
	if (!FindMemBPInformation(mBP, dwBpAddress))
	{
		AfxMessageBox("���a�}���O�_�I");
		return;
	}

	for (DWORD i = 0; i<mBP.dwNumPage; i++)
	{
		//���P�_���S���t�@�Ӥ��s�_�I�b�o�Ӥ����W,�p�G�s�b�N�קאּ�t�@���_�I�ҭn�D���ݩ�
		if (!ModifyPageProtect(mBP.dwMemPage[i]))
		{
			//�p�G�S����L���s�_�I�b�����s���W�N�����M�����s����קאּ��Ӫ��ݩ�
			MEMORY_BASIC_INFORMATION mbi = { 0 };
			mbi.BaseAddress = (PVOID)mBP.dwMemPage[i];
			//�p�G���\��^���ݩ�
			if (FindMemOriginalProtect(mbi))
			{
				DWORD dwOldProtect = 0;
				//�ק�
				if (!VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)mBP.dwMemPage[i], 4, mbi.Protect, &dwOldProtect))
				{
					OutputDebugString("EasyDbgDlg.cpp 3299��X��");
					DWORD dwErrorCode = 0;
					dwErrorCode = GetLastError();
					//��X���~�H��
					GetErrorMessage(dwErrorCode);
				}


			}

		}


	}


}

//���ŦX�����s�_�I�H���}��^ �Ѽ��������ޥ� �}�q����R��������
BOOL CLeg_DebugDlg::FindMemBPInformation(MEM_BP& mBP, LONGLONG dwBpAddress)
{

	POSITION pos = NULL;
	pos = m_MemBpList.GetHeadPosition();
	while (pos != NULL)
	{

		mBP = m_MemBpList.GetNext(pos);
		//�p�G����^TRUE
		if (mBP.dwBpAddress == dwBpAddress)
		{

			if (m_MemBpList.GetCount() == 1)
			{
				m_MemBpList.RemoveHead();

				SetDlgItemText(IDC_STATIC1, "���s�_�I�R�����\");
				return TRUE;

			}

			if (pos == NULL)
			{
				m_MemBpList.RemoveTail();

				SetDlgItemText(IDC_STATIC1, "���s�_�I�R�����\");
				return TRUE;

			}

			m_MemBpList.GetPrev(pos);

			m_MemBpList.RemoveAt(pos);
			SetDlgItemText(IDC_STATIC1, "���s�_�I�R�����\");

			return TRUE;
		}

	}
	return FALSE;

}



//���P�_���S���t�@�Ӥ��s�_�I�b�o�Ӥ����W,�p�G�s�b�N�קאּ�t�@���_�I�ҭn�D���ݩ�
//�Ѽ� ���s�������a�}
BOOL CLeg_DebugDlg::ModifyPageProtect(LONGLONG dwBaseAddress)
{
	POSITION pos = NULL;
	pos = m_MemBpList.GetHeadPosition();

	MEM_BP mBP = { 0 };
	BOOL isFind = FALSE;
	//�M��
	while (pos != NULL)
	{
		mBP = m_MemBpList.GetNext(pos);
		for (DWORD i = 0; i<mBP.dwNumPage; i++)
		{
			//�p�G��줺�s�_�I�٦b�����s�����W
			if (mBP.dwMemPage[i] == dwBaseAddress)
			{
				isFind = TRUE;
				DWORD dwOldProtect = 0;
				//�N�⤺�s�O�@�ݩʫ�_���L���һݭn��
				if (!VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwBaseAddress, 4,
					m_Attribute[mBP.dwAttribute], &dwOldProtect))
				{
					OutputDebugString("EasyDbgDlg.cpp 3387��X��");
					DWORD dwErrorCode = 0;
					dwErrorCode = GetLastError();

					GetErrorMessage(dwErrorCode);
				}
			}
		}


	}

	return isFind;
}


//����l���s����������ݩʦ}�ǥX �ޥ�����
BOOL CLeg_DebugDlg::FindMemOriginalProtect(MEMORY_BASIC_INFORMATION& mbi)
{

	POSITION pos;
	pos = m_MemPageList.GetHeadPosition();
	while (pos != NULL)
	{
		MEM_BP_PAGE mBPage = { 0 };
		mBPage = m_MemPageList.GetNext(pos);
		//�p�G����^TRUE
		if (mBPage.dwBaseAddress == (LONGLONG)mbi.BaseAddress)
		{
			//��ȭ�l�ݩ�
			mbi.Protect = mBPage.dwProtect;
			return TRUE;

		}
	}
	return FALSE;


}

//�]�m���s�_�I  dwAttribute 1��ܼg�J�_�I 3��ܳX���_�I
void CLeg_DebugDlg::SetMemBP(LONGLONG dwBpAddress, LONGLONG dwAttribute, LONGLONG dwLength)
{
	if (dwAttribute != 1 && dwAttribute != 3)
	{
		AfxMessageBox("���s�_�I�����˿�");
		return;
	}

	MEMORY_BASIC_INFORMATION mbi = { 0 };
	MEM_BP mbp = { 0 };

	if (!IsAddressValid(dwBpAddress, mbi))
	{
		AfxMessageBox("�_�I�a�}�L��");
		return;
	}
	//�P�_�a�}�M���ץe�F�X�Ӥ����}�[�J���s������ �]���_�I�[�J�_�I��
	if (!AddMemBpPage(dwBpAddress, dwLength, mbi, dwAttribute, mbp))
	{
		AfxMessageBox("�_�I�K�[����");
		return;
	}
	//�ӫO�@�ݩ�
	for (DWORD i = 0; i<mbp.dwNumPage; i++)
	{
		DWORD dwOldProtect = 0;

		if (!VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)mbp.dwMemPage[i], 4, m_Attribute[dwAttribute], &dwOldProtect))
		{
			OutputDebugString("EasyDbgDlg.cpp 2944��X��");
			DWORD dwErrorCode = 0;
			dwErrorCode = GetLastError();
			//��X���~�H��
			GetErrorMessage(dwErrorCode);
			AfxMessageBox("�_�I�[�J����");
			return;

		}


	}

	SetDlgItemText(IDC_STATIC1, "���s�_�I�]�m���\");

}

//�P�_�a�}�O�_����
BOOL CLeg_DebugDlg::IsAddressValid(LONGLONG dwAddress, MEMORY_BASIC_INFORMATION& mbi)
{


	DWORD dwRet = 0;

	dwRet = VirtualQueryEx(m_tpInfo.hProcess, (LPVOID)dwAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	//��^�ȻP��w�R�Ϫ��פ��ۦP�h��ܦa�}�L��
	if (dwRet != sizeof(MEMORY_BASIC_INFORMATION))
	{
		return FALSE;
	}
	//MEM_FREE ������X�� MEM_RESERVE���O�@�ݩʥ���
	if (mbi.State == MEM_COMMIT)
	{
		return TRUE;
	}

	return FALSE;
}


//�P�_�a�}�M���ץe�F�X�Ӥ����}�[�J���s������ �}���_�I�]�[�J�_�I���
BOOL CLeg_DebugDlg::AddMemBpPage(LONGLONG dwBpAddress, LONGLONG dwLength, MEMORY_BASIC_INFORMATION mbi, LONGLONG dwAttribute, MEM_BP& mbp)
{
	//�p�G�b�@�Ӥ�����(�������)
	MEM_BP_PAGE mBPage = { 0 };


	if (dwBpAddress >= (LONGLONG)mbi.BaseAddress && (LONGLONG)mbi.BaseAddress + mbi.RegionSize >= dwBpAddress + dwLength)
	{
		mBPage.dwBaseAddress = (LONGLONG)mbi.BaseAddress;
		mBPage.dwProtect = mbi.Protect;
		//�b���s����S���N�K�[
		if (!FindMemPage((LONGLONG)mbi.BaseAddress))
		{
			m_MemPageList.AddTail(mBPage);
		}
		//�K�[���s���
		mbp.dwAttribute = dwAttribute;
		mbp.dwBpAddress = dwBpAddress;
		mbp.dwLength = dwLength;
		mbp.dwMemPage[0] = mBPage.dwBaseAddress;
		mbp.dwNumPage = 1;
		//�d�ݸӦa�}�B�O�_�w�g�����s�_�I�p�G������b�U�_�I
		if (FindMemBP(dwBpAddress))
		{
			AfxMessageBox("�Ӧa�}�w�g�����s�_�I,����b�U�_�I");
			return FALSE;
		}
		else
		{
			//�K�[���_�I���
			m_MemBpList.AddTail(mbp);
		}
		return TRUE;


	}
	//��h�Ӥ��������p �]����Ӧh���ݤ_���ݦ欰,�]���N���إߩҦ������s�����F
	//������� ����3�ӭ����N�ݤ_���ݦ欰....
	int i = 0;

	mbp.dwAttribute = dwAttribute;
	mbp.dwBpAddress = dwBpAddress;
	mbp.dwLength = dwLength;

	while ((LONGLONG)mbi.BaseAddress + mbi.RegionSize<dwBpAddress + dwLength)
	{
		if (i>4)
		{
			AfxMessageBox("�ڹ�A�L�y�U�o�\�h���������s�_�I");
			return FALSE;
		}
		mBPage.dwBaseAddress = (LONGLONG)mbi.BaseAddress;
		mBPage.dwProtect = mbi.Protect;
		//�b���s����S���N�K�[
		if (!FindMemPage((LONGLONG)mbi.BaseAddress))
		{
			m_MemPageList.AddTail(mBPage);
		}
		mbp.dwMemPage[i] = mBPage.dwBaseAddress;

		DWORD dwRet = 0;

		//��U�@�Ӥ���
		dwRet = VirtualQueryEx(m_tpInfo.hProcess, (LPVOID)((LONGLONG)mbi.BaseAddress + mbi.RegionSize), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		//��^�ȻP��w�R�Ϫ��פ��ۦP�h��ܦa�}�L��
		if (dwRet != sizeof(MEMORY_BASIC_INFORMATION))
		{
			return FALSE;
		}

		i++;


	}

	if (i>4)
	{
		AfxMessageBox("�ڹ�A�L�y�U�o�\�h���������s�_�I");
		return FALSE;
	}

	mBPage.dwBaseAddress = (LONGLONG)mbi.BaseAddress;
	mBPage.dwProtect = mbi.Protect;
	//�b���s����S���N�K�[
	if (!FindMemPage((LONGLONG)mbi.BaseAddress))
	{
		m_MemPageList.AddTail(mBPage);
	}
	mbp.dwMemPage[i] = mBPage.dwBaseAddress;

	if (FindMemBP(dwBpAddress))
	{
		AfxMessageBox("�Ӧa�}�w�g�����s�_�I,����b�U�_�I");
		return FALSE;
	}
	else
	{
		//�K�[���_�I���
		m_MemBpList.AddTail(mbp);
	}
	return TRUE;

}


//�P�_�Y�@�����a�}�O�_�s�b�_�����
BOOL CLeg_DebugDlg::FindMemPage(LONGLONG dwBaseAddress)
{

	POSITION pos;
	pos = m_MemPageList.GetHeadPosition();
	while (pos != NULL)
	{
		MEM_BP_PAGE mBPage = { 0 };
		mBPage = m_MemPageList.GetNext(pos);
		//�p�G����^TRUE
		if (mBPage.dwBaseAddress == dwBaseAddress)
		{
			return TRUE;

		}
	}
	return FALSE;
}

//�P�_�a�}�O�_���_�U���s�_�I
BOOL CLeg_DebugDlg::FindMemBP(LONGLONG dwBpAddress)
{
	POSITION pos = NULL;
	pos = m_MemBpList.GetHeadPosition();
	while (pos != NULL)
	{
		MEM_BP memBp = { 0 };
		memBp = m_MemBpList.GetNext(pos);
		//�p�G����^TRUE
		if (memBp.dwBpAddress == dwBpAddress)
		{
			return TRUE;
		}

	}
	return FALSE;
}

//�]�m�w���_�I �Ѽ� �a�} �ݩ� ����
//dwAttribute 0��ܰ����_�I 3��ܳX���_�I 1 ��ܼg�J�_�I
//dwLength ���� 1 2 4
void CLeg_DebugDlg::SetHardBP(LONGLONG dwBpAddress, LONGLONG dwAttribute, LONGLONG dwLength)
{


	if (dwLength != 1 && dwLength != 2 && dwLength != 4)
	{
		AfxMessageBox("�_�I���׳]�m���~");
		return;
	}
	//�j�������_�I�����קאּ1
	if (dwAttribute == 0)
	{
		dwLength = 1;
	}

	int nIndex = 0;
	//��o��e�Ŷ~�ձH�s���s��
	nIndex = FindFreeDebugRegister();

	if (nIndex == -1)
	{
		AfxMessageBox("��e�w���_�I�w��,�ЧR���b�]�m");
		return;
	}
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	if (!GetThreadContext(m_tpInfo.hThread, &ct))
	{
		OutputDebugString("EasyDbgDlg.cpp 2460��X��");

		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		//��X���~�H��
		GetErrorMessage(dwErrorCode);
		return;

	}
	//��ȧڭ̩w�q��DR7���c��,�o�ˬ٥h�F�첾�ާ@���c��
	DR7 tagDr7 = { 0 };
	tagDr7.dwDr7 = ct.Dr7;

	switch (nIndex)
	{
	case 0:
		//���_�a�}
		ct.Dr0 = dwBpAddress;
		//�_�I����
		tagDr7.DRFlag.len0 = dwLength - 1;
		//�ݩ�
		tagDr7.DRFlag.rw0 = dwAttribute;
		//�����_�I
		tagDr7.DRFlag.L0 = 1;
		//�]�m�ЧӦ�O���ոձH�s�����ϥα��p
		m_Dr_Use.Dr0 = TRUE;

		break;
	case 1:
		ct.Dr1 = dwBpAddress;

		tagDr7.DRFlag.len1 = dwLength - 1;

		tagDr7.DRFlag.rw1 = dwAttribute;

		tagDr7.DRFlag.L1 = 1;

		m_Dr_Use.Dr1 = TRUE;


		break;
	case 2:
		ct.Dr2 = dwBpAddress;

		tagDr7.DRFlag.len2 = dwLength - 1;

		tagDr7.DRFlag.rw2 = dwAttribute;

		tagDr7.DRFlag.L2 = 1;

		m_Dr_Use.Dr2 = TRUE;

		break;
	case 3:
		ct.Dr3 = dwBpAddress;

		tagDr7.DRFlag.len3 = dwLength - 1;

		tagDr7.DRFlag.rw3 = dwAttribute;

		tagDr7.DRFlag.L3 = 1;

		m_Dr_Use.Dr3 = TRUE;
		break;
	}

	//��Ȧ^�h
	ct.Dr7 = tagDr7.dwDr7;
	if (!SetThreadContext(m_tpInfo.hThread, &ct))
	{
		OutputDebugString("EasyDbgDlg.cpp 2531��X��");
		DWORD dwErrorCode = 0;

		dwErrorCode = GetLastError();
		//��X���~�H��
		GetErrorMessage(dwErrorCode);
		return;
	}
	SetDlgItemText(IDC_STATIC1, "�]�m�w���_�I���\");


}

//��^��e�i�Ϊ��ոձH�s��
int CLeg_DebugDlg::FindFreeDebugRegister()
{
	if (!m_Dr_Use.Dr0)
	{
		return 0;

	}
	if (!m_Dr_Use.Dr1)
	{
		return 1;
	}
	if (!m_Dr_Use.Dr2)
	{
		return 2;

	}
	if (!m_Dr_Use.Dr3)
	{
		return 3;
	}
	//�p�GDr0-Dr3���Q�ϥΫh��^-1
	return -1;
}

//G�R�O�B�z
void CLeg_DebugDlg::ON_G_COMMAND(LONGLONG dwAddress)
{
	//�p�G�����w�a�}�q�{�MF9�B�z�@��
	if (dwAddress == 0)
	{
		m_Result.ResetContent();
		ON_VK_F9();
		return;
	}
	INT3_BP bp = { 0 };
	bp.dwAddress = dwAddress;

	DWORD dwOldProtect = 0;
	DWORD dwRet = 0;

	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);


	if (!ReadProcessMemory(m_tpInfo.hProcess, (LPVOID)dwAddress, &bp.bOriginalCode, sizeof(BYTE), NULL))
	{
		OutputDebugString("EasyDbgDlg.cpp 2392��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//�V�Τ��X���~�H��
		GetErrorMessage(dwErrorCode);
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
		return;
	}
	//�D�ä[�_�I
	bp.isForever = FALSE;
	//�g�J0XCC
	if (!WriteProcessMemory(m_tpInfo.hProcess, (LPVOID)dwAddress, &m_tpInfo.bCC, sizeof(BYTE), NULL))
	{

		OutputDebugString("EasyDbgDlg.cpp 2405��X��");
		DWORD dwErrorCode = 0;
		dwErrorCode = GetLastError();
		//�V�Τ��X���~�H��
		GetErrorMessage(dwErrorCode);
		VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
		return;

	}

	VirtualProtectEx(m_tpInfo.hProcess, (LPVOID)dwAddress, 4, dwOldProtect, &dwRet);
	//�[�J�_�I���
	m_Int3BpList.AddTail(bp);

	m_IsGo = TRUE;
	//�B��
	ON_VK_F9();



}


void CLeg_DebugDlg::OnEnChangeEdit2()
{
	// TODO:  �p�G�o�O RICHEDIT ����A����N���|
	// �ǰe���i���A���D�z�мg CDialogEx::OnInitDialog()
	// �禡�M�I�s CRichEditCtrl().SetEventMask()
	// ���㦳 ENM_CHANGE �X�� ORed �[�J�B�n�C

	// TODO:  �b���[�J����i���B�z�`���{���X
}
