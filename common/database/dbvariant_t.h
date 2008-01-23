/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __DB_VARIANT_H__
#define     __DB_VARIANT_H__

#include <time.h>
#include "os.h"
#include "dbexception.h"
#include "utilities.h"
#include "dbvariant1.h"
#include <assert.h>

namespace DB_TOOLKIT
{

	class _db_variant_t : public DBVARIANT
	{
	public:
		_db_variant_t() throw()
		{ vt = VT_DBEMPTY; }

		_db_variant_t(const _db_variant_t& var) throw(CDBOutOfMemory)
		{ 
		try
		{
			if(this != &var)
			{
				switch(var.vt)
				{
				case VT_DBBSTR:
					CopyToBSTR(BSTR_TO_CCHAR(var.bstrValue));
					break;
				case VT_DBBLOB:
					CopyToBLOB(BLOB_TO_CREALDATA(var.blobValue),BLOB_LENGTH(var.blobValue));
					break;
				default:
					memcpy(this,&var,sizeof(_db_variant_t));
					break;
				}
				vt = var.vt;
			}
		}
		catch(const CDBOutOfMemory& e)
		{
			Clear();
			vt = VT_DBEMPTY;
			throw e;
		}
		catch(...)
		{
			Clear();
			vt = VT_DBEMPTY;
		}
		}

		_db_variant_t(signed char cVal) throw()
		{ cValue = cVal; vt = VT_DBI1; }
		_db_variant_t(unsigned char ucVal) throw()
		{ cValue = ucVal; vt = VT_DBUI1; }
		_db_variant_t(signed short sVal) throw()
		{ sValue = sVal; vt = VT_DBI2; }
		_db_variant_t(unsigned short usVal) throw()
		{ sValue = usVal; vt = VT_DBUI2; }
		_db_variant_t(signed int nVal) throw()
		{
			lValue = nVal; vt = VT_DBI4;
		}
		_db_variant_t(unsigned int nVal) throw()
		{
			lValue = nVal; vt = VT_DBI4;
		}
		_db_variant_t(signed long lVal) throw()
		{ lValue = lVal;vt = VT_DBI4; }
		_db_variant_t(unsigned long ulVal) throw()
		{ lValue = ulVal; vt = VT_DBUI4; }
		_db_variant_t(Int64 llVal) throw()
		{ llValue = llVal; vt = VT_DBI8; }
		_db_variant_t(UInt64 ullVal) throw()
		{ llValue = ullVal; vt = VT_DBUI8; }
		_db_variant_t(float fVal) throw()
		{ fValue = fVal; vt= VT_DBSINGLE; }
		_db_variant_t(double dVal) throw()
		{ dValue = dVal; vt = VT_DBDOUBLE; }
		_db_variant_t(bool bVal) throw()
		{ bValue = bVal; vt = VT_DBBOOLEAN; };
		_db_variant_t(DBDATE date) throw()
		{
			memcpy(&dateValue,&date,sizeof(DBDATE));
			vt = VT_DBDATE;
		}
		_db_variant_t(DBTIME time) throw()
		{
			memcpy(&timeValue,&time,sizeof(DBTIME));
			vt = VT_DBTIME;
		}
		_db_variant_t(DBDATETIME dtVal) throw()
		{
			memcpy(&dtValue,&dtVal,sizeof(DBDATETIME));
			vt = VT_DBDATETIME;
		}
		_db_variant_t(const char* szVal) throw(CDBOutOfMemory,CDBBadParamException)
		{
			try
			{
				CopyToBSTR(szVal);
				vt = VT_DBBSTR;
			}
			catch(const CDBOutOfMemory& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(const CDBBadParamException& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(...)
			{
				Clear();
				vt = VT_DBEMPTY;
			}
		}
		_db_variant_t(unsigned char* dataBlob,int nLen) throw(CDBOutOfMemory,CDBBadParamException)
		{
			try
			{
				CopyToBLOB(dataBlob,nLen);
				vt = VT_DBBLOB;
			}
			catch(const CDBOutOfMemory& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(const CDBBadParamException& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(...)
			{
				Clear();
				vt = VT_DBEMPTY;
			}
		}
		_db_variant_t(DBGUID guidVal) throw()
		{
			memcpy(&guidValue,&guidVal,sizeof(DBGUID));
			vt = VT_DBGUID;
		}


		const _db_variant_t& operator = (const _db_variant_t& var) throw(CDBOutOfMemory)
		{
			try
			{
				if(this != &var)
				{
					switch(var.vt)
					{
					case VT_DBBSTR:
						CopyToBSTR(BSTR_TO_CCHAR(var.bstrValue));
						break;
					case VT_DBBLOB:
						CopyToBLOB(BLOB_TO_REALDATA(var.blobValue),BLOB_LENGTH(var.blobValue));
						break;
					default:
						memcpy(this,&var,sizeof(_db_variant_t));
						break;
					}
					vt = var.vt;
				}
			}
			catch(const CDBOutOfMemory& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(...)
			{
				Clear();
				vt = VT_DBEMPTY;
			}
			return *this;        
	        
		}

		const _db_variant_t& operator = (signed char cVal) throw()
		{
			Clear(); cValue = cVal; vt = VT_DBI1;
			return *this;
		}
		const _db_variant_t& operator = (unsigned char ucVal) throw()
		{
			Clear(); cValue = ucVal; vt = VT_DBUI1;
			return *this;
		}
		const _db_variant_t& operator = (signed short sVal) throw()
		{
			Clear(); sValue = sVal; vt = VT_DBI2;
			return *this;
		}
		const _db_variant_t& operator = (unsigned short usVal) throw()
		{
			Clear(); sValue = usVal; vt = VT_DBUI2;
			return *this;
		}
		const _db_variant_t& operator = (signed int nVal) throw()
		{
			Clear(); lValue = nVal; vt = VT_DBI4;
			return *this;
		}
		const _db_variant_t& operator = (unsigned int nVal) throw()
		{
			Clear(); lValue = nVal; vt = VT_DBI4;
			return *this;
		}
		const _db_variant_t& operator = (signed long lVal) throw()
		{
			Clear(); lValue = lVal; vt = VT_DBI4;
			return *this;
		}
		const _db_variant_t& operator = (unsigned long ulVal) throw()
		{
			Clear(); lValue = ulVal; vt = VT_DBUI4;
			return *this;
		}
		const _db_variant_t& operator = (Int64 llVal) throw()
		{
			Clear(); llValue = llVal; vt = VT_DBI8;
			return *this;
		}
		const _db_variant_t& operator = (UInt64 ullVal) throw()
		{
			Clear(); llValue = ullVal; vt = VT_DBUI8;
			return *this;
		}
		const _db_variant_t& operator = (float fVal) throw()
		{
			Clear(); fValue = fVal; vt = VT_DBSINGLE;
			return *this;
		}
		const _db_variant_t& operator = (double dVal) throw()
		{
			Clear(); dValue = dVal; vt = VT_DBDOUBLE;
			return *this;
		}
		const _db_variant_t& operator = (DBDATE date) throw()
		{
			Clear(); 
			memcpy(&dateValue,&date,sizeof(DBDATE));
			vt = VT_DBDATE;
			return *this;
		}
		const _db_variant_t& operator = (DBTIME time) throw()
		{
			Clear();
			memcpy(&timeValue,&time,sizeof(DBTIME));
			vt = VT_DBTIME;
			return *this;
		}
		const _db_variant_t& operator = (DBDATETIME dt) throw()
		{
			Clear();
			memcpy(&dtValue,&dt,sizeof(DBDATETIME));
			vt = VT_DBDATETIME;
			return *this;
		}
		const _db_variant_t& operator = (const char* szVal) throw(CDBOutOfMemory,CDBBadParamException)
		{
			try
			{
				CopyToBSTR(szVal);
				vt = VT_DBBSTR;
			}
			catch(const CDBOutOfMemory& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(const CDBBadParamException& e)
			{
				Clear();
				vt = VT_DBEMPTY;
				throw e;
			}
			catch(...)
			{
				Clear();
				vt = VT_DBEMPTY;
			}
			return *this;        
		}
	        
		~_db_variant_t()
		{ 
			Clear();
			vt = VT_DBEMPTY;
		}

		operator signed char() const throw(CDBBadConversion)
		{
			if(VT_DBI1 == vt) return cValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBI1,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.cValue;
		}
		operator unsigned char() const throw(CDBBadConversion)
		{
			if(VT_DBUI1 == vt) return (unsigned char)cValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBUI1,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return (unsigned char)(var.cValue);
		}
		operator signed short() const throw(CDBBadConversion)
		{
			if(VT_DBI2 == vt) return sValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBI2,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.sValue;
		}
		operator unsigned short() const throw(CDBBadConversion)
		{
			if(VT_DBUI2 == vt) return (unsigned short)sValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBUI2,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return (unsigned short)(var.sValue);
		}
		operator signed long() const throw(CDBBadConversion)
		{
			if(VT_DBI4 == vt) return lValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBI4,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.lValue;
		}
		operator unsigned long() const throw(CDBBadConversion)
		{
			if(VT_DBUI4 == vt) return (unsigned long)lValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBUI4,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return (unsigned long)(var.lValue);
		}

		operator signed int() const throw(CDBBadConversion)
		{
			return (signed int)(signed long)(*this);
		}

		operator unsigned int() const throw(CDBBadConversion)
		{
			return (unsigned int)(unsigned long)(*this);
		}
		operator Int64() const throw(CDBBadConversion)
		{
			if(VT_DBI8 == vt) return llValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBI8,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.llValue;
		}
		operator UInt64() const throw(CDBBadConversion)
		{
			if(VT_DBUI8 == vt) return (UInt64)llValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBUI8,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return (UInt64)(var.llValue);
		}
		operator float() const throw(CDBBadConversion)
		{
			if(VT_DBSINGLE == vt) return fValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBSINGLE,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	            
			return var.fValue;
		}
		operator double() const throw(CDBBadConversion)
		{
			if(VT_DBDOUBLE == vt) return dValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBDOUBLE,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.dValue;
		}
		operator bool() const throw(CDBBadConversion)
		{
			if(VT_DBBOOLEAN == vt) return bValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBBOOLEAN,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.bValue;
		}
		operator DBDATE() const throw(CDBBadConversion)
		{
			if(VT_DBDATE == vt) return dateValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBDATE,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.dateValue;
		}
		operator DBTIME() const throw(CDBBadConversion)
		{
			if(VT_DBTIME == vt) return timeValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBTIME,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.timeValue;
		}
		operator DBDATETIME() const throw(CDBBadConversion)
		{
			if(VT_DBDATETIME == vt) return dtValue;
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBDATETIME,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return var.dtValue;
		}
		operator string() const throw(CDBBadConversion)
		{
			if(VT_DBBSTR == vt) return BSTR_TO_CCHAR(bstrValue);
			_db_variant_t var;
			try
			{
				var.ChangeType(VT_DBBSTR,this);
			}
			catch(const CDBBadConversion& e)
			{
				throw e;
			}
	        
			return string(BSTR_TO_CCHAR(var.bstrValue));
		}

		void ChangeType(DBVARTYPE vtype,const _db_variant_t* pVar=NULL) throw(CDBBadConversion,CDBOutOfMemory)
		{
			if(NULL == pVar)   
			{
				pVar = this;
			}
			else
			{
				Clear();
			}
			if((pVar->vt==VT_DBNULL)||(pVar->vt==VT_DBEMPTY))
			{
				throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
			}
	        
			if((this!=pVar) || (vt!=pVar->vt))
			{
				switch(vtype)
				{
				case VT_DBI1:
				case VT_DBUI1:
					{
						switch(pVar->vt)
						{
						case VT_DBI1:
						case VT_DBUI1:
							cValue = pVar->cValue;
							break;
						case VT_DBI2:
						case VT_DBUI2:
							cValue = (signed char)(pVar->sValue);
							break;
						case VT_DBI4:
						case VT_DBUI4:
							cValue = (signed char)(pVar->lValue);
							break;
						case VT_DBI8:
						case VT_DBUI8:
							cValue = (signed char)(pVar->llValue);
							break;
						case VT_DBBOOLEAN:
							cValue = (signed char)(pVar->bValue?1:0);
							break;
						case VT_DBSINGLE:
							cValue = (signed char)(pVar->fValue);
							break;
						case VT_DBDOUBLE:
							cValue = (signed char)(pVar->dValue);
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								signed char c = ((signed char*)BSTR_TO_CCHAR(pVar->bstrValue))[0];
								Clear();
								cValue = c;
							}
							else
							{
								cValue = ((signed char*)BSTR_TO_CCHAR(pVar->bstrValue))[0];
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								signed char c = ((signed char*)BLOB_TO_REALDATA(pVar->blobValue))[0];
								Clear();
								cValue = c;
							}
							else
							{
								cValue = ((signed char*)BLOB_TO_REALDATA(pVar->blobValue))[0];
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBI2:
				case VT_DBUI2:
					{
						switch(pVar->vt)
						{
						case VT_DBI1:
						case VT_DBUI1:
							sValue = (signed short)(pVar->cValue);
							break;
						case VT_DBI2:
						case VT_DBUI2:
							sValue = pVar->sValue;
							break;
						case VT_DBI4:
						case VT_DBUI4:
							sValue = (signed short)(pVar->lValue);
							break;
						case VT_DBI8:
						case VT_DBUI8:
							sValue = (signed short)(pVar->llValue);
							break;
						case VT_DBSINGLE:
							sValue = (signed short)(pVar->fValue);
							break;
						case VT_DBDOUBLE:
							sValue = (signed short)(pVar->dValue);
							break;
						case VT_DBBOOLEAN:
							sValue = (signed short)(pVar->bValue?1:0);
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								signed short s = (signed short)strtol(BSTR_TO_CCHAR(pVar->bstrValue),NULL,10);
								Clear();
								sValue = s;
							}
							else
							{
								sValue = (signed short)strtol(BSTR_TO_CCHAR(pVar->bstrValue),NULL,10);
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								signed short s = ((signed short*)BLOB_TO_REALDATA(pVar->blobValue))[0];
								Clear();
								sValue = s;
							}
							else
							{
								sValue = ((signed short*)BLOB_TO_REALDATA(pVar->blobValue))[0];
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBI4:
				case VT_DBUI4:
					{
						switch(pVar->vt)
						{
						case VT_DBI1:
						case VT_DBUI1:
							lValue = (signed long)(pVar->cValue);
							break;
						case VT_DBI2:
						case VT_DBUI2:
							lValue = (signed long)(pVar->sValue);
							break;
						case VT_DBI4:
						case VT_DBUI4:
							lValue = pVar->lValue;
							break;
						case VT_DBI8:
						case VT_DBUI8:
							lValue = (signed long)(pVar->llValue);
							break;
						case VT_DBSINGLE:
							lValue = (signed long)(pVar->fValue);
							break;
						case VT_DBDOUBLE:
							lValue = (signed long)(pVar->dValue);
							break;
						case VT_DBBOOLEAN:
							lValue = pVar->bValue?1:0;
							break;
						case VT_DBBSTR:
							if(pVar == this)
							{
								signed long l= strtol(BSTR_TO_CCHAR(pVar->bstrValue),NULL,10);
								Clear();
								lValue = l;
							}
							else
							{
								lValue = strtol(BSTR_TO_CCHAR(pVar->bstrValue),NULL,10);
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								signed long l = ((signed long*)BLOB_TO_REALDATA(pVar->blobValue))[0];
								Clear();
								lValue = l;
							}
							else
							{
								lValue = ((signed long*)BLOB_TO_REALDATA(pVar->blobValue))[0];
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBI8:
				case VT_DBUI8:
					{
						switch(pVar->vt)
						{
						case VT_DBI1:
						case VT_DBUI1:
							llValue = (Int64)(pVar->cValue);
							break;
						case VT_DBI2:
						case VT_DBUI2:
							llValue = (Int64)(pVar->sValue);
							break;
						case VT_DBI4:
						case VT_DBUI4:
							llValue = (Int64)(pVar->lValue);
							break;
						case VT_DBI8:
						case VT_DBUI8:
							llValue = pVar->llValue;
							break;
						case VT_DBSINGLE:
							llValue = (Int64)(pVar->fValue);
							break;
						case VT_DBDOUBLE:
							llValue = (Int64)(pVar->dValue);
							break;
						case VT_DBBOOLEAN:
							llValue = pVar->bValue?1:0;
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								Int64 ll = strtoll(BSTR_TO_CCHAR(pVar->bstrValue),NULL,10);
								Clear();
								llValue = ll;
							}
							else
							{
								llValue = strtoll(BSTR_TO_CCHAR(pVar->bstrValue),NULL,10);
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								Int64 ll = ((Int64*)BLOB_TO_REALDATA(pVar->blobValue))[0];
								Clear();
								llValue = ll;
							}
							else
							{
								llValue = ((Int64*)BLOB_TO_REALDATA(pVar->blobValue))[0];
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBSINGLE:
					{
						switch(pVar->vt)
						{
						case VT_DBUI1:
						case VT_DBI1:
							fValue = (float)(pVar->cValue);
							break;
						case VT_DBUI2:
						case VT_DBI2:
							fValue = (float)(pVar->sValue);
							break;
						case VT_DBUI4:
						case VT_DBI4:
							fValue = (float)(pVar->lValue);
							break;
						case VT_DBUI8:
						case VT_DBI8:
							fValue = (float)(pVar->llValue);
							break;
						case VT_DBSINGLE:
							fValue = pVar->fValue;
							break;
						case VT_DBDOUBLE:
							fValue = (float)(pVar->dValue);
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								float f = (float)strtod(BSTR_TO_CCHAR(pVar->bstrValue),NULL);
								Clear();
								fValue = f;
							}
							else
							{
								fValue = (float)strtod(BSTR_TO_CCHAR(pVar->bstrValue),NULL);
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								float f = ((float*)BLOB_TO_CREALDATA(pVar->blobValue))[0];
								Clear();
								fValue = f;
							}
							else
							{
								fValue = ((float*)BLOB_TO_CREALDATA(pVar->blobValue))[0];
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBDOUBLE:
					{
						switch(pVar->vt)
						{
						case VT_DBUI1:
						case VT_DBI1:
							dValue = (double)(pVar->cValue);
							break;
						case VT_DBUI2:
						case VT_DBI2:
							dValue = (double)(pVar->sValue);
							break;
						case VT_DBUI4:
						case VT_DBI4:
							dValue = (double)(pVar->lValue);
							break;
						case VT_DBUI8:
						case VT_DBI8:
							dValue = (double)(pVar->llValue);
							break;
						case VT_DBSINGLE:
							dValue = (double)(pVar->fValue);
							break;
						case VT_DBDOUBLE:
							dValue = pVar->dValue;
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								double d = strtod(BSTR_TO_CCHAR(pVar->bstrValue),NULL);
								Clear();
								dValue = d;
							}
							else
							{
								dValue = strtod(BSTR_TO_CCHAR(pVar->bstrValue),NULL);
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								double d = ((double*)BLOB_TO_CREALDATA(pVar->blobValue))[0];
								Clear();
								dValue = d;
							}
							else
							{
								dValue = ((double*)BLOB_TO_CREALDATA(pVar->blobValue))[0];
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBDATE:
					{
						switch(pVar->vt)
						{
						case VT_DBDATE:
							memcpy(&dateValue,&(pVar->dateValue),sizeof(DBDATE));
							break;
						case VT_DBDATETIME:
							if(this == pVar)
							{
								DBDATETIME dt;
								memcpy(&dt,&(pVar->dtValue),sizeof(DBDATETIME));
								dateValue.year = dt.year;
								dateValue.month = dt.month;
								dateValue.day = dt.day;
							}
							else
							{
								dateValue.year = pVar->dtValue.year;
								dateValue.month = pVar->dtValue.month;
								dateValue.day = pVar->dtValue.day;
							}
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								DBDATE date = StrToDate(BSTR_TO_CCHAR(pVar->bstrValue));
								Clear();
								memcpy(&dateValue,&date,sizeof(DBDATE));
							}
							else
							{
								dateValue = StrToDate(BSTR_TO_CCHAR(pVar->bstrValue));
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								DBDATE date = BlobToDate(pVar->blobValue);
								Clear();
								memcpy(&dateValue,&date,sizeof(DBDATE));
							}
							else
							{
								dateValue = BlobToDate(pVar->blobValue);
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBTIME:
					{
						switch(pVar->vt)
						{
						case VT_DBTIME:
							memcpy(&timeValue,&(pVar->timeValue),sizeof(DBTIME));
							break;
						case VT_DBDATETIME:
							if(this == pVar)
							{
								DBDATETIME dt;
								memcpy(&dt,&(pVar->dtValue),sizeof(DBDATETIME));
								timeValue.hour = dt.hour;
								timeValue.minute = dt.minute;
								timeValue.second = dt.second;
							}
							else
							{
								timeValue.hour = pVar->dtValue.hour;
								timeValue.minute = pVar->dtValue.minute;
								timeValue.second = pVar->dtValue.second;
							}                         
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								DBTIME time = StrToTime(BSTR_TO_CCHAR(pVar->bstrValue));
								Clear();
								memcpy(&timeValue,&time,sizeof(DBTIME));
							}
							else
							{
								timeValue = StrToTime(BSTR_TO_CCHAR(pVar->bstrValue));
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								DBTIME time = BlobToTime(pVar->blobValue);
								Clear();
								memcpy(&timeValue,&time,sizeof(DBTIME));
							}
							else
							{
								timeValue = BlobToTime(pVar->blobValue);
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBDATETIME:
					{
						switch(pVar->vt)
						{
						case VT_DBDATE:
							if(this == pVar)
							{
								DBDATE date;
								memcpy(&date,&(pVar->dateValue),sizeof(DBDATE));
								memset(&dtValue,0,sizeof(DBDATETIME));
								dtValue.year = date.year;
								dtValue.month = date.month;
								dtValue.day  = date.day;
							}
							else
							{
								memset(&dtValue,0,sizeof(DBDATETIME));
								dtValue.year = pVar->dateValue.year;
								dtValue.month = pVar->dateValue.month;
								dtValue.day  = pVar->dateValue.day;
							}
							break;
						case VT_DBTIME:
							{
								if(this == pVar)
								{
									DBTIME time;
									memcpy(&time,&(pVar->timeValue),sizeof(DBTIME));
									memset(&dtValue,0,sizeof(DBDATETIME));
									dtValue.hour = time.hour;
									dtValue.minute = time.minute;
									dtValue.second  = time.second;
								}
								else
								{
									memset(&dtValue,0,sizeof(DBDATETIME));
									dtValue.hour = pVar->timeValue.hour;
									dtValue.minute = pVar->timeValue.minute;
									dtValue.second  = pVar->timeValue.second;
								}
								time_t t = time(NULL);
								tm* ptm = localtime(&t);
								//日期设为当前系统日期
								dtValue.year = (unsigned short)(ptm->tm_year+1900);
								dtValue.month = (unsigned char)(ptm->tm_mon+1);
								dtValue.day = (unsigned char)(ptm->tm_mday);
	                            
							}
							break;
						case VT_DBDATETIME:
							memcpy(&dtValue,&(pVar->dtValue),sizeof(DBDATETIME));
							break;
						case VT_DBBSTR:
							if(this == pVar)
							{
								DBDATETIME dt = StrToDateTime(BSTR_TO_CCHAR(pVar->bstrValue));
								Clear();
								memcpy(&dtValue,&dt,sizeof(DBDATETIME));
							}
							else
							{
								dtValue = StrToDateTime(BSTR_TO_CCHAR(pVar->bstrValue));
							}
							break;
						case VT_DBBLOB:
							if(this == pVar)
							{
								DBDATETIME dt = BlobToDateTime(pVar->blobValue);
								Clear();
								memcpy(&dtValue,&dt,sizeof(DBDATETIME));
							}
							else
							{
								dtValue = BlobToDateTime(pVar->blobValue);
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBGUID:
					{
						switch(pVar->vt)
						{
						case VT_DBGUID:
							memcpy(&guidValue,&(pVar->guidValue),sizeof(DBGUID));
							break;
						case VT_DBBSTR:
							if(pVar == this)
							{
								DBGUID guid = StrToGUID(BSTR_TO_CCHAR(pVar->bstrValue));
								Clear();
								memcpy(&guidValue,&guid,sizeof(DBGUID));
							}
							else
							{
								guidValue = StrToGUID(BSTR_TO_CCHAR(pVar->bstrValue));
							}
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
	                
				case VT_DBBSTR:
					{
						switch(pVar->vt)
						{
							char s[256];
						case VT_DBI1:
						case VT_DBUI1:
							sprintf(s,"%c",pVar->cValue);
							CopyToBSTR(s);
							break;
						case VT_DBBOOLEAN:
							sprintf(s,"%d",pVar->bValue?1:0);
							CopyToBSTR(s);
							break;
						case VT_DBI2:
						case VT_DBUI2:
							sprintf(s,"%hd",pVar->sValue);
							CopyToBSTR(s);
							break;
						case VT_DBI4:
						case VT_DBUI4:
							sprintf(s,"%ld",pVar->lValue);
							CopyToBSTR(s);
							break;
						case VT_DBI8:
						case VT_DBUI8:
	#ifdef OS_IS_MSWINDOWS
							sprintf(s,"%I64d",pVar->llValue);
	#else
							sprintf(s,"%lld",pVar->llValue);
	#endif
							CopyToBSTR(s);
							break;
						case VT_DBSINGLE:
							sprintf(s,"%f",pVar->fValue);
							CopyToBSTR(s);
							break;
						case VT_DBDOUBLE:
							sprintf(s,"%f",pVar->dValue);
							CopyToBSTR(s);
							break;
						case VT_DBDATE:
							sprintf(s,"%d-%02d-%02d",
								pVar->dateValue.year,pVar->dateValue.month,
								pVar->dateValue.day);
							CopyToBSTR(s);
							break;
						case VT_DBTIME:
							sprintf(s,"%02d:%02d:%02d",
								pVar->timeValue.hour,pVar->timeValue.minute,
								pVar->timeValue.second);
							CopyToBSTR(s);
							break;
						case VT_DBDATETIME:
							sprintf(s,"%d-%02d-%02d %02d:%02d:%02d",
								pVar->dtValue.year,pVar->dtValue.month,pVar->dtValue.day,
								pVar->dtValue.hour,pVar->dtValue.minute,pVar->dtValue.second);
							CopyToBSTR(s);
							break;
						case VT_DBGUID:
							sprintf(s,"%08lX-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X",
								pVar->guidValue.Data1,pVar->guidValue.Data2,
								pVar->guidValue.Data3,pVar->guidValue.Data4,
								pVar->guidValue.Data[0],pVar->guidValue.Data[1],
								pVar->guidValue.Data[2],pVar->guidValue.Data[3],
								pVar->guidValue.Data[4],pVar->guidValue.Data[5]);
							CopyToBSTR(s);
							break;
						case VT_DBBSTR:
							CopyToBSTR(BSTR_TO_CCHAR(pVar->bstrValue));
							break;
						case VT_DBBLOB:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBBLOB:
					{
						switch(pVar->vt)
						{
						case VT_DBI1:
						case VT_DBUI1:
							CopyToBLOB((unsigned char*)&(pVar->cValue),sizeof(pVar->cValue));
							break;
						case VT_DBBOOLEAN:
							CopyToBLOB((unsigned char*)&(pVar->bValue),sizeof(pVar->bValue));
							break;
						case VT_DBI2:
						case VT_DBUI2:
							CopyToBLOB((unsigned char*)&(pVar->sValue),sizeof(pVar->sValue));
							break;
						case VT_DBI4:
						case VT_DBUI4:
							CopyToBLOB((unsigned char*)&(pVar->lValue),sizeof(pVar->lValue));
							break;
						case VT_DBI8:
						case VT_DBUI8:
							CopyToBLOB((unsigned char*)&(pVar->llValue),sizeof(pVar->llValue));
							break;
						case VT_DBSINGLE:
							CopyToBLOB((unsigned char*)&(pVar->fValue),sizeof(pVar->fValue));
							break;
						case VT_DBDOUBLE:
							CopyToBLOB((unsigned char*)&(pVar->fValue),sizeof(pVar->dValue));
							break;
						case VT_DBDATE:
							{
								unsigned char data[8];
								data[0] = (unsigned char)((pVar->dateValue.year)/1000);
								data[1] = (unsigned char)((pVar->dateValue.year%1000)/100);
								data[2] = (unsigned char)((pVar->dateValue.year%100)/10);
								data[3] = (unsigned char)((pVar->dateValue.year)%10);
								data[4] = (unsigned char)((pVar->dateValue.month)/10);
								data[5] = (unsigned char)((pVar->dateValue.month)%10);
								data[6] = (unsigned char)((pVar->dateValue.day)/10);
								data[7] = (unsigned char)((pVar->dateValue.day)%10);
								CopyToBLOB(data,8);
							}
							break;
						case VT_DBTIME:
							{
								unsigned char data[6];
								data[0] = (unsigned char)(pVar->timeValue.hour/10);
								data[1] = (unsigned char)(pVar->timeValue.hour%10);
								data[2] = (unsigned char)(pVar->timeValue.minute/10);
								data[3] = (unsigned char)(pVar->timeValue.minute%10);
								data[4] = (unsigned char)(pVar->timeValue.second/10);
								data[5] = (unsigned char)(pVar->timeValue.second%10);
								CopyToBLOB(data,6);
							}
							break;
						case VT_DBDATETIME:
							{
								unsigned char data[14];
								data[0] = (unsigned char)((pVar->dtValue.year)/1000);
								data[1] = (unsigned char)((pVar->dtValue.year%1000)/100);
								data[2] = (unsigned char)((pVar->dtValue.year%100)/10);
								data[3] = (unsigned char)((pVar->dtValue.year)%10);
								data[4] = (unsigned char)((pVar->dtValue.month)/10);
								data[5] = (unsigned char)((pVar->dtValue.month)%10);
								data[6] = (unsigned char)((pVar->dtValue.day)/10);
								data[7] = (unsigned char)((pVar->dtValue.day)%10);
								data[8] = (unsigned char)((pVar->dtValue.hour)/10);
								data[9] = (unsigned char)((pVar->dtValue.hour)%10);
								data[10] = (unsigned char)((pVar->dtValue.minute)/10);
								data[11] = (unsigned char)((pVar->dtValue.minute)%10);
								data[12] = (unsigned char)((pVar->dtValue.second)/10);
								data[13] = (unsigned char)((pVar->dtValue.second)%10);
								CopyToBLOB(data,14);
							}
							break;
						case VT_DBBLOB:
							CopyToBLOB(BLOB_TO_REALDATA(pVar->blobValue),BLOB_LENGTH(pVar->blobValue));
							break;
						case VT_DBBSTR:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
	                    
					}
					break;
				case VT_DBBOOLEAN:
					{
						switch(pVar->vt)
						{
						case VT_DBI1:
						case VT_DBUI1:
							bValue = pVar->cValue!=0;
							break;
						case VT_DBI2:
						case VT_DBUI2:
							bValue = pVar->sValue!=0;
							break;
						case VT_DBI4:
						case VT_DBUI4:
							bValue = pVar->lValue!=0;
							break;
						case VT_DBI8:
						case VT_DBUI8:
							bValue = pVar->llValue!=0;
							break;
						case VT_DBSINGLE:
							bValue = pVar->fValue!=0;
							break;
						case VT_DBDOUBLE:
							bValue = pVar->dValue!=0;
							break;
						case VT_DBBOOLEAN:
							bValue = pVar->bValue;
							break;
						case VT_DBDATE:
							bValue = (pVar->dateValue.year!=0) || (pVar->dateValue.month!=0) ||
									(pVar->dateValue.day!=0);
							break;
						case VT_DBTIME:
							bValue = (pVar->timeValue.hour!=0) || (pVar->timeValue.minute!=0) ||
									(pVar->timeValue.second!=0);
							break;
						case VT_DBDATETIME:
							bValue = (pVar->dtValue.year!=0) || (pVar->dtValue.month!=0) ||
									(pVar->dtValue.day!=0) || (pVar->dtValue.hour!=0) ||
									(pVar->dtValue.minute!=0) ||(pVar->dtValue.second!=0);
							break;
						case VT_DBGUID:
							bValue = (pVar->guidValue.Data1!=0) || (pVar->guidValue.Data2!=0) ||
									(pVar->guidValue.Data3!=0) || (pVar->guidValue.Data4!=0) ||
									(pVar->guidValue.Data[0]!=0) || (pVar->guidValue.Data[1]!=0) ||
									(pVar->guidValue.Data[2]!=0) || (pVar->guidValue.Data[3]!=0) ||
									(pVar->guidValue.Data[4]!=0) || (pVar->guidValue.Data[5]!=0);
							break;
						case VT_DBBSTR:
							bValue = pVar->bstrValue!=NULL;
							break;
						case VT_DBBLOB:
							bValue = pVar->blobValue!=NULL;
							break;
						default:
							throw CDBBadConversion(UD_ERROR_INVALID_CONVERSION);
							break;
						}
					}
					break;
				case VT_DBEMPTY:
				case VT_DBNULL:
					Clear();
					break;
				default:
					assert(false);
					break;
				}
				vt = vtype;
			}
	        
		}
	private:
		void Clear()
		{
			if((VT_DBBSTR==vt) || (VT_DBBLOB==vt))
			{
				if(bstrValue)
				{
					delete []bstrValue;
					bstrValue = NULL;
				}
			}
			vt = VT_DBEMPTY;
		}
		void CopyToBSTR(const char* szVal) throw(CDBOutOfMemory,CDBBadParamException)
		{
			Clear();
			if(NULL == szVal)
			{
				vt = VT_DBEMPTY;
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER);
			}
			int nLen = strlen(szVal);
			bstrValue = new char[sizeof(int)+nLen+1]; //长度位+字符串数据+'\0'（其中长度位的长度值不包括'\0')
			if(NULL == bstrValue)
			{
				vt = VT_DBEMPTY;
				throw CDBOutOfMemory(UD_ERROR_OUT_OF_MEMORY);
			}
			*(int*)bstrValue = nLen; //设置长度位
			memcpy(bstrValue+sizeof(int),szVal,nLen); //拷贝字符串
			bstrValue[nLen+sizeof(int)] = '\0'; //加入字符终止符
			vt = VT_DBBSTR;
		}
		void CopyToBLOB(const unsigned char* data,int nLen) throw(CDBOutOfMemory,CDBBadParamException)
		{
			Clear();
			if((NULL == data) || (nLen <=0))
			{
				vt = VT_DBEMPTY;
				throw CDBBadParamException(UD_ERROR_INVALID_PARAMETER);
			}
			blobValue = new unsigned char[sizeof(int)+nLen];//长度位+数据
			if(NULL == blobValue)
			{
				vt = VT_DBEMPTY;
				throw CDBOutOfMemory(UD_ERROR_OUT_OF_MEMORY);
			}
			*(int*)blobValue = nLen; //设置长度位
			memcpy(blobValue+sizeof(int),data,nLen);//拷贝数据
			vt = VT_DBBLOB;
		}
	};
};

#endif
