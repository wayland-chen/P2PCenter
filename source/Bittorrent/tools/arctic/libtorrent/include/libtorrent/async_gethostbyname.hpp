/*

Copyright (c) 2005, Arvid Norberg
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

#ifndef TORRENT_ASYNC_GETHOSTBYNAME_HPP_INCLUDED
#define TORRENT_ASYNC_GETHOSTBYNAME_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <string>
#include "libtorrent/socket.hpp"

namespace libtorrent
{
	namespace detail
	{
		struct lookup_entry
		{
			bool finished;
			address ip;
			std::string name;
			std::string failed;
		};
		typedef boost::shared_ptr<lookup_entry> lookup_ptr;
	}
	
	struct dns_lookup
	{
		dns_lookup() {}
		dns_lookup(char const* name, int port);
		bool running() const { return m_entry; }
		bool finished() const { return m_entry->finished; }
		bool failed() const { return !m_entry->failed.empty(); }
		std::string const& error() const { return m_entry->failed; }
		address ip() const { return m_entry->ip; }
	private:
		detail::lookup_ptr m_entry;
	};
}

#endif // TORRENT_ASYNC_GETHOSTBYNAME_HPP_INCLUDED

