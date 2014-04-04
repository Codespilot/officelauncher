/*
 * Copyright (C) 2012-2013 Alfresco Software Limited.
 *
 * This file is part of Alfresco
 *
 * Alfresco is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Alfresco is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Alfresco. If not, see <http://www.gnu.org/licenses/>.
 */

#include "OfficeLauncherCommons.h"
#include <sstream>
#include "utf8.h"

namespace OfficeLauncherCommons
{

std::wstring utf8_to_wstring(const std::string& src)
{
    std::wstring dest;
#ifdef _WIN32
    utf8::utf8to16(src.begin(), src.end(), std::back_inserter(dest));
#else
    utf8::utf8to32(src.begin(), src.end(), std::back_inserter(dest));
#endif
    return dest;
}

std::string wstring_to_utf8(const std::wstring& src)
{
    std::string dest;
#ifdef _WIN32
    utf8::utf16to8(src.begin(), src.end(), std::back_inserter(dest));
#else
    utf8::utf32to8(src.begin(), src.end(), std::back_inserter(dest));
#endif
    return dest;
}

// urlDecode: modified version from boost_asio/example/http/server3/request_handler.cpp
// original code distributed under the Boost Software License, Version 1.0.
std::string urlDecodeToUtf8(const std::wstring &src)
{
    // the src is a wstring and can already contain Unicode chars. So we need to convert
    // them back to UTF8.
    std::string inp = wstring_to_utf8(src);
    // now we can decode the percent encoding in inp
    std::string result;
    for (std::size_t i = 0; i < inp.size(); ++i)
    {
        if ( (inp[i] == '%') && (i + 3 <= inp.size()) )
        {
            int value = 0;
            std::istringstream is(inp.substr(i + 1, 2));
            if (is >> std::hex >> value)
            {
                result += static_cast<char>(value);
                i += 2;
            }
            else
            {
                result += inp[i];
            }
        }
        else
        {
            result += inp[i];
        }
    }
    return result;
}

std::wstring decodeDollarEncoding(const std::string &src)
{
    std::string utf8_result;
    for (std::size_t i = 0; i < src.size(); ++i)
    {
        if ( (src[i] == '$') && (i + 3 <= src.size()) )
        {
            int value = 0;
            std::istringstream is(src.substr(i + 1, 2));
            if (is >> std::hex >> value)
            {
                utf8_result += static_cast<char>(value);
                i += 2;
            }
            else
            {
                utf8_result += src[i];
            }
        }
        else
        {
            utf8_result += src[i];
        }
    }
    return utf8_to_wstring(utf8_result);
}

std::wstring urlDecodeComplete(const std::wstring &src)
{
    return utf8_to_wstring(urlDecodeToUtf8(src));
}

};