========================================================================
    �ģ��� : ExtIconHandler ��Ŀ�ſ�
========================================================================

Ӧ�ó�����Ϊ����������� ExtIconHandler ��Ŀ����������
��д��̬���ӿ�(DLL)����㡣

���ļ�����
�����Ŀ��ÿ���ļ�������ժҪ��

ExtIconHandler.vcproj
    ������Ӧ�ó��������ɵ� VC++ ��Ŀ������Ŀ�ļ���
    �������й����ɴ��ļ��� Visual C++ �汾����Ϣ���Լ�
    �й�ʹ��Ӧ�ó�����ѡ���
    ƽ̨�����ú���Ŀ���ܵ���Ϣ��

ExtIconHandler.idl
    ���ļ�������Ŀ�ж�������Ϳ⡢
    �ӿں� co-class �� IDL ���塣
    ���ļ����� MIDL ����������������:
        C++ �ӿڶ���� GUID ����(ExtIconHandler.h)
        GUID ����(ExtIconHandler_i.c)
        ���Ϳ�(ExtIconHandler.tlb)
        ���ʹ������(ExtIconHandler_p.c and dlldata.c)

ExtIconHandler.h
    ���ļ������� ExtIconHandler.idl �ж������� C++
    �ӿڶ���� GUID �����������ڱ����ڼ��� MIDL �������ɡ�
ExtIconHandler.cpp
    ���ļ���������ӳ��� DLL ������ʵ�֡�
ExtIconHandler.rc
    ���ǳ���ʹ�õ����� Microsoft Windows ��Դ
    ���б�

ExtIconHandler.def
    ��ģ�鶨���ļ�Ϊ�������ṩ�й� DLL �����
    ��������Ϣ���������������ݵĵ���:
        DllGetClassObject  
        DllCanUnloadNow    
        GetProxyDllInfo    
        DllRegisterServer	
        DllUnregisterServer

/////////////////////////////////////////////////////////////////////////////
������׼�ļ�:

StdAfx.h��StdAfx.cpp
    ��Щ�ļ�����������Ϊ ExtIconHandler.pch
    ��Ԥ����ͷ(PCH)�ļ��Լ���Ϊ StdAfx.obj ��Ԥ���������ļ���

Resource.h
    ���Ƕ�����Դ ID �ı�׼ͷ�ļ���

/////////////////////////////////////////////////////////////////////////////
