========================================================================
    活动模板库 : ExtIconHandler 项目概况
========================================================================

应用程序向导为您创建了这个 ExtIconHandler 项目，供您用作
编写动态链接库(DLL)的起点。

此文件包含
组成项目的每个文件的内容摘要。

ExtIconHandler.vcproj
    这是用应用程序向导生成的 VC++ 项目的主项目文件。
    它包含有关生成此文件的 Visual C++ 版本的信息，以及
    有关使用应用程序向导选择的
    平台、配置和项目功能的信息。

ExtIconHandler.idl
    此文件包含项目中定义的类型库、
    接口和 co-class 的 IDL 定义。
    此文件将由 MIDL 编译器处理以生成:
        C++ 接口定义和 GUID 声明(ExtIconHandler.h)
        GUID 定义(ExtIconHandler_i.c)
        类型库(ExtIconHandler.tlb)
        封送处理代码(ExtIconHandler_p.c and dlldata.c)

ExtIconHandler.h
    此文件包含在 ExtIconHandler.idl 中定义的项的 C++
    接口定义和 GUID 声明。它将在编译期间由 MIDL 重新生成。
ExtIconHandler.cpp
    此文件包含对象映射和 DLL 导出的实现。
ExtIconHandler.rc
    这是程序使用的所有 Microsoft Windows 资源
    的列表。

ExtIconHandler.def
    此模块定义文件为链接器提供有关 DLL 所需的
    导出的信息。它包含如下内容的导出:
        DllGetClassObject  
        DllCanUnloadNow    
        GetProxyDllInfo    
        DllRegisterServer	
        DllUnregisterServer

/////////////////////////////////////////////////////////////////////////////
其他标准文件:

StdAfx.h、StdAfx.cpp
    这些文件用于生成名为 ExtIconHandler.pch
    的预编译头(PCH)文件以及名为 StdAfx.obj 的预编译类型文件。

Resource.h
    这是定义资源 ID 的标准头文件。

/////////////////////////////////////////////////////////////////////////////
