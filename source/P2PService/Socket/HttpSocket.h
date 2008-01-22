#if !defined(HttpSocket_H)
#define HttpSocket_H


#include "SocketHandler.h"
#include "HttpRequest.h"



/////////////////////////////////////////////////////////////////////////////////////
// HttpSocket
//
// Purpose:

class HttpSocket 
	public SocketHandler
{
    string method = "GET";

	stringstream response;

    // last response code
    int responseCode = -1;

    // responce message
    stringstream responseMessage;

    // shows if redirects allowed
    bool _allowRedirects;


    HttpSocket () :
		SocketHandler (),
		
		_responseCode(-1),
		_responseMsg(""),
		_allowRedirects(true),
	{}
    
    void setAllowRedirects ( bool set ) 
	{
		_allowRedirects = set;
    }

    bool allowRedirects() 
	{
		return _allowRedirects;
    }

    string & getRequestCmd () 
	{
		return method;
    }
    
    /**
     * Gets HTTP response status.  From responses like:
     * <PRE>
     * HTTP/1.0 200 OK
     * HTTP/1.0 401 Unauthorized
     * </PRE>
     * Extracts the ints 200 and 401 respectively.
     * Returns -1 if none can be discerned
     * from the response (i.e., the response is not valid HTTP).
     * @throws IOException if an error occurred connecting to the server.
     */
    int getResponseCode () 
	{
		if ( _responseCode != -1 ) 
		{
			return _responseCode;
		}

		// make sure we've gotten the headers
		getInputStream();

		string resp = getHeader(0);

		/* should have no leading/trailing LWS
		 * expedite the typical case by assuming it has
		 * form "HTTP/1.x <WS> 2XX <mumble>"
		 */
		int ind;
		try 
		{	
			ind = resp.indexOf(' ');
			while(resp.charAt(ind) == ' ')
				ind++;
			responseCode = Integer.parseInt(resp.substring(ind, ind + 3));
			responseMessage = resp.substring(ind + 4).trim();
			return responseCode;
		} 
		catch (Exception e) 
		{ 
			return responseCode;
		}
    }

    /**
     * Gets the HTTP response message, if any, returned along with the
     * response code from a server.  From responses like:
     * <PRE>
     * HTTP/1.0 200 OK
     * HTTP/1.0 404 Not Found
     * </PRE>
     * Extracts the Strings "OK" and "Not Found" respectively.
     * Returns null if none could be discerned from the responses 
     * (the result was not valid HTTP).
     * @throws IOException if an error occurred connecting to the server.
     */
    string getResponseMessage ()
	{
		getResponseCode();
		return responseMessage;
    }




};


#endif