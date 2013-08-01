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

#if defined(_BITCOIN_MINER_CUDA_) || defined(_BITCOIN_MINER_OPENCL_)
#include "iostream"
#include "rpcminerthreadgpu.h"
#include "hex.h"

RPCMinerThreadGPU::RPCMinerThreadGPU()
{
}

RPCMinerThreadGPU::~RPCMinerThreadGPU()
{
}

void RPCMinerThreadGPU::Run(void *arg)
{
	threaddata *td=(threaddata *)arg;
	int64 currentblockid=-1;

	static const unsigned int SHA256InitState[8] ={0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	uint256 tempbuff[4];
	uint256 &temphash=*alignup<16>(tempbuff);
	uint256 hashbuff[4];
	uint256 &hash=*alignup<16>(hashbuff);
	uint256 currenttarget;

	unsigned char currentmidbuff[256];
	unsigned char currentblockbuff[256];
	volatile unsigned char *midbuffptr;
	volatile unsigned char *blockbuffptr;
	volatile unsigned int *nonce;
	unsigned int gpunonce;
	midbuffptr=alignup<16>(currentmidbuff);
	blockbuffptr=alignup<16>(currentblockbuff);
	nonce=(unsigned int *)(blockbuffptr+12);

	gpurunnertype gpu;

	if(gpu.GetDeviceIndex()<0 && gpu.GetDeviceCount()<=0)
	{
		TerminateProcess(GetCurrentProcess(), (UINT)-11);
		return;
	}

	SetThreadPriority(THREAD_PRIORITY_LOWEST);

	FormatHashBlocks(&temphash,sizeof(temphash));
	for(int i=0; i<64/4; i++)
	{
		((unsigned int*)&temphash)[i] = CryptoPP::ByteReverse(((unsigned int*)&temphash)[i]);
	}

	(*nonce)=0;

	gpu.FindBestConfiguration();

	int iterations=10000/(gpu.GetStepIterations()+gpu.GetNumBlocks()+gpu.GetNumThreads());
	
	int nSub = 0;
	while(td->m_generate)
	{
	
		if(td->m_havework)
		{
			int64 t1, t2 ,t3, t4, t5, t0;

			t0=GetTimeMillis();

			CRITICAL_BLOCK(td->m_cs);
			if(currentblockid!=td->m_nextblock.m_blockid)
			{

				std::string block, midstate;
				Hex::Encode(td->m_nextblock.m_block, block);
				Hex::Encode(td->m_nextblock.m_midstate,midstate );
				printf("findhashes_0 m_blockid=%I64d m_block=%s \n" , currentblockid, block.c_str());
				printf("findhashes_0 m_blockid=%I64d midstate=%s \n" , currentblockid, midstate.c_str());



				currenttarget=td->m_nextblock.m_target;
				currentblockid=td->m_nextblock.m_blockid;
				::memcpy((void *)midbuffptr,&td->m_nextblock.m_midstate[0],32);
				::memcpy((void *)blockbuffptr,&td->m_nextblock.m_block[0],64);
				::memcpy(&temphash,&td->m_nextblock.m_hash1[0],64);
				(*nonce)=0;
				for(int i=0; i<8; i++)
				{
					gpu.GetIn()->m_AH[i]=((unsigned int *)midbuffptr)[i];
				}
				gpu.GetIn()->m_merkle=((unsigned int *)blockbuffptr)[0];
				gpu.GetIn()->m_ntime=((unsigned int *)blockbuffptr)[1];
				gpu.GetIn()->m_nbits=((unsigned int *)blockbuffptr)[2];
			}
		 
			 
			t1=GetTimeMillis();

			for(int i=0; i<iterations; i++)
			{	
				int64 tt1, tt2 ;
				tt1=GetTimeMillis();

				gpu.GetIn()->m_nonce=(*nonce);				
				gpunonce=gpu.RunStep();					 
				if(gpunonce!=0)
				{
					(*nonce)=CryptoPP::ByteReverse(gpunonce);
					SHA256Transform(&temphash,(void *)blockbuffptr,(void *)midbuffptr);
					SHA256Transform(&hash,&temphash,SHA256InitState);

					if(hash<currenttarget)
					{
						CRITICAL_BLOCK(td->m_cs);
						td->m_foundhashes.push_back(foundhash(currentblockid,(*nonce)));
						
						printf("findhashes_1 m_blockid=%I64d hash=%s | currenttarget=%s\n" , currentblockid, hash.ToString().c_str(), currenttarget.ToString().c_str());

						std::string block, _hash;
						Hex::Encode(td->m_nextblock.m_block, block);						
						printf("findhashes_1 m_blockid=%I64d m_block=%s  \n", currentblockid, block.c_str() );												
					}
				}

				(*nonce)+=(gpu.GetStepIterations()*gpu.GetNumBlocks()*gpu.GetNumThreads());

				{
					CRITICAL_BLOCK(td->m_cs);					
					td->m_hashcount+=(gpu.GetStepIterations()*gpu.GetNumBlocks()*gpu.GetNumThreads());
				}
				tt2=GetTimeMillis();
				//printf("tt2-tt1=%"PRI64d"ms | iterations-i=%d   \n",
				//tt2-tt1 ,iterations-i );

			}
			t2=GetTimeMillis();
		//	printf("currentblockid=%"PRI64d" | t1-t0=%"PRI64d"ms | t2-t1=%"PRI64d"ms  \n",
			//	currentblockid, t1-t0, t2-t1 );
		}
		else
		{
			printf("  Sleep(100) \n" );
			Sleep(100);
		}

	}

	{
		CRITICAL_BLOCK(td->m_cs);
		td->m_done=true;
	}

}

#endif	// defined(_BITCOIN_MINER_CUDA_) || defined(_BITCOIN_MINER_OPENCL_)
