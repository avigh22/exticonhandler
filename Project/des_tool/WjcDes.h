//////////////////////////////////////////////////////////////////////////
/*
    Provided by ������, Northeastern University (www.neu.edu.cn)
    Email: blackdrn@sohu.com
	This product is free for use.
*/
//////////////////////////////////////////////////////////////////////////

enum	{ENCRYPT,DECRYPT};

//////////////////////////////////////////////////////////////////////////

// ��/���� Type��ENCRYPT:����,DECRYPT:����
void Des_Run(char Out[8], char In[8], bool Type=ENCRYPT);
void Des_Run(char * pOut, char* pIn, __int64  * pInOutLen , bool Type=ENCRYPT, char fill = ' ');
// ������Կ
void Des_SetKey(const char Key[8]);

//////////////////////////////////////////////////////////////////////////

