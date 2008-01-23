/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __INTERFACE_FIELD_H__
#define     __INTERFACE_FIELD_H__

namespace DB_TOOLKIT
{

	class   CFieldInfo
	{
	public:
		enum  FieldType
		{
			ftNULL,
			ftBinary,ftByte,ftInt16,ftInt32,ftInt64,
			ftUInt16,ftUInt32,ftUInt64,
			ftSingle,ftDouble,ftDecimal,
			ftDate,ftTime,ftDateTime,
			ftString,ftVarString,ftWString,ftWVarString,
			ftBlob,
			ftCurrency, 
			ftSet,ftEnum,ftBoolean,ftGUID
		};

	public:
		CFieldInfo():m_strName()
		{ 
			m_Type = ftNULL;
			m_nLength = 0;
			m_lAttributes = 0; 
		}

		~CFieldInfo()
		{ }
	    
		const string&          GetName()
		{ 
			return m_strName; 
		}

		FieldType       GetType()
		{ 
			return m_Type; 
		}

		unsigned int    GetLength()
		{ 
			return m_nLength; 
		}

		long            GetAttributes()
		{ 
			return m_lAttributes; 
		}
	    
	public:
		string          m_strName;
		FieldType       m_Type;
		unsigned int    m_nLength;
		long            m_lAttributes;
	};
};

#endif
