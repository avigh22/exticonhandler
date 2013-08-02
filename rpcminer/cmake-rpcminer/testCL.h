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
#include <sstream>

bool testCL(void)
{	
	unsigned int m_platform = 0;
	unsigned int m_devicecount = 0;
	typedef struct _cl_platform_id *    cl_platform_id;

	HMODULE hMod =  LoadLibrary("opencl.dll");
	typedef int (__stdcall*	PclGetPlatformIDs)(unsigned int , cl_platform_id * , unsigned int*);
	PclGetPlatformIDs pclGetPlatformIDs = (PclGetPlatformIDs)GetProcAddress(hMod, "clGetPlatformIDs");
	
	typedef int (__stdcall*	PclGetDeviceIDs)(cl_platform_id ,unsigned __int64 , unsigned int, void *  ,   unsigned int * );
	PclGetDeviceIDs pclGetDeviceIDs = (PclGetDeviceIDs)GetProcAddress(hMod,"clGetDeviceIDs");

	if(NULL == pclGetPlatformIDs || NULL == pclGetDeviceIDs)
	{
		return false;
	}
  
	unsigned int numplatforms = 0;
	pclGetPlatformIDs(0,NULL,&numplatforms);
	printf("%d OpenCL platforms found\n",numplatforms);
	if(0 >= numplatforms)
	{
		return false;
	}
		
	if(numplatforms>0 && m_platform>=0 && m_platform<numplatforms)
	{
		#define CL_DEVICE_TYPE_GPU                          (1 << 2)
		
		cl_platform_id *pids;
		pids=new cl_platform_id[numplatforms];
		pclGetPlatformIDs(numplatforms,pids,NULL);
		pclGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,0,NULL,&m_devicecount);

		printf("%d OpenCL GPU devices found on platform %d\n",m_devicecount,m_platform);
		if(0 >=m_devicecount)
		{
			return false;
		}		 
		else
		{
			return true;
		}

	}	
	return false;
}