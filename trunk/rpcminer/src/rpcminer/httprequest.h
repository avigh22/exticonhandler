/**
    Copyright (C) 2010  puddinpop

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**/

#ifndef _http_request_
#define _http_request_

#include <string>
#include <vector>

class HTTPRequest
{
public:
	HTTPRequest(const std::string &url);
	~HTTPRequest();

	const bool DoRequest(std::string &result);

private:

	static size_t WriteData(void *ptr, size_t size, size_t nmemb, void *user_data);
	static size_t ReadData(void *ptr, size_t size, size_t nmemb, void *user_data);

	std::string m_url;

	std::vector<char> m_writebuff;
	std::vector<char> m_readbuff;
};

#endif	// _http_request_
