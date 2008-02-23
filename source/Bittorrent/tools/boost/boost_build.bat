@ECHO OFF
@ECHO ON

SET MSVC_ROOT="C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7"

SET VC7_ROOT="C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7"

SET VISUALC_ROOT="C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7"

SET JAM_TOOLSET=VISUALC

tools\jam\src\bin.ntx86\bjam --toolset=msvc-7.1 --stagedir=./lib_x86 --builddir=./ address-model=32 link=static runtime-link=static threading=multi stage debug release
