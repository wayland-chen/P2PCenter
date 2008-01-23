/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __DB_EXCEPTIONS_H__
#define     __DB_EXCEPTIONS_H__

#pragma warning( disable : 4290 )

#include "errormsg.h"

#include <string>
using namespace std;

namespace DB_TOOLKIT
{

	class CDBException
	{
	public:
		
		CDBException(long lErrorCode,string strError="") throw()
		{ 
			m_strError = strError;
			m_lErrorCode = lErrorCode;
		}
	    
		virtual ~CDBException() throw()
		{ }

	public:
		string ErrorMessage() const throw()
		{ return m_strError; }
		long   Error() const throw()
		{ return m_lErrorCode; }
	protected:
		string  m_strError;
		long	m_lErrorCode;
	};


	class CDBBadParamException : public CDBException
	{
	public:
		CDBBadParamException(long lErrorCode,string strError="Bad Parameter!!!!!") throw()
			:CDBException(lErrorCode,"Bad Parameter:"+strError)
		{
		};
		CDBBadParamException(long lErrorCode,const string& strFuncName,const string& strParamName,const string& strPassValue) throw()
			:CDBException(lErrorCode,"Bad Parameter:Parameter " + strParamName + 
									" receive bad value:" + strPassValue + 
									" in function " + strFuncName)
		{
	        
		}
	};

	class CDBBadQuery : public CDBException
	{
	public:
		CDBBadQuery(long lErrorCode,string strError="Bad Query!!!!!!!!!") throw()
			:CDBException(lErrorCode,"Bad Query:"+strError)
		{
		}
	};

	class CDBBadConversion : public CDBException
	{
	public:
		CDBBadConversion(long lErrorCode,string strError = "Bad Conversion!!!!!!!!!") throw()
			:CDBException(lErrorCode,"Bad Conversion:"+strError)
		{
		};
		CDBBadConversion(long lErrorCode,const string& strTypeName,const string& strData) throw()
			:CDBException(lErrorCode,"Bad Conversion: try to convert " + 
									strData + " to a " + strTypeName)
		{
		}
	};

	class CDBOutOfMemory : public CDBException
	{
	public:
		CDBOutOfMemory(long lErrorCode,string strError="Error:Out of Memory!") throw()
			:CDBException(lErrorCode,strError)
		{ }
	};
};

#endif
