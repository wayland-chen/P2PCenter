; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

; 安装程序初始定义常量
!define PRODUCT_NAME "Kan8888_P2PClient"
!define PRODUCT_VERSION "1.02"
!define PRODUCT_PUBLISHER "Kevin, Inc."
!define PRODUCT_WEB_SITE "http://p2p.kan8888.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\P2PService.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
!insertmacro MUI_PAGE_LICENSE "copyright.txt"
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
!define MUI_FINISHPAGE_RUN "$INSTDIR\P2PIEClient.exe"
!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "P2PClientSetup.exe"
InstallDir "$PROGRAMFILES\Kan8888_P2P客户端"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  CreateDirectory "$SMPROGRAMS\Kan8888_P2P客户端"
  CreateShortCut "$SMPROGRAMS\Kan8888_P2P客户端\Kan8888_P2P客户端.lnk" "$INSTDIR\P2PIEClient.exe"
  CreateShortCut "$SMPROGRAMS\Kan8888_P2P客户端\卸载.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\Kan8888_P2P客户端.lnk" "$INSTDIR\P2PIEClient.exe"

  KillProcDLL::KillProc "Updater.exe"
  KillProcDLL::KillProc "Iexplore.exe"
  KillProcDLL::KillProc "P2PIEClient.exe"

  ExecWait 'Net Stop K8P2PService'

  File "P2PService.exe"
  File "P2PIEClient.exe"
  File "P2PIEClient.ini"
  File "P2PNetwork.ini"
  File "P2PNetwork.dll"
  File "P2PWebAX.dll"
  File "TcpClient.dll"
  File "Updater.exe"
  File "Updater.ini"
  File "msvcr71.dll"
  File "msvcp71.dll"
  
  RegDll "$INSTDIR\P2PWebAX.dll"

  ExecWait '"$INSTDIR\P2PService.exe" /Service'
  ExecWait 'Net Start K8P2PService'
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\P2PService.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\P2PService.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
  ExecWait '"$INSTDIR\P2PService.exe" -UnRegServer'
  KillProcDLL::KillProc "P2PService.exe"
  KillProcDLL::KillProc "Updater.exe"
  KillProcDLL::KillProc "Iexplore.exe"
  KillProcDLL::KillProc "P2PIEClient.exe"
  Delete "$INSTDIR\P2PService.exe"
  Delete "$INSTDIR\P2PIEClient.exe"
  Delete "$INSTDIR\P2PIEClient.ini"
  Delete "$INSTDIR\Updater.exe"
  UnRegDll "$INSTDIR\P2PWebAX.dll"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\msvcp71.dll"
  Delete "$INSTDIR\msvcr71.dll"
  Delete "$INSTDIR\P2PWebAX.dll"
  Delete "$INSTDIR\P2PNetwork.dll"
  Delete "$INSTDIR\TcpClient.dll"
  Delete "$INSTDIR\P2PNetwork.ini"
  Delete "$INSTDIR\Updater.ini"

  Delete "$DESKTOP\Kan8888_P2P客户端.lnk"
  Delete "$SMPROGRAMS\Kan8888_P2P客户端\Kan8888_P2P客户端.lnk"
  Delete "$SMPROGRAMS\Kan8888_P2P客户端\卸载.lnk"
  RMDir "$SMPROGRAMS\Kan8888_P2P客户端"
  
  RMDir /r "$INSTDIR\log"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2
  Abort
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
FunctionEnd
