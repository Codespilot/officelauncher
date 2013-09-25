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

#include <string>

#ifndef H_SimpleUri
#define H_SimpleUri

class SimpleUri
{

public:
    SimpleUri(const std::wstring& uri);
    bool isValid();
    bool isHttpOrHttpsSchema();
    std::wstring getSchema();
    std::wstring getServer();
    std::wstring getPath();
    std::wstring getFilename();
    std::wstring getFileExtension();

private:
    bool m_valid;
    std::wstring m_scheme;
    std::wstring m_auth;
    std::wstring m_server;
    std::wstring m_port;
    std::wstring m_path;
    std::wstring m_query;
    std::wstring m_fragment;
    std::wstring m_filename;
    std::wstring m_fileExtension;
    void parseServerAndAuth();

};

#endif // H_SimpleUri