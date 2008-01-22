/********************************************************************
	created:	2008/01/11
	created:	11:1:2008   15:49
	author:		Kevin
	
	purpose:	

	email:		kan8888.com@163.com
	msn:		fuwenke@gmail.com
	QQ:		799383417
	site:		opensource.kan8888.com
*********************************************************************/
#ifndef __INI_FILE_H_
#define __INI_FILE_H_


#include "StringBuffer.h"

#include "Array.h"

using namespace KBASE;

/*定义操作类*/
class CIniFile
{
//构造与析构函数
public:
	CIniFile();
	CIniFile(const char* sFileName);
	virtual ~CIniFile();

//定义私有数据成员
protected:
	//定义行结构
	struct LINE_ITEM
	{
		int 	nType;		//类型
		CString	sName;		//名称
		CString sValue;		//值
	};
	CPtrArray   	m_arrayOfLine;	//存放配置文件中的所有行
	CString			m_sFileName;	//配置文件名称
	int				m_nTotalSection;//总区域数
	bool			m_bAutoFlush;	//是否自动刷新到文件?
	bool			m_bModified;	//是否修改过
	
//文件操作方法
public:
	//重新初始化本对象
	void Reset(void);
	//载入配置文件,成功返回0,失败返回小于0的值
	int LoadFromFile(const char* sFileName);
	//刷新当前配置项到文件中,成功返回0,失败返回涉于0的值
	int FlushToFile(void);

//区域和配置项操作方法
private:
	//根据区域名称,取得所在行号
	int GetSectionIndex(int nStartLine, const char* sSectionName);
	//根据区域行号及配置项名称,取得配置项所在行号
	int GetKeyIndex(int nSectionIndex, const char* sKeyName);
public:
	//取得总区域数
	int GetSectionCount(void);
	//取得区域名称
	CString GetSectionName(int nIndex);
	//取得指定区域中总配置项数
	int GetKeyCount(const char* sSectionName);
	//取得配置项名称
	CString GetKeyName(const char* sSectionName, int nIndex);
	//删除一个配置项
	int DelKey(const char* sSectionName, const char* sKeyName);
	//增加一个区域,返回索引值,失败返回小于0的值
	int AddSection(const char* sSectionName);
	//删除一个区域
	void DelSection(const char* sSectionName);
	
//配置项值操作方法
public:
	//取得配置项的值(字符串)
	CString GetString(const char* sSectionName, const char* sKeyName);
	//取得配置项的值(整数)
	int GetInt(const char* sSectionName, const char* sKeyName);
	//取得配置项的值(布尔)
	bool GetBool(const char* sSectionName, const char* sKeyName);
	//设置配置项的值(字符串)
	void SetString(const char* sSectionName, const char* sKeyName, const char* sKeyValue);
	//设置配置项的值(整数)
	void SetInt(const char* sSectionName, const char* sKeyName, int nKeyValue);
	//设置配置项的值(布尔)
	void SetBool(const char* sSectionName, const char* sKeyName, bool bKeyValue);
};

#endif
