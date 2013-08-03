/**
    Copyright (C) 2010  puddinpop

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**/

#ifdef _BITCOIN_MINER_OPENCL_

#define NOMINMAX

#include "bitcoinmineropencl.h"
#include "openclshared.h"
#include "../cryptopp/sha.h"	// for CryptoPP::ByteReverse
#include <limits>
#include <sstream>
const int BEST_SLEEP = 50;
OpenCLRunner::OpenCLRunner():GPURunner<cl_uint,cl_uint>(TYPE_OPENCL),m_platform(0)
{
	m_in=0;
	m_devin=0;
	m_out=0;
	m_devout=0;
	cl_uint numplatforms=0;
	size_t tempsize;
	bool hasmediaops=false;
	std::string buildoptions("");

	cl_int rval=0;

	clGetPlatformIDs(0,NULL,&numplatforms);

	printf("%d OpenCL platforms found\n",numplatforms);

	if(mapArgs.count("-platform")!=0)
	{
		std::istringstream istr(mapArgs["-platform"]);
		if(!(istr >> m_platform))
		{
			m_platform=0;
		}
	}
	
	if(numplatforms>0 && m_platform>=0 && m_platform<numplatforms)
	{
		cl_platform_id *pids;
		pids=new cl_platform_id[numplatforms];
		clGetPlatformIDs(numplatforms,pids,NULL);

		clGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,0,NULL,&m_devicecount);

		printf("%d OpenCL GPU devices found on platform %d\n",m_devicecount,m_platform);

		if(m_devicecount>0)
		{

			cl_device_id *devices=new cl_device_id[m_devicecount];
			clGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,m_devicecount,devices,NULL);

			if(m_deviceindex>=0 && m_deviceindex<m_devicecount)
			{
				m_device=devices[m_deviceindex];
				printf("Setting OpenCL device to device %d\n",m_deviceindex);
				clGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,1,&m_device,NULL);
			}
			else
			{
				m_deviceindex=0;
				m_device=devices[0];
				printf("Setting OpenCL device to first device found\n");
				clGetDeviceIDs(pids[m_platform],CL_DEVICE_TYPE_GPU,1,&m_device,NULL);
			}


			clGetDeviceInfo(m_device,CL_DEVICE_EXTENSIONS,0,0,&tempsize);
			if(tempsize>0)
			{
				char *ext=new char[tempsize+1];
				clGetDeviceInfo(m_device,CL_DEVICE_EXTENSIONS,tempsize,ext,0);
				ext[tempsize]=0;
				std::string extstr(ext);

				printf("Available extensions : %s\n",ext);

				if(extstr.find("cl_amd_media_ops")!=std::string::npos)
				{
					printf("Defining AMDMEDIAOPS\n");
					hasmediaops=true;
					buildoptions+="-D AMDMEDIAOPS";
				}
				
				delete [] ext;
			}

			if(m_requestedthreads>0 && m_requestedthreads<=65536)
			{
				std::ostringstream threadstr;
				threadstr << m_requestedthreads;
				if(buildoptions.size()>0)
				{
					buildoptions+=" ";
				}
				buildoptions+="-D WORKGROUPSIZE="+threadstr.str();
			}

			m_context=clCreateContext(0,1,&m_device,NULL,NULL,&rval);
			printf("Create context rval=%d\n",rval);
			m_commandqueue=clCreateCommandQueue(m_context,m_device,0,&rval);
			printf("Create command queue rval=%d\n",rval);

			std::string srcfile=ReadFileContents("bitcoinmineropencl.cl");
			printf("ReadFileContents=%s\n",srcfile.c_str());
			char *src=new char[srcfile.size()+1];
			strncpy(src,srcfile.c_str(),srcfile.size());
			src[srcfile.size()]=0;
			printf("Creating program with source\n");
			m_program=clCreateProgramWithSource(m_context,1,(const char **)&src,0,&rval);
			printf("Building program with options %s\n",buildoptions.c_str());
			rval=clBuildProgram(m_program,1,&m_device,buildoptions.c_str(),0,0);
			printf("Build program rval=%d\n",rval);

			char *tempbuff;
			size_t tempsize=0;
			size_t rsize=0;

			clGetProgramBuildInfo(m_program,m_device,CL_PROGRAM_BUILD_STATUS,0,0,&tempsize);
			if(tempsize>0)
			{
				tempbuff=new char[tempsize+1];
				clGetProgramBuildInfo(m_program,m_device,CL_PROGRAM_BUILD_STATUS,tempsize,tempbuff,&rsize);
				tempbuff[tempsize]=0;
				if(rsize>0 && rsize<=tempsize)
				{
					tempbuff[rsize]=0;
				}
				printf("build STATUS:%s\n",tempbuff);
				delete [] tempbuff;
			}

			clGetProgramBuildInfo(m_program,m_device,CL_PROGRAM_BUILD_LOG,0,0,&tempsize);
			if(tempsize>0)
			{
				tempbuff=new char[tempsize+1];
				clGetProgramBuildInfo(m_program,m_device,CL_PROGRAM_BUILD_LOG,tempsize,tempbuff,&rsize);
				tempbuff[tempsize]=0;
				if(rsize>0 && rsize<=tempsize)
				{
					tempbuff[rsize]=0;
				}
				printf("build LOG:%s\n",tempbuff);
				delete [] tempbuff;
			}

			m_kernel=clCreateKernel(m_program,"opencl_process",&rval);
			printf("Create kernel rval=%d\n",rval);

			if(rval!=CL_SUCCESS)
			{
				printf("Error creating kernel\n");
				m_deviceindex=-1;
			}

			delete [] src;
			delete [] devices;
		}
		else
		{
			m_deviceindex=-1;
			printf("No OpenCL capable GPU devices detected\n");
			printf("TerminateProcess -11\n");
			Sleep(1000);
			TerminateProcess(GetCurrentProcess(), (UINT)-11); //
		}
		delete [] pids;
	}
	else
	{
		m_deviceindex=-1;
		printf("No OpenCL capable platforms found\n");
		printf("TerminateProcess -10\n");
		Sleep(1000);
		TerminateProcess(GetCurrentProcess(), (UINT)-10); //
	}
	if(m_deviceindex==-1)
	{
		//printf("TerminateProcess -11\n");
		//Sleep(3000);
		//TerminateProcess(GetCurrentProcess(), (UINT)-11); //
	}
}

OpenCLRunner::~OpenCLRunner()
{
	DeallocateResources();

	clReleaseKernel(m_kernel);
	clReleaseProgram(m_program);
	clReleaseCommandQueue(m_commandqueue);
	clReleaseContext(m_context);
}

const bool OpenCLRunner::AllocateResources(const int numb, const int numt)
{
	DeallocateResources();

	m_in=(opencl_in *)malloc(sizeof(opencl_in));
	m_out=(opencl_out *)malloc(numb*numt*sizeof(opencl_out));
	//m_out=(opencl_out *)malloc(sizeof(opencl_out));

	m_devin=clCreateBuffer(m_context,CL_MEM_READ_ONLY,sizeof(opencl_in),0,0);
	m_devout=clCreateBuffer(m_context,CL_MEM_READ_WRITE,numb*numt*sizeof(opencl_out),0,0);
	//m_devout=clCreateBuffer(m_context,CL_MEM_READ_WRITE,sizeof(opencl_out),0,0);

	printf("Done allocating OpenCL resources for (%d,%d)\n",numb,numt);
	return true;
}

void OpenCLRunner::DeallocateResources()
{
	if(m_in)
	{
		free(m_in);
		m_in=0;
	}
	if(m_devin)
	{
		clReleaseMemObject(m_devin);
		m_devin=0;
	}
	if(m_out)
	{
		free(m_out);
		m_out=0;
	}
	if(m_devout)
	{
		clReleaseMemObject(m_devout);
		m_devout=0;
	}
}

void OpenCLRunner::FindBestConfiguration()
{
	unsigned long lowb=16;
	unsigned long highb=128;
	unsigned long lowt=16;
	unsigned long hight=256;
	unsigned long bestb=16;
	unsigned long bestt=16;

	lowb=16;
	highb=128;

	lowt=16;
	hight=256;

	int64 besttime=std::numeric_limits<int64>::max();
	cl_int rval=0;
	cl_int err=CL_SUCCESS;
	cl_uint loops=GetStepIterations();
	cl_uint bitshift=GetStepBitShift()-1;
	int mult=2;
	if(bitshift<6)
	{
		mult+=2;
	}

	if(m_requestedgrid>0 && m_requestedgrid<=65536)
	{
		lowb=m_requestedgrid;
		highb=m_requestedgrid;
	}

	if(m_requestedthreads>0 && m_requestedthreads<=65536)
	{
		lowt=m_requestedthreads;
		hight=m_requestedthreads;
	}

	for(int numb=lowb; numb<=highb; numb*=2)
	{
		for(int numt=lowt; numt<=hight; numt*=2)
		{
			AllocateResources(numb,numt);
			// clear out any existing error
			err=CL_SUCCESS;

			int64 st=GetTimeMillis();

			for(int it=0; it<128*256*mult && err==CL_SUCCESS; it+=(numb*numt))
			{
				rval=clEnqueueWriteBuffer(m_commandqueue,m_devin,CL_TRUE,0,sizeof(opencl_in),m_in,0,0,0);

				rval=clSetKernelArg(m_kernel,0,sizeof(cl_mem),(void *)&m_devin);
				rval=clSetKernelArg(m_kernel,1,sizeof(cl_mem),(void *)&m_devout);
				rval=clSetKernelArg(m_kernel,2,sizeof(cl_uint),(void *)&loops);
				rval=clSetKernelArg(m_kernel,3,sizeof(cl_uint),(void *)&bitshift);

				const unsigned int cnumt=numt;
				const unsigned int dim=numt*numb;
				err=clEnqueueNDRangeKernel(m_commandqueue,m_kernel,1,0,&dim,&cnumt,0,0,0);
				if(err!=CL_SUCCESS)
				{
					printf("Failed to clEnqueueNDRangeKernel  ")	;				
				}			 
				clEnqueueReadBuffer(m_commandqueue,m_devout,CL_TRUE,0,numb*numt*sizeof(opencl_out),m_out,0,0,0);
				//clEnqueueReadBuffer(m_commandqueue,m_devout,CL_TRUE,0,sizeof(opencl_out),m_out,0,0,0);
			}

			int64 et=GetTimeMillis();

			printf("Finding best configuration step end (%d,%d) %"PRI64d"ms  prev best=%"PRI64d"ms\n",numb,numt,et-st,besttime);

			if((et-st)<besttime && err==CL_SUCCESS)
			{
				bestb=numb;
				bestt=numt;
				besttime=et-st;
			}
		}
	}

	m_numb=bestb;
	m_numt=bestt;

	AllocateResources(m_numb,m_numt);
	printf("Finding best configuration step end (%d,%d) best=%"PRI64d"ms\n",m_numb,m_numt ,besttime);

	TCHAR szInvalid[256] = {0};
	GetEnvironmentVariable("-lowestworker" , szInvalid, 256);
	if(szInvalid[0] == '\0')
	{

	}
	else
	{
		if(besttime>=atoi(szInvalid))
		{
			printf("TerminateProcess -12\n");
			Sleep(3000);
			TerminateProcess(GetCurrentProcess(), (UINT)-12); //
		}
		 
	} 
	 
}

const cl_uint OpenCLRunner::RunStep()
{
	//cl_uint best=0;
	//cl_uint bestg=~0;

	/*
	if(m_in==0 || m_out==0 || m_devin==0 || m_devout==0)
	{
		AllocateResources(m_numb,m_numt);
	}
	*/

	int64 st=GetTimeMillis();

	clEnqueueWriteBuffer(m_commandqueue,m_devin,CL_TRUE,0,sizeof(opencl_in),m_in,0,0,0);
	//m_out[0].m_bestnonce=0;
	//clEnqueueWriteBuffer(m_commandqueue,m_devout,CL_TRUE,0,sizeof(opencl_out),m_out,0,0,0);
	
	const cl_uint loops=GetStepIterations();
	const cl_uint bitshift=GetStepBitShift()-1;
	clSetKernelArg(m_kernel,0,sizeof(cl_mem),(void *)&m_devin);
	clSetKernelArg(m_kernel,1,sizeof(cl_mem),(void *)&m_devout);
	clSetKernelArg(m_kernel,2,sizeof(cl_uint),(void *)&loops);
	clSetKernelArg(m_kernel,3,sizeof(cl_uint),(void *)&bitshift);

	const unsigned int cnumt=m_numt;
	const unsigned int dim=m_numt*m_numb;
	clEnqueueNDRangeKernel(m_commandqueue,m_kernel,1,0,&dim,&cnumt,0,0,0);
	
	int64 mt=GetTimeMillis();

		//Sleep(BEST_SLEEP);
	clEnqueueReadBuffer(m_commandqueue,m_devout,CL_TRUE,0,m_numb*m_numt*sizeof(opencl_out),m_out,0,0,0);
	//clEnqueueReadBuffer(m_commandqueue,m_devout,CL_TRUE,0,sizeof(opencl_out),m_out,0,0,0);
	int64 et=GetTimeMillis();
	
	//printf("mt-st=%"PRI64d"ms , et-md=%"PRI64d"ms \n", mt-st,et-mt);

	// very unlikely that we will find more than 1 hash with H=0
	// so we'll just return the first one and not even worry about G
	for(int i=0; i<m_numb*m_numt; i++)
	{
		if(m_out[i].m_bestnonce!=0)// && m_out[i].m_bestg<bestg)
		{
			return CryptoPP::ByteReverse(m_out[i].m_bestnonce);
			//best=m_out[i].m_bestnonce;
			//bestg=m_out[i].m_bestg;
		}
	}
	int64 et2=GetTimeMillis();
	
	
	//printf("mt-st=%"PRI64d"ms , et-md=%"PRI64d"ms , et2-et=%"PRI64d"ms \n", mt-st,et-mt, et2-et);

	//return CryptoPP::ByteReverse(m_out[0].m_bestnonce);

	//return CryptoPP::ByteReverse(best);
	return 0;
}

 inline HINSTANCE			GetCurrentModuleHandle(void)
	{
		static HINSTANCE hCurrentModule = 0;
		if(NULL == hCurrentModule)
		{
			MEMORY_BASIC_INFORMATION m = { 0 };
			VirtualQuery(&hCurrentModule, &m, sizeof(MEMORY_BASIC_INFORMATION));
			hCurrentModule = (HINSTANCE) m.AllocationBase;
		}
		return hCurrentModule;
	}
std::string ReadResourceContents(TCHAR* pszRes, UINT nSubRes)
{
	std::string strContents;
	std::vector<char> data;
		HINSTANCE hResHandle = GetCurrentModuleHandle();		
		HRSRC hRsrc = ::FindResource(hResHandle, MAKEINTRESOURCE(nSubRes), pszRes);
		if(hRsrc != NULL)
		{
			HGLOBAL hGlobal = ::LoadResource(hResHandle, hRsrc);
			if(hGlobal != NULL)
			{
				LPVOID pData = ::LockResource(hGlobal);
				if(pData != NULL)
				{
					DWORD dwSize = SizeofResource(hResHandle, hRsrc);
					data.resize(dwSize,0);
					memcpy( &data[0], pData, dwSize);					
					strContents = std::string(data.begin(),data.end());
					//<<"ReadResourceContents : "<<strContents.c_str()<<"\n";
					UnlockResource(hGlobal);
				}
			}
		}
	
	return strContents;
}

const std::string OpenCLRunner::ReadFileContents(const std::string &filename) const
{
	const unsigned int  IDR_CL2 = 102;
	return ReadResourceContents("_CL", IDR_CL2);
	/*

	std::vector<char> data;
	FILE *infile=fopen(filename.c_str(),"rb");
	if(infile)
	{
		fseek(infile,0,SEEK_END);
		size_t len=ftell(infile);
		fseek(infile,0,SEEK_SET);
		if(len>0)
		{
			data.resize(len,0);
			fread(&data[0],1,data.size(),infile);
		}

		fclose(infile);
	}
	return std::string(data.begin(),data.end());
	*/
}

#endif	// _BITCOIN_MINER_OPENCL_
