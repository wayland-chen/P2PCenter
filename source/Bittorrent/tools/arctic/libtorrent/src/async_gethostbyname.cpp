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


#include "libtorrent/async_gethostbyname.hpp"
#include <boost/thread.hpp>
#include <queue>

namespace
{
	using libtorrent::detail::lookup_ptr;
	using libtorrent::detail::lookup_entry;
	using libtorrent::address;
		  
	class lookup_thread
	{
	friend struct libtorrent::dns_lookup;
		lookup_thread()
			: m_close(false)
			, m_thread(boost::ref(*this))
		{}

	public:

		~lookup_thread()
		{
			{
				boost::mutex::scoped_lock l(m_lock);
				m_close = true;
				m_cond.notify_one();
			}
			m_thread.join();
		}
		
		lookup_ptr initiate_lookup(char const* name, int port)
		{
			lookup_ptr e(new lookup_entry);
			e->finished = false;
			e->name = name;
			e->ip.port = port;
			boost::mutex::scoped_lock l(m_lock);
			m_queue.push(e);
			m_cond.notify_one();
			return e;
		}

		void operator()()
		{
			for (;;)
			{
				boost::mutex::scoped_lock l(m_lock);
				if (m_close) return;
				if (m_queue.empty())
					m_cond.wait(l);
				if (m_close) return;
				lookup_ptr e = m_queue.front();
				m_queue.pop();
				try
				{
					e->ip = address(e->name.c_str(), e->ip.port);
				}
				catch (std::exception& ex)
				{
					e->failed = ex.what();
					e->failed += ": ";
					e->failed += e->name;
				}
				e->finished = true;
			}
		}
		
	private:
		boost::mutex m_lock;
		boost::condition m_cond;
		std::queue<lookup_ptr> m_queue;
		bool m_close;
		boost::thread m_thread;
	};
} // anonymous namespace

namespace libtorrent
{
	dns_lookup::dns_lookup(char const* name, int port)
	{
		static lookup_thread t;
		m_entry = t.initiate_lookup(name, port);
	}
}


