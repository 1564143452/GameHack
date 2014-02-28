#pragma once

// �����ܵ��ӿ���
typedef enum PIPETYPE
{
    PIPE_SERVER,
    PIPE_CLIENT
} PIPETYPE;

typedef struct PIPECLIENTINFO
{
public:
    PIPECLIENTINFO(LPCTSTR lpszAppPath, LPCTSTR lpszCmdLine, LPSECURITY_ATTRIBUTES pSec)
    {
        ZeroMemory(m_sAppPath, MAX_PATH);
        ZeroMemory(m_sCmdLine, MAX_PATH);

        if(NULL != lpszAppPath)
            _tcscpy_s(m_sAppPath, lpszAppPath);

        if(NULL != lpszCmdLine)
            _tcscpy_s(m_sCmdLine, lpszCmdLine);

        m_lpSecurityDescriptor = pSec;
    }
    // �ӽ��̾���·��
    TCHAR m_sAppPath[MAX_PATH];
    // ���ݸ��ӽ��̵�������
    TCHAR m_sCmdLine[MAX_PATH];
    //
    LPVOID m_lpSecurityDescriptor;
} PIPECLIENTINFO, *LPPIPECLIENTINFO;

class IAnonymousPipe
{
public:
    virtual ~IAnonymousPipe(void) = 0 {};
    virtual BOOL Create() = 0;
    virtual BOOL Read(LPVOID lpBuf, DWORD dwBufSize, LPDWORD dwReaded) = 0;
    virtual BOOL Write(LPCVOID lpBuf, DWORD dwBufSize, LPDWORD dwWrited) = 0;
    virtual void Close() = 0;
};