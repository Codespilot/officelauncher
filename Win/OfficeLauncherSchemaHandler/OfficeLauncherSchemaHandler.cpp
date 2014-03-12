/*
 * Copyright (C) 2012-2014 Alfresco Software Limited.
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

#include "stdafx.h"
#include <string>
#include <iostream>
#include "SimpleUri.h"
#include <Windows.h>
#include <vector>
#include <sstream>
#include "../PlatformDependentOfficeLauncher.h"
#include "../../OfficeLauncherPlugInErrorCodes.h"
#include "../../OfficeLauncherCommons.h"

using namespace OfficeLauncherCommons;

#define MAX_URL_LENGTH 1024

bool confirmOpen(SimpleUri& decodedUri)
{
    std::wstring msg = L"Do you want to open this file?\n\n";
    msg.append(L"File name: ");
    msg.append(decodedUri.getFilename());
    msg.append(L"\nFrom: ");
    msg.append(decodedUri.getServer());
    msg.append(L"\n\nSome files can harm your computer. If you do not fully trust the source, do not open the file.");
    int result = MessageBox(NULL, msg.c_str(), L"Confirm", MB_OKCANCEL | MB_ICONINFORMATION);
    return (result == IDOK);
}

void errorMessage(const std::wstring& message)
{
    MessageBox(NULL, message.c_str(), L"Error", MB_OK | MB_ICONERROR);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    PlatformDependentOfficeLauncher platformOfficeLauncher;

    bool readOnly = false;
    std::wstring schemaHandlerUri(lpCmdLine);
    std::wstring encodedUrl;
    if(schemaHandlerUri.length() < 21)
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return 1;
    }
    if(schemaHandlerUri.substr(0,20) == L"officelauncher-edit:")
    {
        readOnly = false;
        encodedUrl = schemaHandlerUri.substr(20,schemaHandlerUri.length()-20);
    }
    else if(schemaHandlerUri.substr(0,20) == L"officelauncher-view:")
    {
        readOnly = true;
        encodedUrl = schemaHandlerUri.substr(20,schemaHandlerUri.length()-20);
    }
    else
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return 1;
    }

    if(encodedUrl.size() > MAX_URL_LENGTH)
    {
        errorMessage(L"URL too long.");
        return 1;
    }
    SimpleUri decodedUri(urlDecode(encodedUrl));
    if(!decodedUri.isValid())
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return 1;
    }
    if(!decodedUri.isHttpOrHttpsSchema())
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return 1;
    }
    if(!( platformOfficeLauncher.suppressOpenWarning(decodedUri) || confirmOpen(decodedUri)))
    {
        return 1;
    }
    int result = platformOfficeLauncher.openDocument(encodedUrl, readOnly);
    if(OLP_ERROR_SUCCESS != result)
    {
        errorMessage(L"Failed starting Microsoft Office.");
        return 1;
    }
    return 0;
}
