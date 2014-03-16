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
#include <vector>
#include <iterator>
#include "windows.h"
#include <WinReg.h>
#include "../SimpleUri.h"

#ifndef H_PlatformDependentOfficeLauncher
#define H_PlatformDependentOfficeLauncher

class PlatformDependentOfficeLauncher
{

public:
    PlatformDependentOfficeLauncher();
    bool suppressOpenWarning(const SimpleUri& decodedUri);
    long openDocument(const std::wstring& encodedUrl, bool readOnly);

private:
    std::vector<std::wstring> m_validProgIds;
    bool readRegValueSZ(HKEY key, const std::wstring& path, const std::wstring& name, std::wstring& value);
    bool readRegValueDWORD(HKEY key, const std::wstring& path, const std::wstring& name, DWORD& value);
    bool progIdEnabled(const std::wstring& progid);
    bool executeCommand(const std::wstring& commandLine);
    bool formatMessage(const std::wstring& source, const std::wstring& param1, const std::wstring& param2, std::wstring& dest);
    bool formatMessageOld(const std::wstring& source, std::wstring& destMessage, ...);
    bool executeDDE(const std::wstring& ddeCommand, const std::wstring& application, const std::wstring& topic);

};

#endif // H_PlatformDependentOfficeLauncher 