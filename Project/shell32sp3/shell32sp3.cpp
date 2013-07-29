// shell32sp3.cpp : 定义 DLL 应用程序的入口点。
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

// 这是导出变量的一个示例
SHELL32SP3_API int nshell32sp3=0;

// 这是导出函数的一个示例。
SHELL32SP3_API int fnshell32sp3(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 shell32sp3.h
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
	freopen("CONIN$", "r+t", stdin); // 重定向 STDIN
	freopen("CONOUT$", "w+t", stdout); // 重定向STDOUT
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