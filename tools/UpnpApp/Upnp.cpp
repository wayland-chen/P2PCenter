#include "stdafx.h"

#include "upnp.h"

#define UPNPPORTMAP0   _T("WANIPConnection")
#define UPNPPORTMAP1   _T("WANPPPConnection")
#define UPNPGETEXTERNALIP _T("GetExternalIPAddress"),_T("NewExternalIPAddress")
#define UPNPADDPORTMAP _T("AddPortMapping")
#define UPNPDELPORTMAP _T("DeletePortMapping")

static const ulong UPNPADDR = 0xFAFFFFEF;
static const int UPNPPORT = 1900;
static const CString URNPREFIX = _T("urn:schemas-upnp-org:");

const CString getString(int i)
{
	CString s;

	s.Format(_T("%d"), i);

	return s;
}

const CString GetArgString(const CString& name, const CString& value)
{
	return _T("<") + name + _T(">") + value + _T("</") + name + _T(">");
}

const CString GetArgString(const CString& name, int value)
{
	return _T("<") + name + _T(">") + getString(value) + _T("</") + name + _T(">");
}

bool SOAP_action(CString addr, WORD port, const CString request, CString &response)
{
	char buffer[10240];

	const CStringA sa(request);
	int length = sa.GetLength();
	strcpy(buffer, (const char*)sa);

	DWORD ip = inet_addr(CStringA(addr));
	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.S_un.S_addr = ip;
	int s = socket(AF_INET, SOCK_STREAM, 0);
	u_long lv = 1;
	ioctlsocket(s, FIONBIO, &lv);
	connect(s, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	Sleep(50);
	/*int n =*/ send(s, buffer, length, 0);
	Sleep(500);
	int rlen = recv(s, buffer, sizeof(buffer), 0);
	closesocket(s);
	if (rlen == SOCKET_ERROR) return false;
	if (!rlen) return false;

	response = CString(CStringA(buffer, rlen));

	return true;
}

int SSDP_sendRequest(int s, DWORD ip, WORD port, const CString& request)
{
	char buffer[10240];

	const CStringA sa(request);
	int length = sa.GetLength();
	strcpy(buffer, (const char*)sa);

	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.S_un.S_addr = ip;

	return sendto(s, buffer, length, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
}

bool parseHTTPResponse(const CString& response, CString& result)
{
	int pos = 0;

	CString status = response.Tokenize(_T("\r\n"), pos);

	result = response;
	result.Delete(0, pos);

	pos = 0;
	status.Tokenize(_T(" "), pos);
	status = status.Tokenize(_T(" "), pos);
	if (status.IsEmpty() || status[0]!='2') return false;
	return true;
}

const CString getProperty(const CString& all, const CString& name)
{
	CString startTag = '<' + name + '>';
	CString endTag = _T("</") + name + '>';
	CString property;

	int posStart = all.Find(startTag);
	if (posStart<0) return CString();

	int posEnd = all.Find(endTag, posStart);
	if (posStart>=posEnd) return CString();

	return all.Mid(posStart + startTag.GetLength(), posEnd - posStart - startTag.GetLength());
}

MyUPnP::MyUPnP()
: m_version(1)
{
	m_uLocalIP = 0;
	isSearched = false;
}

MyUPnP::~MyUPnP()
{
	UPNPNAT_MAPPING search;
	POSITION pos = m_Mappings.GetHeadPosition();
	while(pos){
		search = m_Mappings.GetNext(pos);
		RemoveNATPortMapping(search, false);
	}

	m_Mappings.RemoveAll();
}


bool MyUPnP::InternalSearch(int version)
{
	if(version<=0)version = 1;
	m_version = version;

#define NUMBEROFDEVICES 2
	CString devices[][2] = {
		{UPNPPORTMAP1, _T("service")},
		{UPNPPORTMAP0, _T("service")},
		{_T("InternetGatewayDevice"), _T("device")},
	};

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	u_long lv = 1;
	ioctlsocket(s, FIONBIO, &lv);

	int rlen = 0;
	for (int i=0; rlen<=0 && i<500; i++) {
		if (!(i%100)) {
			for (int i=0; i<NUMBEROFDEVICES; i++) {
				m_name.Format(_T("%s%s:%s:%d"), URNPREFIX, devices[i][1], devices[i][0], version);
				CString request;
				request.Format(_T("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: %d\r\nST: %s\r\n\r\n"),
					6, m_name);
				SSDP_sendRequest(s, UPNPADDR, UPNPPORT, request);
			}
		}

		Sleep(10);

		char buffer[10240];
		rlen = recv(s, buffer, sizeof(buffer), 0);
		if (rlen <= 0) continue;
		closesocket(s);

		CString response = CString(CStringA(buffer, rlen));
		CString result;
		if (!parseHTTPResponse(response, result)) return false;

		for (int d=0; d<NUMBEROFDEVICES; d++) {
			m_name.Format(_T("%s%s:%s:%d"), URNPREFIX, devices[d][1], devices[d][0], version);
			if (result.Find(m_name) >= 0) {
				for (int pos = 0;;) {
					CString line = result.Tokenize(_T("\r\n"), pos);
					if (line.IsEmpty()) return false;
					CString name = line.Mid(0, 9);
					name.MakeUpper();
					if (name == _T("LOCATION:")) {
						line.Delete(0, 9);
						m_description = line;
						m_description.Trim();
						return GetDescription();
					}
				}
			}
		}
	}
	closesocket(s);

	return false;
}

bool MyUPnP::Search(int version)
{
	if (isSearched) return isComplete();

	isSearched = true;

	return InternalSearch(version);
}

static CString NGetAddressFromUrl(const CString& str, CString& post, CString& host, int& port)
{
	CString s = str;

	post = _T("");
	host = post;
	port = 0;
	int pos = s.Find(_T("://"));
	if (!pos) return CString();
	s.Delete(0, pos + 3);

	pos = s.Find('/');
	if (!pos) {
		host = s;
		s = _T("");
	} else {
		host = s.Mid(0, pos);
		s.Delete(0, pos);
	}

	if (s.IsEmpty()) {
		post = _T("");
	} else {
		post = s;
	}

	pos = 0;
	CString addr = host.Tokenize(_T(":"), pos);
	s = host.Tokenize(_T(":"), pos);
	if (s.IsEmpty()) {
		port = 80;
	} else {
		port = _tstoi(s);
	}

	return addr;
}

bool MyUPnP::GetDescription()
{
	if(!Valid())return false;
	CString post, host, addr;
	int port = 0;
	addr = NGetAddressFromUrl(m_description, post, host, port);
	if(addr.IsEmpty())return false;
	CString request = CString(_T("GET ")) + post + _T(" HTTP/1.1\r\nHOST: ") + host + _T("\r\nACCEPT-LANGUAGE: en\r\n\r\n");
	CString response;
	if (!SOAP_action(addr, (WORD)port, request, response)) return false;
	CString result;
	if (!parseHTTPResponse(response, result)) return false;

	m_friendlyname = getProperty(result, _T("friendlyName"));
	m_modelname = getProperty(result, _T("modelName"));
	m_baseurl = getProperty(result, _T("URLBase"));
	if(m_baseurl.IsEmpty())m_baseurl = CString(_T("http://")) + host + _T("/");
	if(m_baseurl[m_baseurl.GetLength() - 1]!='/')m_baseurl += _T("/");

	CString serviceType = _T("<serviceType>") + m_name + _T("</serviceType>");
	int pos = result.Find(serviceType);
	if (pos >= 0) {
		result.Delete(0, pos + serviceType.GetLength());
		pos = result.Find(_T("</service>"));
		if (pos >= 0) {
			result = result.Mid(0, pos);
			m_controlurl = getProperty(result, _T("controlURL"));
			if (!m_controlurl.IsEmpty() && m_controlurl[0] == '/') {
				m_controlurl = m_baseurl + m_controlurl.Mid(1);
			}
		}
	}

	return isComplete();
}

CString MyUPnP::GetProperty(const CString& name, CString& response)
{
	if (!isComplete())return CString();
	CString post, host, addr;
	int port = 0;
	addr = NGetAddressFromUrl(m_controlurl, post, host, port);
	if(addr.IsEmpty())return CString();
	CString cnt;
	CString psr;
	cnt.Append(_T("<s:Envelope\r\n    xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\n    "));
	cnt.Append(_T("s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n  <s:Body>\r\n    <u:"));
	cnt.Append(name);
	cnt.Append(_T(" xmlns:u=\""));
	cnt.Append(m_name);
	cnt.Append(_T("\">\r\n    </u:"));
	cnt.Append(name);
	cnt.Append(_T(">\r\n  </s:Body>\r\n</s:Envelope>\r\n\r\n"));
	psr.Append(_T("POST "));
	psr.Append(post);
	psr.Append(_T(" HTTP/1.1\r\nHOST: "));
	psr.Append(host);
	psr.Append(_T("\r\nContent-Length: "));
	psr.Append(getString(CStringA(cnt).GetLength()));
	psr.Append(_T("\r\nContent-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \""));
	psr.Append(m_name);
	psr.Append(_T("#"));
	psr.Append(name);
	psr.Append(_T("\"\r\n\r\n"));
	psr.Append(cnt);

	CString request = psr;
	if (!SOAP_action(addr, (WORD)port, request, response)) return CString();
	CString result;
	if (!parseHTTPResponse(response, result)) return CString();

	return getProperty(result, response);
}

bool MyUPnP::InvokeCommand(const CString& name, const CString& args)
{
	if(!isComplete())return false;
	CString post, host, addr;
	int port = 0;
	addr = NGetAddressFromUrl(m_controlurl, post, host, port);
	if(addr.IsEmpty())return false;
	CString cnt;
	CString psr;
	cnt.Append(_T("<?xml version=\"1.0\"?><s:Envelope\r\n    xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\n    "));
	cnt.Append(_T("s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n  <s:Body>\r\n    <u:"));
	cnt.Append(name);
	cnt.Append(_T(" xmlns:u=\""));
	cnt.Append(m_name);
	cnt.Append(_T("\">\r\n"));
	cnt.Append(args);
	cnt.Append(_T("    </u:"));
	cnt.Append(name);
	cnt.Append(_T(">\r\n  </s:Body>\r\n</s:Envelope>\r\n\r\n"));
	psr.Append(_T("POST "));
	psr.Append(post);
	psr.Append(_T(" HTTP/1.1\r\nHOST: "));
	psr.Append(host);
	psr.Append(_T("\r\nContent-Length: "));
	psr.Append(getString(CStringA(cnt).GetLength()));
	psr.Append(_T("\r\nContent-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \""));
	psr.Append(m_name);
	psr.Append(_T("#"));
	psr.Append(name);
	psr.Append(_T("\"\r\n\r\n"));
	psr.Append(cnt);

	CString response;
	CString request = psr;
	if (!SOAP_action(addr, (WORD)port, request, response)) return false;
	CString result;
	if (!parseHTTPResponse(response, result)) return false;

	return true;
}

bool MyUPnP::addPortmap(int eport, int iport, const CString& iclient, const CString& descri, const CString& type)
{
	CString args;

	args.Empty();
	args.Append(GetArgString(_T("NewRemoteHost"), _T("")));
	args.Append(GetArgString(_T("NewExternalPort"), eport));
	args.Append(GetArgString(_T("NewProtocol"), type));
	args.Append(GetArgString(_T("NewInternalPort"), iport));
	args.Append(GetArgString(_T("NewInternalClient"), iclient));
	args.Append(GetArgString(_T("NewEnabled"), _T("1")));
	args.Append(GetArgString(_T("NewPortMappingDescription"), descri));
	args.Append(GetArgString(_T("NewLeaseDuration"), 0));

	return InvokeCommand(UPNPADDPORTMAP, args);
}

bool MyUPnP::deletePortmap(int eport, const CString& type)
{
	CString args;

	args.Empty();
	args.Append(GetArgString(_T("NewRemoteHost"), _T("")));
	args.Append(GetArgString(_T("NewExternalPort"), eport));
	args.Append(GetArgString(_T("NewProtocol"), type));

	return InvokeCommand(UPNPDELPORTMAP, args);
}

/////////////////////////////////////////////////////////////////////////////////
// Adds a NAT Port Mapping
// Params:
//  UPNPNAT_MAPPING *mapping  ->  Port Mapping Data
//   If mapping->externalPort is 0, then
//   mapping->externalPort gets the value of mapping->internalPort
//  bool tryRandom:
//   If If mapping->externalPort is in use, tries to find a free
//   random external port.
//
// Return:
//  UNAT_OK:
//   Successfull.
//  UNAT_EXTERNAL_PORT_IN_USE:
//   Error, you are trying to add a port mapping with an external port
//   in use.
//  UNAT_NOT_IN_LAN:
//   Error, you aren't in a LAN -> no router or firewall
//  UNAT_ERROR:
//   Error, use GetLastError() to get an error description.
/////////////////////////////////////////////////////////////////////////////////
MyUPnP::UPNPNAT_RETURN MyUPnP::AddNATPortMapping(UPNPNAT_MAPPING *mapping, bool tryRandom)
{
	CString ProtoStr, Proto;

	if(!IsLANIP(GetLocalIP())){
		SetLastError(_T("You aren't behind a Hardware Firewall or Router"));
		return UNAT_NOT_IN_LAN;
	}

	if (!isComplete()) {
		Search();
		if (!isComplete()) {
			SetLastError(_T("Can not found a UPnP Router"));
			return UNAT_ERROR;
		}
	}

	if (mapping->protocol == UNAT_TCP){
		Proto = _T("TCP");
		ProtoStr = _T("TCP");
	}
	else {
		Proto = _T("UDP");
		ProtoStr = _T("UDP");
	}

	if(mapping->externalPort == 0)
		mapping->externalPort = mapping->internalPort;

	//WORD rndPort = mapping->externalPort;
	for (int retries = 255; retries; retries--) {
		CString Desc;
		Desc.Format(_T("eMule (%s) [%s: %u]"), mapping->description, ProtoStr, mapping->externalPort);

		if (addPortmap(mapping->externalPort, mapping->internalPort, GetLocalIPStr(), Desc, Proto)) {
			m_Mappings.AddTail(*mapping);
			return UNAT_OK;
		}

		if (!tryRandom) {
			SetLastError(_T("External NAT port in use"));
			return UNAT_EXTERNAL_PORT_IN_USE;
		}

		mapping->externalPort = (WORD)(2049 + (65535 - 2049) * rand() / (RAND_MAX + 1));
	}

	SetLastError(_T("External NAT port in use: Too many retries"));
	return UNAT_EXTERNAL_PORT_IN_USE;
}

/////////////////////////////////////////////////////////////////////////////////
// Removes a NAT Port Mapping
// Params:
//  UPNPNAT_MAPPING *mapping  ->  Port Mapping Data
//   Should be the same struct passed to AddNATPortMapping
//  bool removeFromList -> Remove the port mapping from the internal list
//   Should by allways true (dafault value if not passed).
//   If you set it to false can cause an unexpected error.
//
//
// Return:
//  UNAT_OK:
//   Successfull.
//  UNAT_NOT_OWNED_PORTMAPPING:
//   Error, you are trying to remove a port mapping not owned by this class
//  UNAT_NOT_IN_LAN:
//   Error, you aren't in a LAN -> no router or firewall
//  UNAT_ERROR:
//   Error, use GetLastError() to get an error description.
/////////////////////////////////////////////////////////////////////////////////
MyUPnP::UPNPNAT_RETURN MyUPnP::RemoveNATPortMapping(UPNPNAT_MAPPING mapping, bool removeFromList)
{
	if(!IsLANIP(GetLocalIP())){
		SetLastError(_T("You aren't behind a Hardware Firewall or Router"));
		return UNAT_NOT_IN_LAN;
	}

	if (!isComplete()) {
		Search();
		if (!isComplete()) {
			SetLastError(_T("Can not found a UPnP Router"));
			return UNAT_ERROR;
		}
	}

	for(POSITION pos = m_Mappings.GetHeadPosition(); pos!=NULL; m_Mappings.GetNext(pos)){
		UPNPNAT_MAPPING search = m_Mappings.GetAt(pos);

		if (search.externalPort == mapping.externalPort 
			&& search.protocol == mapping.protocol)
		{
			CString Proto;

			if (mapping.protocol == UNAT_TCP)
				Proto = _T("TCP");
			else
				Proto = _T("UDP");

			if (deletePortmap(mapping.externalPort, Proto)) {
				if(removeFromList)
					m_Mappings.RemoveAt(pos);
				return UNAT_OK;
			} else {
				SetLastError(_T("Error getting StaticPortMappingCollection"));
				return UNAT_ERROR;
			}
		}
	}

	SetLastError(_T("Port mapping not owned by this class"));
	return UNAT_NOT_OWNED_PORTMAPPING;
}

void MyUPnP::clearNATPortMapping()
{
	UPNPNAT_MAPPING search;
	POSITION pos = m_Mappings.GetHeadPosition();
	while(pos){
		search = m_Mappings.GetNext(pos);
		RemoveNATPortMapping(search, false);
	}

	m_Mappings.RemoveAll();
}

bool MyUPnP::RemoveSpecifiedPort(WORD port,UPNPNAT_PROTOCOL protocol)
{
	UPNPNAT_MAPPING search;
	POSITION pos = m_Mappings.GetHeadPosition();
	while(pos){
		POSITION cur_pos = pos;
		search = m_Mappings.GetNext(pos);
		if(search.externalPort==port && search.protocol==protocol)
		{
			RemoveNATPortMapping(search, false);
			m_Mappings.RemoveAt(cur_pos);
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////
// Initializes m_localIP variable, for future access to GetLocalIP()
/////////////////////////////////////////////////////////////////////////////////
void MyUPnP::InitLocalIP()
{
#ifndef _DEBUG
	try
#endif
	{
		char szHost[256];
		if (gethostname(szHost, sizeof(szHost)) == 0){
			hostent* pHostEnt = gethostbyname(szHost);
			if (pHostEnt != NULL && pHostEnt->h_length == 4 && pHostEnt->h_addr_list[0] != NULL){
				UPNPNAT_MAPPING mapping;
				struct in_addr addr;

				memcpy(&addr, pHostEnt->h_addr_list[0], sizeof(struct in_addr));
				m_slocalIP = inet_ntoa(addr);
				m_uLocalIP = addr.S_un.S_addr;
			}
			else{
				m_slocalIP = _T("");
				m_uLocalIP = 0;
			}
		}
		else{
			m_slocalIP = _T("");
			m_uLocalIP = 0;
		}
	}
#ifndef _DEBUG
	catch(...){
		m_slocalIP = _T("");
		m_uLocalIP = 0;
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////////
// Returns the Local IP
/////////////////////////////////////////////////////////////////////////////////
DWORD MyUPnP::GetLocalIP()
{
	if(m_uLocalIP == 0)
		InitLocalIP();

	return m_uLocalIP;
}

/////////////////////////////////////////////////////////////////////////////////
// Returns a CString with the local IP in format xxx.xxx.xxx.xxx
/////////////////////////////////////////////////////////////////////////////////
CString MyUPnP::GetLocalIPStr()
{
	if(m_slocalIP.IsEmpty())
		InitLocalIP();

	return m_slocalIP;
}

/////////////////////////////////////////////////////////////////////////////////
// Sets the value of m_lastError (last error description)
/////////////////////////////////////////////////////////////////////////////////
void MyUPnP::SetLastError(CString error) {
	m_slastError = error;
};

/////////////////////////////////////////////////////////////////////////////////
// Returns the last error description in a CString
/////////////////////////////////////////////////////////////////////////////////
CString MyUPnP::GetLastError()
{ 
	return m_slastError; 
}

/////////////////////////////////////////////////////////////////////////////////
// Returns true if nIP is a LAN ip, false otherwise
/////////////////////////////////////////////////////////////////////////////////
bool MyUPnP::IsLANIP(DWORD nIP){
	// filter LAN IP's
	// -------------------------------------------
	// 0.*
	// 10.0.0.0 - 10.255.255.255  class A
	// 172.16.0.0 - 172.31.255.255  class B
	// 192.168.0.0 - 192.168.255.255 class C

	unsigned char nFirst = (unsigned char)nIP;
	unsigned char nSecond = (unsigned char)(nIP >> 8);

	if (nFirst==192 && nSecond==168) // check this 1st, because those LANs IPs are mostly spreaded
		return true;

	if (nFirst==172 && nSecond>=16 && nSecond<=31)
		return true;

	if (nFirst==0 || nFirst==10)
		return true;

	return false; 
}
