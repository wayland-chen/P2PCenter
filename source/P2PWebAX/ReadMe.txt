========================================================================
    活动模板库 : P2PWebAX 项目概况
========================================================================

应用程序向导为您创建了这个 P2PWebAX 项目，供您用作
编写动态链接库(DLL)的起点。
此项目用 Visual C++ 属性实现。

此文件包含
组成项目的每个文件的内容摘要。

P2PWebAX.vcproj
    这是用应用程序向导生成的 VC++ 项目的主项目文件。
    它包含有关生成此文件的 Visual C++ 版本的信息，以及
    有关使用应用程序向导选择的
    平台、配置和项目功能的信息。

_P2PWebAX.idl
    此文件将在生成项目时由编译器生成。
    它将包含项目中定义的类型库、接口和 co-class 的 IDL 定义。
    此文件将由 MIDL 编译器处理以生成:
        C++ 接口定义和 GUID 声明(_P2PWebAX.h)
        GUID 定义(_P2PWebAX_i.c)
        类型库(_P2PWebAX.tlb)
        封送处理代码(_P2PWebAX_p.c 和 dlldata.c)
P2PWebAX.cpp
    此文件包含对象映射和 DLL 导出的实现。
P2PWebAX.rc
    这是程序使用的所有 Microsoft Windows 资源
    的列表。

P2PWebAX.def
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
    这些文件用于生成名为 P2PWebAX.pch
    的预编译头(PCH)文件以及名为 StdAfx.obj 的预编译类型文件。

Resource.h
    这是定义资源 ID 的标准头文件。

/////////////////////////////////////////////////////////////////////////////
代理/存根(stub) DLL 项目和模块定义文件:

P2PWebAXps.vcproj
    此文件是生成代理/存根(stub) DLL (如有必要)的项目文件。
	主项目中的 IDL 文件必须至少包含一个接口，而且必须
	在生成代理/存根(stub) DLL 之前先编译 IDL 文件。	此过程生成
	dlldata.c、P2PWebAX_i.c 和 P2PWebAX_p.c，这些是
	生成代理/存根(stub) DLL 所需的。

P2PWebAXps.def
    此模块定义文件为链接器提供
    有关代理/存根(stub) 所需的导出的信息。
/////////////////////////////////////////////////////////////////////////////
