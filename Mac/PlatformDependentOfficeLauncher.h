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
#include <map>
#include "../SimpleUri.h"

#ifndef H_PlatformDependentOfficeLauncher
#define H_PlatformDependentOfficeLauncher

class PlatformDependentOfficeLauncher
{

public:
    PlatformDependentOfficeLauncher();
    bool suppressOpenWarning(const SimpleUri& url_utf8);
    int openDocument(const std::wstring& url, bool readOnly);

private:
    std::map<std::string, std::string> m_extensionToBundleMap;

};

#endif // H_PlatformDependentOfficeLauncher 