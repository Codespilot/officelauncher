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
#include "3rdParty\utf8\utf8.h"

// urlDecode: modified version from boost_asio/example/http/server3/request_handler.cpp
// original code distributed under the Boost Software License, Version 1.0.
std::wstring urlDecode(std::wstring &src)
{
    std::wstring result;
    for (std::size_t i = 0; i < src.size(); ++i)
    {
        if ( (src[i] == '%') && (i + 3 <= src.size()) )
        {
            int value = 0;
            std::wistringstream is(src.substr(i + 1, 2));
            if (is >> std::hex >> value)
            {
                result += static_cast<wchar_t>(value);
                i += 2;
            }
            else
            {
                result += src[i];
            }
        }
        else
        {
            result += src[i];
        }
    }
    return result;
}

bool readRegValueSZ(HKEY key, const std::wstring& path, const std::wstring& name, std::wstring& value)
{
    HKEY keyHandle;
    if( RegOpenKeyEx(key, path.c_str(), 0, KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS)
    {
        DWORD type;
        WCHAR buffer[1024];
        DWORD bufferSize = sizeof(buffer);
        LONG queryResult = RegQueryValueEx(keyHandle, name.c_str(), NULL, &type, (LPBYTE)&buffer, &bufferSize);
        RegCloseKey(keyHandle);
        if( (queryResult == ERROR_SUCCESS) && ((type == REG_SZ) || (type == REG_EXPAND_SZ)) )
        {
            value = buffer;
            return true;
        }
    }
    return false;
}

bool readRegValueDWORD(HKEY key, const std::wstring& path, const std::wstring& name, DWORD& value)
{
    HKEY keyHandle;
    if( RegOpenKeyEx(key, path.c_str(), 0, KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS)
    {
        DWORD type;
        DWORD buffer;
        DWORD bufferSize = sizeof(DWORD);
        LONG queryResult = RegQueryValueEx(keyHandle, name.c_str(), NULL, &type, (LPBYTE)&buffer, &bufferSize);
        RegCloseKey(keyHandle);
        if( (queryResult == ERROR_SUCCESS) && (type == REG_DWORD) )
        {
            value = buffer;
            return true;
        }
    }
    return false;
}

bool confirmOpen(const std::wstring& filename, const std::wstring& server)
{
    std::wstring msg = L"Do you want to open this file?\n\n";
    msg.append(L"File name: ");
    msg.append(filename);
    msg.append(L"\nFrom: ");
    msg.append(server);
    msg.append(L"\n\nSome files can harm your computer. If you do not fully trust the source, do not open the file.");
    int result = MessageBox(NULL, msg.c_str(), L"Confirm", MB_OKCANCEL | MB_ICONINFORMATION  /*| MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY*/);
    return (result == IDOK);
}

bool suppressOpenWarning()
{
    DWORD value = 0;
    bool readResult = readRegValueDWORD(HKEY_CURRENT_USER, L"Software\\Office Launcher Plug-In", L"SuppressOpenWarning", value);
    return readResult && (value == 1);
}

bool progIdEnabled(const std::wstring& progid)
{
    std::vector<std::wstring> m_validProgIds;
    m_validProgIds.push_back(L"Excel.Sheet");
    m_validProgIds.push_back(L"Excel.SheetMacroEnabled");
    m_validProgIds.push_back(L"Excel.Template");
    m_validProgIds.push_back(L"Word.Document");
    m_validProgIds.push_back(L"Word.Template");
    m_validProgIds.push_back(L"PowerPoint.Template");
    m_validProgIds.push_back(L"PowerPoint.Slide");
    m_validProgIds.push_back(L"PowerPoint.Show");
    m_validProgIds.push_back(L"Visio.Stencil");
    m_validProgIds.push_back(L"Visio.Drawing");
    m_validProgIds.push_back(L"Visio.Template");
    m_validProgIds.push_back(L"AcroPDF.PDF");
    m_validProgIds.push_back(L"Access.Application");
    for(std::vector<std::wstring>::iterator it = m_validProgIds.begin(); it != m_validProgIds.end(); ++it)
    {
        if(progid == *it)
        {
            return true;
        }
        std::wstring prefix(*it);
        prefix.append(L".");
        if( (progid.size() > prefix.size()) && (progid.substr(0,prefix.size()) == prefix) )
        {
            return true;
        }
    }
    return false;
}

void StringReplace(std::wstring& str, const std::wstring& oldStr, const std::wstring& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::wstring::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

bool formatMessage(const std::wstring& source, const std::wstring& param1, const std::wstring& param2, std::wstring& dest)
{
    dest = source;
    StringReplace(dest, L"%1", param1);
    StringReplace(dest, L"%2", param2);
    StringReplace(dest, L"%3", L"");
    StringReplace(dest, L"%4", L"");
    StringReplace(dest, L"%5", L"");
    StringReplace(dest, L"%6", L"");
    StringReplace(dest, L"%7", L"");
    StringReplace(dest, L"%8", L"");
    StringReplace(dest, L"%9", L"");
    StringReplace(dest, L"%0", L"");
    return true;
}

std::string wstring_to_utf8(const std::wstring& src)
{
    std::string out_str;
    utf8::utf16to8(src.begin(), src.end(), std::back_inserter(out_str));
    return out_str;
}

bool executeCommand(const std::wstring& commandLine)
{
    STARTUPINFO startupinfo;
    PROCESS_INFORMATION procinfo;
    memset(&procinfo, 0, sizeof(procinfo));
    memset(&startupinfo, 0, sizeof(startupinfo));
    startupinfo.cb = sizeof(startupinfo);
    WCHAR tempCmdLine[MAX_PATH * 2];
    if(wcscpy_s(tempCmdLine, MAX_PATH *2, commandLine.c_str()) != 0)
    {
        return false;
    }
    if(CreateProcess(NULL,         // LPCWSTR lpApplicationName
                     tempCmdLine,  // LPWSTR lpCommandLine
                     NULL,         // LPSECURITY_ATTRIBUTES lpProcessAttributes
                     NULL,         // LPSECURITY_ATTRIBUTES lpThreadAttributes
                     FALSE,        // BOOL bInheritHandles
                     0,            // DWORD dwCreationFlags
                     NULL,         // LPVOID lpEnvironment
                     NULL,         // LPCWSTR lpCurrentDirectory
                     &startupinfo, // LPSTARTUPINFOW lpStartupInfo
                     &procinfo))   // LPPROCESS_INFORMATION lpProcessInformation
    {
        if(WaitForInputIdle(procinfo.hProcess, INFINITE) == ERROR_SUCCESS)
        {
            return true;
        }
    }
    return false;
}

HDDEDATA CALLBACK nopDDECallback(UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hDDEData, DWORD dwData1, DWORD dwData2)
{
    return NULL;
}

bool executeDDE(const std::wstring& ddeCommand, const std::wstring& application, const std::wstring& topic)
{
    bool result = false;

    const std::string debug_ddeCommand = wstring_to_utf8(ddeCommand);

    DWORD ddeInstance = 0;
    if(DdeInitialize(&ddeInstance, nopDDECallback, CBF_FAIL_ALLSVRXACTIONS, 0L) != DMLERR_NO_ERROR)
    {
        return false;
    }

    HSZ hszService, hszTopic;
    hszService = DdeCreateStringHandle(ddeInstance, application.c_str(), CP_WINUNICODE);
    hszTopic = DdeCreateStringHandle(ddeInstance, topic.c_str(), CP_WINUNICODE);

    if( (hszService != 0L) && (hszTopic != 0L) )
    {

        HCONV hConv = DdeConnect(ddeInstance, hszService, hszTopic, NULL);

        if(hConv != NULL)
        {
            HDDEDATA hExecData = DdeCreateDataHandle(ddeInstance, (LPBYTE)ddeCommand.c_str(), (ddeCommand.size() + 1) * sizeof(wchar_t), 0, NULL, CF_UNICODETEXT, 0);
            if(hExecData != NULL)
            {
                HDDEDATA ddeTransResult = DdeClientTransaction((LPBYTE)hExecData, (DWORD)-1, hConv, NULL, 0, XTYP_EXECUTE, 1000, NULL);
                if(ddeTransResult != 0)
                {
                    result = true;
                }
                else
                {
                    if(DdeGetLastError(ddeInstance) == DMLERR_NO_CONV_ESTABLISHED)
                    {
                        result = false;
                    }
                    else
                    {
                        result = true;
                    }
                }
            }
        }
    }

    if(hszService !=  0L)
    {
        DdeFreeStringHandle(ddeInstance, hszService);
    }
    if(hszTopic != 0L)
    {
        DdeFreeStringHandle(ddeInstance, hszTopic);
    }
    DdeUninitialize(ddeInstance);

    return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
    // get URI and decode it
    if(argc != 2)
    {
        return 1;
    }
    bool readOnly = false;
    std::wstring officeLauncherUri(argv[1]);
    std::wstring encodedUrl;
    if(officeLauncherUri.length() < 21)
    {
        return 1;
    }
    if(officeLauncherUri.substr(0,20) == L"officelauncher-edit:")
    {
        readOnly = false;
        encodedUrl = officeLauncherUri.substr(20,officeLauncherUri.length()-20);
    }
    else if(officeLauncherUri.substr(0,20) == L"officelauncher-view:")
    {
        readOnly = true;
        encodedUrl = officeLauncherUri.substr(20,officeLauncherUri.length()-20);
    }
    else
    {
        return 1;
    }
    std::wstring decodedUrl = urlDecode(encodedUrl);
    SimpleUri uri(decodedUrl);
    
    // check if URI is valid and a HTTP(s) URI
    if(!uri.isValid())
    {
        return 1;
    }
    if(!uri.isHttpOrHttpsSchema())
    {
        return 1;
    }

    // get user confirmation
    if(!( suppressOpenWarning() || confirmOpen(uri.getFilename(),uri.getServer())))
    {
        return 1;
    }

    // get ProgId for file extension from registry
    std::wstring progId;
    if(!readRegValueSZ(HKEY_CLASSES_ROOT,uri.getFileExtension(),L"",progId))
    {
        return 1;
    }

    // check if there is a more recent version of this ProgId
    std::wstring curVer;
    if(readRegValueSZ(HKEY_CLASSES_ROOT,progId,L"CurVer",curVer))
    {
        progId = curVer;
    }

    // check if ProgId is enabled for this Plug-In
    if(!progIdEnabled(progId))
    {
        return 1;
    }

    // read shell execute commands from registry
    std::wstring shellVerb(readOnly ? L"OpenAsReadOnly" : L"Open");
    std::wstring shellBasePath(progId);
    shellBasePath.append(L"\\shell\\");
    shellBasePath.append(shellVerb);
    std::wstring command;
    std::wstring ddeexec;
    std::wstring ddeexecApplication;
    std::wstring ddeexecTopic;
    std::wstring ddeexecIfExec;
    bool hasCommand = readRegValueSZ(HKEY_CLASSES_ROOT,shellBasePath+L"\\command",L"",command);
    bool hasDdeexec = readRegValueSZ(HKEY_CLASSES_ROOT,shellBasePath+L"\\ddeexec",L"",ddeexec);
    bool hasDdeexecApplication = readRegValueSZ(HKEY_CLASSES_ROOT,shellBasePath+L"\\ddeexec\\Application",L"",ddeexecApplication);
    bool hasDdeexecTopic = readRegValueSZ(HKEY_CLASSES_ROOT,shellBasePath+L"\\ddeexec\\Topic",L"",ddeexecTopic);
    bool hasDdeexecIfExec = readRegValueSZ(HKEY_CLASSES_ROOT,shellBasePath+L"\\ddeexec\\IfExec",L"",ddeexecIfExec);
    if(!hasCommand)
    {
        return 1;
    }
    if(hasDdeexec)
    {
        if(!hasDdeexecApplication)
        {
            ddeexecApplication = command;
        }
        if(!hasDdeexecTopic)
        {
            ddeexecTopic = L"System";
        }
    }

    // build formatted command strings
    std::wstring formattedCommand;
    std::wstring formattedDdeexec;
    std::wstring formattedDdeexecIfExec;
    if(hasCommand)
    {
        if(!formatMessage(command,decodedUrl,L"",formattedCommand))
        {
            return 1;
        }
    }
    if(hasDdeexec)
    {
        if(!formatMessage(ddeexec,decodedUrl,L"",formattedDdeexec))
        {
            return 1;
        }
    }
    if(hasDdeexecIfExec)
    {
        if(!formatMessage(ddeexecIfExec,decodedUrl,L"",formattedDdeexecIfExec))
        {
            return 1;
        }
    }

    // try pure DDE first
    if(hasDdeexec)
    {
        if(executeDDE(formattedDdeexec,ddeexecApplication,ddeexecTopic))
        {
            return 0;
        }
    }

    // create a new process
    if(!executeCommand(formattedCommand))
    {
        return 1;
    }

    // send a DDE command after creating the process
    if(hasDdeexec)
    {
        const std::wstring& ddeCMD = hasDdeexecIfExec ? formattedDdeexecIfExec : formattedDdeexec;
        if(!executeDDE(ddeCMD,ddeexecApplication,ddeexecTopic))
        {
            return 1;
        }
    }

    return 0;
}
