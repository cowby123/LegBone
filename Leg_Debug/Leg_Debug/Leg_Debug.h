
// Leg_Debug.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CLeg_DebugApp: 
// �аѾ\��@�����O�� Leg_Debug.cpp
//

class CLeg_DebugApp : public CWinApp
{
public:
	CLeg_DebugApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CLeg_DebugApp theApp;