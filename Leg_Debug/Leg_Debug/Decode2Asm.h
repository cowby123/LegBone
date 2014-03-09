// Decode2Asm.h: interface for the CDecode2Asm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECODE2ASM_H__7AE15245_B351_41F6_B8B2_09D157829988__INCLUDED_)
#define AFX_DECODE2ASM_H__7AE15245_B351_41F6_B8B2_09D157829988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*// �L�����X�ѪR
extern "C"
void
__stdcall
Decode2Asm(IN PBYTE pCodeEntry,   // �ݭn�ѪR���O�a�}
           OUT char* strAsmCode,  // �o��϶׽s���O�H��
           OUT UINT* pnCodeSize,  // �ѪR���O����
           LONGLONG nAddress);  
		   */
// �a�����X�ѪR 
void
__stdcall
Decode2AsmOpcode(IN PBYTE pCodeEntry,   // �ݭn�ѪR���O�a�}
OUT char* strAsmCode,        // �o��϶׽s���O�H��
OUT char* strOpcode,         // �ѪR�����X�H��
OUT UINT* pnCodeSize,        // �ѪR���O����
LONGLONG nAddress);

#endif // !defined(AFX_DECODE2ASM_H__7AE15245_B351_41F6_B8B2_09D157829988__INCLUDED_)
