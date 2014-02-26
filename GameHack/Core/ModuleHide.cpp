#include "stdafx.h"
#include "ModuleHide.h"

extern "C" DWORD __stdcall GetLMHNode();
extern "C" DWORD __stdcall GetLDMNode();

CModuleHide::CModuleHide()
{
}

CModuleHide::~CModuleHide()
{
}

BOOL CModuleHide::LDRHide(HMODULE hModule)
{
    if(!RemovePESignature(hModule))
        return FALSE;

    return RemoveModuleFromLDR(hModule);
}

BOOL CModuleHide::RemovePESignature(HMODULE hModule)
{
    if(NULL == hModule)
        return FALSE;

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    DWORD dwOldProtect = 0;

    if(!VirtualProtect(pDosHeader, 1024, PAGE_READWRITE, &dwOldProtect))
        return FALSE;

    pDosHeader->e_magic = 0;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pDosHeader + 1);
    pNtHeader->Signature = 0;

    if(!VirtualProtect(pDosHeader, 1024, dwOldProtect, &dwOldProtect))
        return FALSE;

    return TRUE;
}

BOOL CModuleHide::RemoveModuleFromLDR(HMODULE hModule)
{
    if(NULL == hModule)
        return FALSE;

    PLDR_MODULE pLMFNode = NULL, pLNode = NULL ;
    PLDR_MODULE pLMHNode = NULL, pLMPNode = NULL;
    PLDR_MODULE pLMTNode = NULL;
    BOOL bSuccess = FALSE;
    //��ȡLDR_MODULE����ͷָ��
    pLMHNode = (PLDR_MODULE)GetLMHNode();
    pLMFNode = (PLDR_MODULE)GetLDMNode();
    //����Ŀ��
    PLDR_MODULE pLMNode = pLMFNode;
    pLMPNode = pLMHNode;

    do
    {
        //�Ƚ��Ƿ���Ŀ��ģ��
        if((DWORD)pLMNode->BaseAddress == (DWORD)hModule)
        {
            bSuccess = TRUE;
            break;
        }

        pLMPNode = pLMNode;
        pLMNode = (PLDR_MODULE)pLMNode->InLoadOrderModuleList.Flink;
    }
    while(pLMNode != pLMHNode);

    if(!bSuccess)
    {
        return FALSE; //δ�ҵ�Ŀ��ģ��
    }

    //�Ͽ�InLoadOrderModuleList��
    //�ؽ�Flink
    pLMTNode = (PLDR_MODULE)pLMNode->InLoadOrderModuleList.Flink;
    pLMPNode->InLoadOrderModuleList.Flink = (PLIST_ENTRY)pLMTNode;
    //�ؽ�Blink
    ((PLDR_MODULE)(pLMNode->InLoadOrderModuleList.Flink))->InLoadOrderModuleList.Blink  =
        pLMNode->InLoadOrderModuleList.Blink;
    //�Ͽ�InMemoryOrderModuleList��
    //�ؽ�Flink
    pLMPNode->InMemoryOrderModuleList.Flink =
        pLMNode->InMemoryOrderModuleList.Flink;
    //�ؽ�Blink
    pLMTNode = (PLML)(pLMNode->InMemoryOrderModuleList.Flink - sizeof(LIST_ENTRY));
    pLMTNode->InMemoryOrderModuleList.Blink =
        pLMNode->InMemoryOrderModuleList.Blink;
    //�Ͽ�InInitializationOrderModuleList��
    //�ؽ�Flink
    pLMPNode->InInitializationOrderModuleList.Flink =
        pLMNode->InInitializationOrderModuleList.Flink;
    //�ؽ�Blink
    pLMTNode = (PLML)(pLMNode->InInitializationOrderModuleList.Flink - 2 * sizeof(LIST_ENTRY));
    pLMTNode->InInitializationOrderModuleList.Blink  = pLMNode->InInitializationOrderModuleList.Blink;
    return TRUE;
}

