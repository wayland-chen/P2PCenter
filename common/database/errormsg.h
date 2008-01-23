/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */


#ifndef     __UD_ERROR_MESSAGE_H__
#define     __UD_ERROR_MESSAGE_H__


#ifdef WIN32
#include "mysql/include/errmsg.h" //mysql客户端的错误码定义
#else 
#include "errmsg.h" 
#endif


namespace DB_TOOLKIT
{

	typedef struct 
	{
		unsigned long   m_nError;
		char            m_szError[256];
	}ERROR_MSG;

	//一般的错误
	#define  DB_NOERROR                      0x00
	#define  DB_ERROR                        CR_MAX_ERROR+1 //CR_MAX_ERROR为mysql客户端错误定义最大值

	//没有实现
	#define  UD_ERROR_NOT_IMPLEMENT_IDX					0
	#define  UD_ERROR_NOT_IMPLEMENT						DB_ERROR+1

	//无效非法的连接字符串
	#define  UD_ERROR_CONN_CONNECTIONSTRING_IDX         1
	#define  UD_ERROR_CONN_CONNECTIONSTRING             DB_ERROR+2

	//无效的Command对象
	#define  UD_ERROR_INVALID_COMMAND_OBJ_IDX			2
	#define  UD_ERROR_INVALID_COMMAND_OBJ               DB_ERROR+3

	//无效的Connection对象								
	#define  UD_ERROR_INVALID_CONNECTION_OBJ_IDX		3
	#define  UD_ERROR_INVALID_CONNECTION_OBJ			DB_ERROR+4

	//无效的Recordset对象
	#define  UD_ERROR_INVALID_RECORDSET_OBJ_IDX			4
	#define  UD_ERROR_INVALID_RECORDSET_OBJ				DB_ERROR+5

	//无效的Row对象
	#define  UD_ERROR_INVALID_ROW_OBJ_IDX				5
	#define  UD_ERROR_INVALID_ROW_OBJ					DB_ERROR+6

	//无效的Parameter对象
	#define	 UD_ERROR_INVALID_PARAMETER_OBJ_IDX			6
	#define  UD_ERROR_INVALID_PARAMETER_OBJ				DB_ERROR+7

	//无活动的连接
	#define  UD_ERROR_NO_ACTIVE_CONN_IDX				7
	#define	 UD_ERROR_NO_ACTIVE_CONN					DB_ERROR+8

	//Recordset对象中当前记录的光标位于最后一条记录的后面
	#define  UD_ERROR_RST_EOF_IDX                       8
	#define  UD_ERROR_RST_EOF                           DB_ERROR+9

	//Recordset对象中当前记录的光标位于第一条记录的前面
	#define  UD_ERROR_RST_BOF_IDX                       9
	#define  UD_ERROR_RST_BOF                           DB_ERROR+10

	//无效的参数
	#define  UD_ERROR_INVALID_PARAMETER_IDX				10
	#define  UD_ERROR_INVALID_PARAMETER					DB_ERROR+11

	//无效的参数集合对象
	#define  UD_ERROR_INVALID_PARAM_COLLECTION_OBJ_IDX	11
	#define  UD_ERROR_INVALID_PARAM_COLLECTION_OBJ		DB_ERROR+12

	//分配内存失败
	#define  UD_ERROR_OUT_OF_MEMORY_IDX					12
	#define  UD_ERROR_OUT_OF_MEMORY						DB_ERROR+13

	//无效的转换操作
	#define  UD_ERROR_INVALID_CONVERSION_IDX			13
	#define  UD_ERROR_INVALID_CONVERSION				DB_ERROR+14

	//字段值为NULL
	#define  UD_ERROR_FIELD_VALUE_NULL_IDX				14
	#define  UD_ERROR_FIELD_VALUE_NULL					DB_ERROR+15

	extern ERROR_MSG   g_ErrorMsg[];
};

#endif
