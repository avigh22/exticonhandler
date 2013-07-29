// des_tool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <fstream>
#include "iosfwd"
#include "ec.h"
#include "shlwapi.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	setkey("SHhost");
	if(argc == 3)
	{
			ifstream fs;
			fs.open( argv[1], ios::in|ios::binary );
			bool b = fs.fail();
				if(b)
				{ 
					std::cout<<"can not open "<<argv[1];
					return 1;
				}

			fs.seekg(0, ios::end);
			unsigned long n = fs.tellg();
			 fs.seekg(0, ios::beg);
			char * buf = new char[n+5]; ;
			fs.read( buf, n );
			buf[n] = '\0';
			fs.close();
			
			ec(buf, n); 
			ofstream os;
			os.open(argv[2], ios::out|ios::binary|ios::trunc);
			fs.seekg(0, ios::beg);
			bool bo = os.fail();
			if(bo)
			{
				
					std::cout<<"can not save to "<<argv[2];
					return 1;
			}
			os.write(buf, n);
			os.close();
			std::cout<<"success.";

	}
	else if(argc == 4)
	{
		ifstream fs;
			fs.open( argv[1], ios::in|ios::binary );
			bool b = fs.fail();
				if(b)
				{ 
					std::cout<<"can not open "<<argv[1];
					return 1;
				}
			fs.seekg(0, ios::end);
			unsigned long n = fs.tellg();
			fs.seekg(0, ios::beg);
			 
			char * buf = new char[n]; ;
			fs.read( buf, n );
			 
			fs.close();
			
			dc(buf, n ); 
			ofstream os;
			os.open(argv[2], ios::out|ios::trunc|ios::binary);
			bool bo = os.fail();
			if(bo)
			{
				
					std::cout<<"can not save to "<<argv[2];
					return 1;
			}
			os.write(buf, n);
			os.close();
			std::cout<<"success.";

	}
	else
	{
		std::cout<<"	eg. des_tool.exe "<<"c:\\a.txt"<<" c:\\a.txt.dec"<<"\n";
		
		std::cout<<"	eg. des_tool.exe "<<"c:\\a.txt.dec"<<" c:\\a.txt 1"<<"\n";
	}

	return 0;
}

