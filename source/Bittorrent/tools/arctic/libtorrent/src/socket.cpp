/*

Copyright (c) 2003, Arvid Norberg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#include <boost/cstdint.hpp>
#include <cassert>
#include <iostream>

#ifdef _MSC_VER
#pragma warning(push, 1)
#endif

#include <boost/shared_ptr.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "libtorrent/socket.hpp"


// ---------- winsock part ------------------------------------

#ifdef _WIN32

#define NOMINMAX
#define FD_SETSIZE 1024
#include <winsock2.h>

using namespace libtorrent;

// a kind of hack to hide the WinSock API
namespace
{
	typedef int socklen_t;

	struct error_map_pair
	{
		int native_error;
		enum libtorrent::socket::error_code error;
		bool operator<(const error_map_pair& p)
		{ return native_error < p.native_error; }
		bool operator==(int e) { return e == native_error; }
	};

	libtorrent::socket::error_code get_last_error()
	{
		int native_error = WSAGetLastError();
		error_map_pair error_map[] =
		{
			{WSAENETDOWN, libtorrent::socket::netdown},
			{WSAEFAULT, libtorrent::socket::fault},
			{WSAEACCES, libtorrent::socket::access},
			{WSAEADDRINUSE, libtorrent::socket::address_in_use},
			{WSAEADDRNOTAVAIL, libtorrent::socket::address_not_available},
			{WSAEINPROGRESS, libtorrent::socket::in_progress},
			{WSAEINTR, libtorrent::socket::interrupted},
			{WSAEINVAL, libtorrent::socket::invalid},
			{WSAENETRESET, libtorrent::socket::net_reset},
			{WSAENOTCONN, libtorrent::socket::not_connected},
			{WSAENOBUFS, libtorrent::socket::no_buffers},
			{WSAEOPNOTSUPP, libtorrent::socket::operation_not_supported},
			{WSAENOTSOCK, libtorrent::socket::not_socket},
			{WSAESHUTDOWN, libtorrent::socket::shutdown},
			{WSAEWOULDBLOCK, libtorrent::socket::would_block},
			{WSAECONNRESET, libtorrent::socket::connection_reset},
			{WSAETIMEDOUT, libtorrent::socket::timed_out},
			{WSAECONNABORTED, libtorrent::socket::connection_aborted},
			{WSAEMSGSIZE, libtorrent::socket::message_size},
			{WSAEALREADY, libtorrent::socket::not_ready},
			{WSAEAFNOSUPPORT, libtorrent::socket::no_support},
			{WSAECONNREFUSED, libtorrent::socket::connection_refused},
			{WSAEISCONN, libtorrent::socket::is_connected},
			{WSAENETUNREACH, libtorrent::socket::net_unreachable},
			{WSANOTINITIALISED, libtorrent::socket::not_initialized},
		};
		int num_elements = sizeof(error_map) / sizeof(error_map_pair);

		error_map_pair* e	= std::find(error_map
				, error_map + num_elements
				, native_error);

		if (e == error_map + num_elements)
			return libtorrent::socket::unknown_error;
		return e->error;
	}

	unsigned int winsock_refcnt = 0;

	void init()
	{
		winsock_refcnt++;

		if(winsock_refcnt > 1) return;

		// check for appropriate version.
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		int ret;

		ret = WSAStartup(version, &data);
		
		if (ret != 0)
			throw network_error(get_last_error());
	}

	void deinit()
	{
		winsock_refcnt--;
		if (winsock_refcnt < 1) WSACleanup();
	}

}


#else
// ---------- bsd socket part ------------------------------------

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>

using namespace libtorrent;

namespace
{

	bool initialized = false;

	struct error_map_pair
	{
		int native_error;
		enum libtorrent::socket::error_code error;
		bool operator<(const error_map_pair& p)
		{ return native_error < p.native_error; }
		bool operator==(int e) { return e == native_error; }
	};


	libtorrent::socket::error_code get_last_error()
	{
		int native_error = errno;
		error_map_pair error_map[] =
		{
			{ENETDOWN, libtorrent::socket::netdown},
			{EFAULT, libtorrent::socket::fault},
			{EACCES, libtorrent::socket::access},
			{EADDRINUSE, libtorrent::socket::address_in_use},
			{EADDRNOTAVAIL, libtorrent::socket::address_not_available},
			{EINPROGRESS, libtorrent::socket::in_progress},
			{EINTR, libtorrent::socket::interrupted},
			{EINVAL, libtorrent::socket::invalid},
			{ENETRESET, libtorrent::socket::net_reset},
			{ENOTCONN, libtorrent::socket::not_connected},
			{ENOMEM, libtorrent::socket::no_buffers},
			{EOPNOTSUPP, libtorrent::socket::operation_not_supported},
			{ENOTSOCK, libtorrent::socket::not_socket},
			{ESHUTDOWN, libtorrent::socket::shutdown},
			{EAGAIN, libtorrent::socket::would_block},
			{ECONNRESET, libtorrent::socket::connection_reset},
			{ETIMEDOUT, libtorrent::socket::timed_out},
			{ECONNABORTED, libtorrent::socket::connection_aborted},
			{EMSGSIZE, libtorrent::socket::message_size},
			{EALREADY, libtorrent::socket::not_ready},
			{EAFNOSUPPORT, libtorrent::socket::no_support},
			{ECONNREFUSED, libtorrent::socket::connection_refused},
			{EISCONN, libtorrent::socket::is_connected},
			{ENETUNREACH, libtorrent::socket::net_unreachable}
		};
		int num_elements = sizeof(error_map) / sizeof(error_map_pair);

		error_map_pair* e	= std::find(error_map
				, error_map + num_elements
				, native_error);

		if (e == error_map + num_elements)
			return libtorrent::socket::unknown_error;
		return e->error;
	}

	void init()
	{
		if (initialized) return;
		initialized = true;
		signal(SIGPIPE, SIG_IGN);
	}

	void deinit() {}

}

#endif

// ---------- common part ------------------------------------


namespace
{
	sockaddr_in address_to_native(const libtorrent::address& addr)
	{
		sockaddr_in in;
		memset(&in, 0, sizeof(in));

		if (addr.ip() == address::any_addr)
			in.sin_addr.s_addr = INADDR_ANY;
		else
			in.sin_addr.s_addr = htonl(addr.ip());

		if (addr.port == address::any_port)
			in.sin_port = 0;
		else
			in.sin_port = htons(addr.port);

		in.sin_family = AF_INET;
		return in;
	}
}

std::string libtorrent::address::as_string() const
{
	in_addr in;
	in.s_addr = htonl(m_ip);
	return inet_ntoa(in);
}

void libtorrent::socket::listen(libtorrent::address const& iface, int queue)
{
	sockaddr_in listener_address = address_to_native(iface);
	assert(queue > 0);
	
	if(::bind(m_socket, (sockaddr*)&listener_address, sizeof(sockaddr)) != 0)
		throw network_error(last_error());
	
	if(::listen(m_socket, queue) != 0)
		throw network_error(last_error());
}

boost::shared_ptr<libtorrent::socket> libtorrent::socket::accept()
{
	int new_socket;
	sockaddr_in addr;
	socklen_t size = sizeof(addr);
	
	new_socket = (int)::accept(m_socket, (sockaddr*)&addr, &size);

	if(new_socket < 0)
	{
		if (!m_blocking && get_last_error() == would_block)
			return boost::shared_ptr<libtorrent::socket>();
		throw network_error(last_error());
	}

	return boost::shared_ptr<libtorrent::socket>(
		new socket(
			new_socket
			, address(ntohl(addr.sin_addr.s_addr), ntohs(addr.sin_port))
			, m_blocking));
}

address libtorrent::socket::name() const
{
	socklen_t len = sizeof(sockaddr_in);
	sockaddr_in addr;

	int a = getsockname(m_socket, (sockaddr *)&addr, &len);
	if (a != 0) throw network_error(last_error());
	return address(ntohl(addr.sin_addr.s_addr), ntohs(addr.sin_port));
}

int libtorrent::socket::send(const char* buffer, int size)
{
	assert(size > 0);
	assert(buffer != 0);
	return ::send(m_socket, buffer, size, 0);
}

int libtorrent::socket::send_to(const address& addr, const char* buffer, int size)
{
	sockaddr_in in = address_to_native(addr);
	assert(size > 0);
	assert(buffer != 0);
	return ::sendto(m_socket, buffer, size, 0, (const sockaddr*)&in, sizeof(in));
}

int libtorrent::socket::receive(char* buffer, int size)
{
	assert(size > 0);
	assert(buffer != 0);
	sockaddr_in in;
	in.sin_addr.s_addr = htonl(m_sender.ip());
	in.sin_port = htons(m_sender.port);
	socklen_t s = sizeof(in);
	int ret = ::recvfrom(m_socket, buffer, size, 0, (sockaddr*)&in, &s);
	m_sender = address(ntohl(in.sin_addr.s_addr), ntohs(in.sin_port));
	return ret;
}

bool libtorrent::socket::is_readable() const
{
	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(m_socket, &read_set);
	timeval t = { 0, 1 };
	int ret = ::select(m_socket+1, &read_set, 0, 0, &t);
	return ret == 1;
}

bool libtorrent::socket::is_writable() const
{
	fd_set write_set;
	FD_ZERO(&write_set);
	FD_SET(m_socket, &write_set);
	timeval t = { 0, 1 };
	int ret = ::select(m_socket+1, 0, &write_set, 0, &t);
	return ret == 1;
}

bool libtorrent::socket::has_error() const
{
	fd_set error_set;
	FD_ZERO(&error_set);
	FD_SET(m_socket, &error_set);
	timeval t = { 0, 1 };
	int ret = ::select(m_socket+1, 0, 0, &error_set, &t);
	return ret == 1;
}

void libtorrent::selector::remove(boost::shared_ptr<socket> s)
{
	std::vector<boost::shared_ptr<socket> >::iterator i;
	i = std::find(m_readable.begin(), m_readable.end(), s);
	if (i != m_readable.end()) m_readable.erase(i);
	assert(std::find(m_readable.begin(), m_readable.end(), s) == m_readable.end());

	i = std::find(m_writable.begin(), m_writable.end(),s);
	if (i != m_writable.end()) m_writable.erase(i);
	assert(std::find(m_writable.begin(), m_writable.end(), s) == m_writable.end());

	i = std::find(m_error.begin(), m_error.end(), s);
	if (i != m_error.end()) m_error.erase(i);
	assert(std::find(m_error.begin(), m_error.end(), s) == m_error.end());
}


namespace libtorrent
{

	address::address(const address& a)
		: port(a.port)
		, m_ip(a.m_ip)
	{
		init();
	}

	address::address(
		unsigned char a
		, unsigned char b
		, unsigned char c
		, unsigned char d
		, unsigned short port_)
		: port(port_)
		, m_ip((a << 24) + (b << 16) + (c << 8) + d)
	{
		init();
	}

	address::address(unsigned int addr, unsigned short port_)
		: port(port_)
		, m_ip(addr)
	{
		init();
	}

	address::address(const char* address, unsigned short port_)
		: port(port_)
	{
		init();

		if (address == 0 || std::strlen(address) == 0)
		{
			m_ip = address::any_addr;
		}
		else
		{
			// some systems seem to do a lookup (and block)
			// even on addresses that are already resolved
			// (Windows NT 4)
			unsigned int a, b, c, d;
			if (std::sscanf(address, "%u.%u.%u.%u", &a, &b, &c, &d) == 4)
			{
				if (a > 255 || b > 255 || c > 255 || d > 255)
					throw network_error(socket::host_not_found);
				m_ip = (a << 24) + (b << 16) + (c << 8) + d;
			}
			else
			{	  
				// resolve the hostname
				hostent* hostentry = gethostbyname(address);

				if (hostentry == 0) throw network_error(socket::host_not_found);
				m_ip = ntohl(*(unsigned int*)hostentry->h_addr_list[0]);
			}
		}
	}

	address::address()
		: port(address::any_addr)
		, m_ip(address::any_port)
	{
		init();
	}

	address::~address()
	{
		deinit();
	}

}

/*!
	\param the type of the socket, can be socket::tcp or socket::udp.
	\param blocking set this to true if you want the receive() call, for this socket, to block until data is made available.
	\param receive_port This parameter is only used by udp-sockets that will receive data. This port is then
	the port data will be received from. If set to zero, the port this socket is locally bound to is undefined (but as
	long as you just use it to send data, it doesn't matter).
*/
libtorrent::socket::socket(type t, bool blocking, unsigned short receive_port):
	m_socket(0),
	m_blocking(blocking)
{
	init();

#ifndef NDEBUG
	m_type = t;
	m_connected = false;
#endif

	switch(t)
	{
		case tcp: m_socket =(int)::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); break;
		case udp: m_socket = (int)::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); break;
		default: assert(0);
	}
	
	if(m_socket == 0)
		throw network_error(get_last_error());

	if (t == udp && receive_port != 0)
	{
		// bind socket
		sockaddr_in addr
			= address_to_native(address(address::any_addr, receive_port));
	
		if (::bind(m_socket, (sockaddr*)&addr, sizeof(addr)) != 0)
			throw network_error(get_last_error());
	}

	assert(((t == tcp) && (receive_port == 0)) || (t == udp));

	set_blocking(m_blocking);
}

libtorrent::socket::socket(int socket, const address& sender, bool blocking)
	: m_socket(socket)
	, m_sender(sender)
	, m_blocking(blocking)
{
	init();
	set_blocking(m_blocking);
#ifndef NDEBUG
	m_type = tcp;
	m_connected = true;
#endif
}

libtorrent::socket::~socket()
{
#ifdef _WIN32
	closesocket(m_socket);
#else
	::close(m_socket);
#endif
	deinit();
}

void libtorrent::socket::connect(
	const address& addr
	, address const& bind_to)
{
	sockaddr_in in = address_to_native(addr);
	sockaddr_in bind_interface = address_to_native(bind_to);

	if (::bind(m_socket, (const sockaddr*)&bind_interface, sizeof(bind_interface)) != 0)
	{
		error_code err = get_last_error();
#ifdef _WIN32
		if (err != would_block)
			throw network_error(err);
#else
		if (err != in_progress)
			throw network_error(err);
#endif
	}
	if(::connect(m_socket, (const sockaddr*)(&in), sizeof(in)) != 0)
	{
		error_code err = get_last_error();
#ifdef _WIN32
		if (err != would_block)
			throw network_error(err);
#else
		if (err != in_progress)
			throw network_error(err);
#endif
	}

#ifndef NDEBUG
	m_connected = true;
#endif

	m_sender = addr;
}

void libtorrent::socket::set_blocking(bool blocking)
{
	m_blocking = blocking;
#ifdef _WIN32
	unsigned long val = blocking?0:1;
	ioctlsocket(m_socket, FIONBIO, &val);
#else
	int flags = fcntl(m_socket, F_GETFL);
	if (blocking) flags &= ~O_NONBLOCK;
	else flags |= O_NONBLOCK;
	if (fcntl(m_socket, F_SETFL, flags) == -1)
	{ throw network_error(get_last_error()); }
#endif
}

libtorrent::socket::error_code libtorrent::socket::last_error() const
{
	return get_last_error();
}

void libtorrent::selector::wait(int timeout
	, std::vector<boost::shared_ptr<socket> >& readable
	, std::vector<boost::shared_ptr<socket> >& writable
	, std::vector<boost::shared_ptr<socket> >& error)
{
	assert(timeout >= 0);

	readable.clear();
	writable.clear();
	error.clear();

	if (m_readable.size() + m_writable.size() + m_error.size() == 0)
	{
		// TODO: temporary implementation
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.nsec += timeout * 1000;
		boost::thread::sleep(xt);
		return;
	}

	fd_set read_set;
	fd_set write_set;
	fd_set error_set;

	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&error_set);

	int max_filedes = 0;
	
	for (std::vector<boost::shared_ptr<socket> >::iterator i = m_readable.begin();
			i != m_readable.end();
			++i)
	{
		FD_SET((*i)->m_socket, &read_set);
#ifndef _WIN32
		if (max_filedes < (*i)->m_socket) max_filedes = (*i)->m_socket;
#endif
	}
	for (std::vector<boost::shared_ptr<socket> >::iterator i = m_writable.begin();
			i != m_writable.end();
			++i)
	{
		FD_SET((*i)->m_socket, &write_set);
#ifndef _WIN32
		if (max_filedes < (*i)->m_socket) max_filedes = (*i)->m_socket;
#endif
	}
	for (std::vector<boost::shared_ptr<socket> >::iterator i = m_error.begin();
			i != m_error.end();
			++i)
	{
		FD_SET((*i)->m_socket, &error_set);
#ifndef _WIN32
		if (max_filedes < (*i)->m_socket) max_filedes = (*i)->m_socket;
#endif
	}

	int seconds = timeout / 1000000;
	timeval t = { seconds, timeout - seconds * 1000000 };
	int ret = ::select(max_filedes+1, &read_set, &write_set, &error_set, &t);

	if (ret == 0) return;
	if (ret < 0 && get_last_error() != socket::interrupted)
		throw network_error(get_last_error());

	for (std::vector<boost::shared_ptr<socket> >::iterator i = m_readable.begin(); i != m_readable.end(); ++i)
		if (FD_ISSET((*i)->m_socket, &read_set)) readable.push_back(*i);
	for (std::vector<boost::shared_ptr<socket> >::iterator i = m_writable.begin(); i != m_writable.end(); ++i)
		if (FD_ISSET((*i)->m_socket, &write_set)) writable.push_back(*i);
	for (std::vector<boost::shared_ptr<socket> >::iterator i = m_error.begin(); i != m_error.end(); ++i)
		if (FD_ISSET((*i)->m_socket, &error_set)) error.push_back(*i);

#ifndef NDEBUG
	for (std::vector<boost::shared_ptr<socket> >::iterator i = writable.begin(); i != writable.end(); ++i)
		assert((*i)->is_writable());
#endif
}

const char* libtorrent::network_error::what() const throw()
{
	switch (m_error_code)
	{
#ifdef _WIN32
		case socket::not_initialized: return "A successful WSAStartup call must occur before using this function.";
#endif
		case socket::netdown: return "The network subsystem has failed.";
		case socket::fault: return "The buf parameter is not completely contained in a valid part of the user address space.";
		case socket::access: return "ACCSESS";
		case socket::address_in_use: return "A process on the machine is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse.";
		case socket::address_not_available: return "The specified address is not a valid address for this machine.";
		case socket::in_progress: return "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
		case socket::interrupted: return "The (blocking) call was canceled through WSACancelBlockingCall.";
		case socket::invalid: return "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.";
		case socket::net_reset: return "The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.";
		case socket::not_connected: return "The socket is not connected.";
		case socket::no_buffers: return "Not enough buffers available, too many connections.";
		case socket::operation_not_supported: return "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.";
		case socket::not_socket: return "NOTSOCK";
		case socket::shutdown: return "The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.";
		case socket::would_block: return "The socket is marked as nonblocking and the receive operation would block.";
		case socket::connection_reset: return "The virtual circuit was reset by the remote side executing a hard or abortive close.";
		case socket::timed_out: return "The connection has been dropped because of a network failure or because the peer system failed to respond.";
		case socket::connection_aborted: return "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.";
		case socket::message_size: return "The message was too large to fit into the specified buffer and was truncated.";
		case socket::not_ready: return "A nonblocking connect call is in progress on the specified socket.";
		case socket::no_support: return "Addresses in the specified family cannot be used with this socket.";
		case socket::connection_refused: return "The attempt to connect was forcefully rejected.";
		case socket::is_connected: return "The socket is already connected.";
		case socket::net_unreachable: return "The network cannot be reached from this host at this time.";
		case socket::host_not_found: return "Hostname not found";
		case socket::unknown_error:
		default: return "unknown network error.";
	}
}

void libtorrent::socket::set_receive_bufsize(int size)
{
	assert(size > 0);
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&size), sizeof(size));
}

void libtorrent::socket::set_send_bufsize(int size)
{
	assert(size > 0);
	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&size), sizeof(size));
}

void libtorrent::socket::close()
{
#ifndef NDEBUG
	m_connected = false;
#endif
#ifdef _WIN32
	closesocket(m_socket);
#else
	::close(m_socket);
#endif
}
