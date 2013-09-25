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

#ifndef H_PlatformDependentOfficeLauncher
#define H_PlatformDependentOfficeLauncher

class PlatformDependentOfficeLauncher
{

public:
    PlatformDependentOfficeLauncher();
    int viewDocument(const std::string& url_utf8);
    int editDocument(const std::string& url_utf8);
    bool suppressOpenWarning(const std::string& url_utf8);

private:
    int openDocument(const std::string& url_utf8, bool readOnly);
    std::map<std::string, std::string> m_extensionToBundleMap;

};

#endif // H_PlatformDependentOfficeLauncher 