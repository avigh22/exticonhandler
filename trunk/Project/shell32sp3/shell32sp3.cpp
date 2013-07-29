// shell32sp3.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "shell32sp3.h"
#include <sstream>
#include <iostream>
#include "xstring"
#include "Windows.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// ���ǵ���������һ��ʾ��
SHELL32SP3_API int nshell32sp3=0;

// ���ǵ���������һ��ʾ����
SHELL32SP3_API int fnshell32sp3(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� shell32sp3.h
Cshell32sp3::Cshell32sp3()
{ 
	return; 
}

std::string& GetRawString(std::string& str  )
{
	//std::cout<<" SecretString : "<<str.c_str()<<"\n";

 	std::string::iterator iter = str.begin();
	for ( ; iter != str.end(); iter++)
	{
		*iter = (( char)*iter + 10);
	}
	//std::cout<<"Get RawString : "<<str.c_str()<<"\n";

	return str;
}
std::string& GetSecretString(std::string& str )
{
	//std::cout<<" SecretString : "<<str.c_str()<<"\n";
 	std::string::iterator iter = str.begin();
	for ( ; iter != str.end(); iter++)
	{
		*iter = (( char)*iter - 10);
	}
	//std::cout<<"Get RawString : "<<str.c_str()<<"\n";
	return str;
}
  
#pragma comment(linker, "/EXPORT:Control_RunDLL=__si0@16,PRIVATE") 
extern "C"  void CALLBACK _si0(	HWND hwnd,	HINSTANCE hinst,	LPTSTR lpCmdLine,	int nCmdShow)
{
	AllocConsole();  	 
	freopen("CONIN$", "r+t", stdin); // �ض��� STDIN
	freopen("CONOUT$", "w+t", stdout); // �ض���STDOUT
    setlocale(0,"chs");

	
	 TCHAR szUser[256] = {0};
		std::string strUserKey = "-user";
	 GetEnvironmentVariable(GetSecretString(strUserKey).c_str() , szUser, 256);
	 std::string strUser = szUser;
	 GetRawString(strUser);

	 TCHAR szWorkTime[256] = {0};
	 GetEnvironmentVariable("worktime" , szWorkTime, 256);

	int i = atoi(szWorkTime);
	 while (i>0){
		 std::cout<<" -user="<<strUser<<" , count="<<i--<<"\n";
		 Sleep(1000);
	 }

}