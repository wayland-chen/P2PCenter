/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __INTERFACE_PARAMETER_H__
#define     __INTERFACE_PARAMETER_H__

#include "dbinterface0.h"

namespace DB_TOOLKIT
{

	class IParameter
	{
	public:
		typedef CFieldInfo::FieldType ParamType;

		IParameter()
		{
			m_Type = CFieldInfo::ftNULL;
			m_Direction = IParameter::Unknown;
		}
	    
		enum  ParamDirection
		{
			Unknown			= 0x00,
			Input           = 0x01,
			Output          = 0x02,
			InputAndOutput  = 0x03,
			ReturnType      = 0x04
		};
	public:
		ParamType       m_Type;
		string          m_strName;
		_db_variant_t   m_Value;
		ParamDirection  m_Direction;
	    
	};


	class IParameterCollection
	{
	public:
		IParameterCollection()
		{
		}
		void Append(const IParameter* pParam) throw(CDBBadParamException)
		{
			if(NULL == pParam)
			{
				///RETURN_WITH_VOID(CDBBadParamException("pParam is null"));
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,"pParam is null");
			}
			m_Items.push_back(*pParam);
		}

		void Remove(unsigned int nIndex) throw(CDBBadParamException)
		{
			if(nIndex>=(unsigned int)m_Items.size())
			{
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,"IParameterCollection:bad index");
			}
			unsigned int i=0;
			vector<IParameter>::iterator idx = m_Items.begin();
			while(i++!=nIndex)  idx++;
			m_Items.erase(idx);
	}

		void RemoveAll() throw()
		{
			m_Items.clear();
		}

		IParameter& operator[] (unsigned int nIndex) throw(CDBBadParamException)
		{
			if(nIndex >= (unsigned int)m_Items.size())
			{
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,"IParameterCollection:bad index");
			}
			return const_cast<IParameter&>(m_Items[nIndex]);
		}

		const IParameter& operator[] (unsigned int nIndex) const throw(CDBBadParamException)
		{
			if(nIndex >= (unsigned int)m_Items.size())
			{
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,"IParameterCollection:bad index");
			}
			return m_Items[nIndex];
		}

		IParameter& operator[] (const string& strParamName) throw(CDBBadParamException)
		{
			try
			{
				int nIndex = IndexOf(strParamName);
				if(-1 == nIndex)
				{
					throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER);
				}
				return operator[](nIndex);
			}
			catch(const CDBBadParamException&)
			{
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,strParamName + " not found");
			}
		}

		const IParameter& operator[] (const string& strParamName)  const throw(CDBBadParamException)
		{
			try
			{
				int nIndex = IndexOf(strParamName);
				if(-1 == nIndex)
				{
					throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER);
				}
				return operator[](nIndex);
			}
			catch(const CDBBadParamException&)
			{
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER,strParamName + " not found");
			}
		}

		int GetParamCount() throw()
		{
			return m_Items.size();
		}

		int IndexOf(const string& strParaName) const
		{
			for(string::size_type i=0;i<m_Items.size();i++)
			{
				if(m_Items[i].m_strName == strParaName)
				{
					return i;
				}
			}
			return -1;
		}

	private:
		IParameterCollection(const IParameterCollection&);
		const IParameterCollection& operator = (const IParameterCollection&);
		
	private:
		vector<IParameter>   m_Items;
	};
};

#endif
