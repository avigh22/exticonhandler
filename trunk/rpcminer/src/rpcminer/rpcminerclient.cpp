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

#include "rpcminerclient.h"

#include "httprequest.h"
#include "hex.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdarg>
#include <cstdio>

int OutputDebugStringF(const char* pszFormat, ...)
{
	char buffer[1024]={0};
	va_list va;
	va_start(va,pszFormat);

	vsnprintf(buffer,1023,pszFormat,va);

	std::cout << buffer;

	va_end(va);

	return 0;
}

RPCMinerClient::RPCMinerClient():m_blockid(0),m_serverstatsurl(""),m_workrefreshms(4000),m_foundcount(0),m_hashraterefresh(10000),m_lasthashdisplay(0),m_done(true),m_running(false)
{
	m_startuptime=GetTimeMillis();
}

RPCMinerClient::~RPCMinerClient()
{
}

void RPCMinerClient::CleanupOldBlocks()
{

	std::vector<int64> erase;

	for(std::map<int64,std::pair<int64,std::vector<unsigned char> > >::iterator i=m_blocklookup.begin(); i!=m_blocklookup.end(); i++)
	{
		if((*i).second.first<GetTimeMillis()-60000)
		{
			erase.push_back((*i).first);
		}
	}

	for(std::vector<int64>::const_iterator i=erase.begin(); i!=erase.end(); i++)
	{
		m_blocklookup.erase((*i));
	}

}

const bool RPCMinerClient::DecodeBase64(const std::string &encoded, std::vector<unsigned char> &decoded) const
{
	if(encoded.size()>0)
	{
		int dlen=((encoded.size()*3)/4)+4;
		decoded.resize(dlen,0);
		std::vector<unsigned char> src(encoded.begin(),encoded.end());
		if(base64_decode(&decoded[0],&dlen,&src[0],src.size())==0)
		{
			decoded.resize(dlen);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		decoded.resize(0);
		return true;
	}
}

const bool RPCMinerClient::EncodeBase64(const std::vector<unsigned char> &data, std::string &encoded) const
{
	if(data.size()>0)
	{
		int dstlen=((data.size()*4)/3)+4;
		std::vector<unsigned char> dst(dstlen,0);
		if(base64_encode(&dst[0],&dstlen,&data[0],data.size())==0)
		{
			dst.resize(dstlen);
			encoded.assign(dst.begin(),dst.end());
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		encoded=std::string("");
		return true;
	}
}

const int64 RPCMinerClient::GetHashRate(const bool reset)
{
	int64 rate=0;
	int64 timenowmillis=GetTimeMillis();
	int64 count=m_minerthreads.GetHashCount(reset);
	if((timenowmillis-m_lasthashdisplay)>0)
	{
		rate=(count/(timenowmillis-m_lasthashdisplay));
	}
	m_lasthashdisplay=timenowmillis;
	return rate;
}

const bool RPCMinerClient::GetServerStats(json_spirit::Object &stats)
{
	std::string result("");

	HTTPRequest req(m_serverstatsurl);
	if(req.DoRequest(result))
	{
		json_spirit::Value val;
		if(json_spirit::read(result,val))
		{
			if(val.type()==json_spirit::obj_type)
			{
				stats=val.get_obj();
				return true;
			}
		}
	}

	return false;

}

const std::string RPCMinerClient::GetTimeStr(const time_t timet) const
{
	std::vector<char> buff(128,0);
	struct tm timetm;
	timetm=*gmtime(&timet);
	size_t len=strftime(&buff[0],buff.size(),"%Y-%m-%d %H:%M:%S",&timetm);
	if(len>0)
	{
		return std::string(buff.begin(),buff.begin()+len);
	}
	else
	{
		return std::string("");
	}
}

void RPCMinerClient::PrintServerStats(json_spirit::Object &stats)
{
	int activeworkers=0;
	//int khashesps=0;
	int shares=0;
	std::string ghashesps("");
	std::string roundstarted("");
	json_spirit::Value val;

	val=json_spirit::find_value(stats,"active_workers");
	if(val.type()==json_spirit::int_type)
	{
		activeworkers=val.get_int();
	}
	
	val=json_spirit::find_value(stats,"ghashes_ps");
	if(val.type()==json_spirit::str_type)
	{
		ghashesps=val.get_str();
	}

	val=json_spirit::find_value(stats,"shares");
	if(val.type()==json_spirit::int_type)
	{
		shares=val.get_int();
	}

	val=json_spirit::find_value(stats,"round_started");
	if(val.type()==json_spirit::str_type)
	{
		roundstarted=val.get_str();
	}

	std::cout << "Server Stats : " << activeworkers << " active workers at an estimated " << ghashesps << " Ghash/s" << std::endl;
	std::cout << shares << " shares in current round, which started " << roundstarted << std::endl;
}

void RPCMinerClient::Run(const std::string &url, const std::string &user, const std::string &password, const int threadcount)
{
	m_running=true;
	m_done=false;
	m_url=url;
	m_user=user;
	m_password=password;
	m_threadcount=threadcount;
	int64 lastrequestedwork=GetTimeMillis()-5000;
	int64 laststats=GetTimeMillis()-25000;
	m_lasttarget=0;
	m_lasthashdisplay=GetTimeMillis();
	int64 timenowmillis;

	std::cout << "Client will start " << m_threadcount << " miner threads" << std::endl;
	std::cout << "Work will be refreshed every " << m_workrefreshms << " ms" << std::endl;

	while(Done()==false)
	{
		if(m_minerthreads.RunningThreadCount()<m_threadcount)
		{
			m_minerthreads.Start(new threadtype);
		}

		
		if(lastrequestedwork<=GetTimeMillis()-m_workrefreshms)
		{
			SendWorkRequest();
			lastrequestedwork=GetTimeMillis();
		}

		if(m_serverstatsurl!="" && laststats<=GetTimeMillis()-60000)
		{
			json_spirit::Object statsobj;
			if(GetServerStats(statsobj))
			{
				PrintServerStats(statsobj);
			}
			laststats=GetTimeMillis();
		}

		if(m_minerthreads.HaveFoundHash())
		{
			std::cout << GetTimeStr(time(0)) << " Found Hash!" << std::endl;
			RPCMinerThread::foundhash fhash;
			m_minerthreads.GetFoundHash(fhash);
			SendFoundHash(fhash.m_blockid,fhash.m_nonce);
			SendWorkRequest();
		}

		CleanupOldBlocks();
		
		timenowmillis=GetTimeMillis();
		if(m_hashraterefresh>0 && (m_lasthashdisplay+m_hashraterefresh)<=timenowmillis)
		{
			int64 hr=GetHashRate(true);
			if(m_blocklookup.size()>0)
			{
				std::cout << hr << " khash/s" << std::endl;
				static int64 allhr = 0;
				static int hrcount = 1;
				allhr += hr;
				if(hr >0 )
					hrcount++;
				
				TCHAR szInvalid[256] = {0};				 
				GetEnvironmentVariable("-invalidworker" , szInvalid, 256);

				HANDLE h = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,128,L"rpcxx_speed");
				int lret = 0;
				if(h)
					lret = GetLastError();
				LPWSTR lpMapAddress = NULL;
				lpMapAddress =(LPWSTR)(LPVOID) MapViewOfFile((HANDLE)(LONG_PTR)h, FILE_MAP_WRITE,0,0,128);
				
				wchar_t buf[32] = {0};
				_ltow(hr, buf, 10);
				if(lpMapAddress && hr>0)
					wcscpy((LPWSTR)lpMapAddress, buf);
				UnmapViewOfFile(lpMapAddress);
				if(lret == ERROR_ALREADY_EXISTS)
					CloseHandle(h);
				if(0 == _stricmp("1",szInvalid))
				{
					
				}
				else
				{
					if((hrcount == 3) && (allhr/hrcount <30000) )
					{
					//	std::cout<<" hrcount : "<<hrcount<<" , allhr/hrcount : "<<allhr/hrcount<<"\n";
						//Sleep(1000);
					//	Stop();
					}
				}
				
			}
			else
			{
				if(m_startuptime+30000<timenowmillis)
				{
					std::cout << "No blocks are being hashed right now.  This can happen if the application is" << std::endl;
					std::cout << "still starting up, you supplied incorrect parameters, or there is a" << std::endl;
					std::cout << "communications error connecting to the RPC server." << std::endl;
					m_minerthreads.ClearWork();
				}
			}
			
		}

		Sleep(100);
	}

	m_running=false;

}

void RPCMinerClient::SaveBlock(json_spirit::Object &block, const std::string &filename)
{
	std::ofstream file(filename.c_str(),std::ios::out|std::ios::trunc);
	json_spirit::write_formatted(block,file);
	file.close();
}

void RPCMinerClient::SendFoundHash(const int64 blockid, const unsigned int nonce)
{
	
	json_spirit::Object obj;
	json_spirit::Array params;
	m_rpcreq.SetURL(m_url);
	m_rpcreq.SetUser(m_user);
	m_rpcreq.SetPassword(m_password);
	std::string res("");
	std::string blockdatastr("");

	if(m_blocklookup.find(blockid)!=m_blocklookup.end())
	{

		std::vector<unsigned char> blockdata(m_blocklookup[blockid].second);

		unsigned int *nonceptr=(unsigned int *)(&blockdata[64+12]);
		(*nonceptr)=nonce;

		Hex::Encode(blockdata,blockdatastr);
		std::cout<<"sendhoundhash blockid="<<blockid<<" blockdata "<<blockdatastr.c_str()<<"\n";
		params.push_back(blockdatastr);

		obj.push_back(json_spirit::Pair("method","getwork"));
		obj.push_back(json_spirit::Pair("params",params));
		obj.push_back(json_spirit::Pair("id",1));

		std::cout << "Sending to server: " << json_spirit::write(obj) << std::endl;

		int tries=0;
		bool done=false;
		while((done=m_rpcreq.DoRequest(json_spirit::write(obj),res))==false && tries++<5)
		{
			std::cout << "Retrying" << std::endl;
			Sleep(100);
		}

		if(done)
		{
			std::cout << "Server sent: " << res << std::endl;
			m_foundcount++;
		}
		else
		{
			std::cout << "Error while sending work to server!" << std::endl;
			std::cout << "CURL return value = " << m_rpcreq.GetLastCurlReturn() << std::endl;
		}

	}
	else
	{
		std::cout << "One or more of the blocks you are working on is stale." << std::endl;
		std::cout << "You or the server might be having connection issues." << std::endl;
	}

}

void RPCMinerClient::SendWorkRequest()
{
	json_spirit::Object obj;
	json_spirit::Array params;
	m_rpcreq.SetURL(m_url);
	m_rpcreq.SetUser(m_user);
	m_rpcreq.SetPassword(m_password);
	std::string res("");

	obj.push_back(json_spirit::Pair("method","getwork"));
	obj.push_back(json_spirit::Pair("params",params));
	obj.push_back(json_spirit::Pair("id",0));

	if(m_rpcreq.DoRequest(json_spirit::write(obj),res))
	{
		json_spirit::Value resobj;
		if(json_spirit::read(res,resobj))
		{
			if(resobj.type()==json_spirit::obj_type)
			{
				json_spirit::Value resval;
				resval=json_spirit::find_value(resobj.get_obj(),"result");

				if(resval.type()==json_spirit::obj_type)
				{
					uint256 target;
					std::vector<unsigned char> hash1;
					std::vector<unsigned char> block;
					std::vector<unsigned char> midstate;
					m_blockid++;
					
					json_spirit::Value val;
					std::string strOutputUrl = " url:"+m_url;
					std::cout<<" url="<<m_url.c_str()<<"\n";

					val=json_spirit::find_value(resval.get_obj(),"hash1");
					if(val.type()==json_spirit::str_type)
					{
						Hex::Decode(val.get_str(),hash1);
						std::cout<<" hash1="<<val.get_str().c_str()<<"\n";						
					}
					val=json_spirit::find_value(resval.get_obj(),"data");
					if(val.type()==json_spirit::str_type)
					{
						Hex::Decode(val.get_str(),block);
						std::cout<<" data="<<val.get_str().c_str()<<"\n";
						
						m_blocklookup[m_blockid].first=GetTimeMillis();
						m_blocklookup[m_blockid].second=block;

						if(block.size()>64)
						{
							block.erase(block.begin(),block.begin()+64);
						}
						std::string strData2;
						Hex::Encode(block, strData2);
						std::cout<<" data2="<<strData2.c_str()<<"\n";
						 			
					}
					val=json_spirit::find_value(resval.get_obj(),"midstate");
					if(val.type()==json_spirit::str_type)
					{
						Hex::Decode(val.get_str(),midstate);
						std::cout<<" midstate="<<val.get_str().c_str()<<"\n";						
					}
					val=json_spirit::find_value(resval.get_obj(),"target");
					if(val.type()==json_spirit::str_type)
					{
						target.SetHex(val.get_str());
						std::cout<<" target="<<Hex::Reverse(target.ToString())<<"\n";						
					}
					//std::cout << "Target = " << Hex::Reverse(target.ToString()) << std::endl;					

					m_minerthreads.SetNextBlock(m_blockid,target,block,midstate,hash1);

					if(target!=m_lasttarget)
					{
						std::cout << "Target = " << Hex::Reverse(target.ToString()) << std::endl;
						m_lasttarget=target;
					}

				}
			}
		}
	}
	else
	{
		std::cout << "Could not retrieve work from RPC server." << std::endl;
		std::cout << "CURL return value = " << m_rpcreq.GetLastCurlReturn() << std::endl;
	}
}
