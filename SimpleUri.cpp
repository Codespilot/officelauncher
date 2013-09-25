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

#include "SimpleUri.h"

SimpleUri::SimpleUri(const std::wstring& _uri) : m_valid(false)
{
    std::wstring temp = _uri;
    m_valid = false;
    // schema
    size_t pos = temp.find(L"://");
    if(pos == std::wstring::npos)
    {
        return;
    }
    m_scheme = temp.substr(0,pos);
    // server and auth
    pos += 3;
    size_t pos2 = temp.find(L"/",pos);
    if(pos2 == std::wstring::npos)
    {
        return;
    }
    std::wstring serverPart = temp.substr(pos, pos2-pos);
    temp = temp.substr(pos2);
    size_t serverSep = serverPart.find(L"@");
    if(serverSep != std::wstring::npos)
    {
        m_auth = serverPart.substr(0,serverSep);
        serverPart =  serverPart.substr(serverSep+1);
    }
    size_t portSep = serverPart.find(L":");
    if(portSep != std::wstring::npos)
    {
        m_port =  serverPart.substr(portSep+1);
        serverPart = serverPart.substr(0,portSep);
    }
    m_server = serverPart;
    // fragment
    size_t posAnker = temp.find(L"#");
    if(posAnker != std::wstring::npos)
    {
        m_fragment = temp.substr(posAnker+1);
        temp = temp.substr(0,posAnker);
    }
    // query
    size_t posQuery = temp.find(L"?");
    if(posQuery != std::wstring::npos)
    {
        m_query = temp.substr(posQuery+1);
        temp = temp.substr(0,posQuery);
    }
    // path
    m_path = temp;
    // filename
    size_t posLastSep = m_path.rfind(L"/");
    if(posLastSep != std::wstring::npos)
    {
        m_filename =  m_path.substr(posLastSep+1);
    }
    // file  extension
    size_t posFileExt = m_filename.rfind(L".");
    if(posFileExt != std::wstring::npos)
    {
        m_fileExtension = m_filename.substr(posFileExt);
    }
    m_valid = true;
}

void SimpleUri::parseServerAndAuth()
{
}

bool SimpleUri::isValid()
{
    return m_valid;
}

bool SimpleUri::isHttpOrHttpsSchema()
{
    if(!m_valid)
    {
        return false;
    }
    if( (m_scheme.size()==4) &&
        ((m_scheme[0]==L'H')||(m_scheme[0]==L'h')) &&
        ((m_scheme[1]==L'T')||(m_scheme[1]==L't')) &&
        ((m_scheme[2]==L'T')||(m_scheme[2]==L't')) &&
        ((m_scheme[3]==L'P')||(m_scheme[3]==L'p')) )
    {
        return true;
    }
    if( (m_scheme.size()==5) &&
        ((m_scheme[0]==L'H')||(m_scheme[0]==L'h')) &&
        ((m_scheme[1]==L'T')||(m_scheme[1]==L't')) &&
        ((m_scheme[2]==L'T')||(m_scheme[2]==L't')) &&
        ((m_scheme[3]==L'P')||(m_scheme[3]==L'p')) &&
        ((m_scheme[4]==L'S')||(m_scheme[4]==L's')) )
    {
        return true;
    }
    return false;
}

std::wstring SimpleUri::getSchema()
{
    return m_scheme;
}

std::wstring SimpleUri::getServer()
{
    return m_server;
}

std::wstring SimpleUri::getPath()
{
    return m_path;
}

std::wstring SimpleUri::getFilename()
{
    return m_filename;
}

std::wstring SimpleUri::getFileExtension()
{
    return m_fileExtension;
}
