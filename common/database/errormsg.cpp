/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 *				PURPOSE		: define error messages used in general db interface
 */


#include "errormsg.h"

namespace DB_TOOLKIT
{

	ERROR_MSG   g_ErrorMsg[] = 
	{
		{ UD_ERROR_NOT_IMPLEMENT,		    "Not implement yet." },
		{ UD_ERROR_CONN_CONNECTIONSTRING,   "Connection:Connection String is invalid." },
		{ UD_ERROR_INVALID_COMMAND_OBJ,     "Invalid command object." },
		{ UD_ERROR_INVALID_CONNECTION_OBJ,	"Invalid connection object." },
		{ UD_ERROR_INVALID_RECORDSET_OBJ,   "Invalid recordset object." },
		{ UD_ERROR_INVALID_ROW_OBJ,			"Invalid row object." },
		{ UD_ERROR_INVALID_PARAMETER_OBJ,   "Invalid parameter object." },
		{ UD_ERROR_NO_ACTIVE_CONN,			"No active connection." },
		{ UD_ERROR_RST_EOF,					"The current pos of recordset is EOF." },
		{ UD_ERROR_RST_BOF,					"The current pos of recordset is BOF." },
		{ UD_ERROR_INVALID_PARAMETER,		"Invalid Parameter." },
		{ UD_ERROR_INVALID_PARAM_COLLECTION_OBJ,"Invalid parameter collection object." },
		{ UD_ERROR_OUT_OF_MEMORY,			"Out of memory." },
		{ UD_ERROR_INVALID_CONVERSION,		"Invalid conversion." },
		{ UD_ERROR_FIELD_VALUE_NULL,		"The field value is NULL" }
	};
};
