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

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "PlatformDependentOfficeLauncher.h"
#include "../OfficeLauncherPlugInErrorCodes.h"
#include "../SimpleUri.h"
#include <strsafe.h>
#include <string>
#include <iostream>


PlatformDependentOfficeLauncher::PlatformDependentOfficeLauncher()
{
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
}

long PlatformDependentOfficeLauncher::viewDocument(const std::string& url_utf8)
{
    return openDocument(url_utf8, true);
}

long PlatformDependentOfficeLauncher::editDocument(const std::string& url_utf8)
{
    return openDocument(url_utf8, false);
}

bool PlatformDependentOfficeLauncher::suppressOpenWarning(const std::string& url_utf8)
{
    DWORD value = 0;
    bool readResult = readRegValueDWORD(HKEY_CURRENT_USER, L"Software\\Office Launcher Plug-In", L"SuppressOpenWarning", value);
    return readResult && (value == 1);
}

long PlatformDependentOfficeLauncher::openDocument(const std::string& url_utf8, bool readOnly)
{

    // get file  extension
    const std::wstring url = FB::utf8_to_wstring(url_utf8);
    SimpleUri decodedUri(url);
    if(!decodedUri.isValid())
    {
        return OLP_ERROR_INVALID_URL;
    }
    if(!decodedUri.isHttpOrHttpsSchema())
    {
        return OLP_ERROR_INVALID_URL;
    }

    // get ProgId for file extension from registry
    std::wstring progId;
    if(!readRegValueSZ(HKEY_CLASSES_ROOT,decodedUri.getFileExtension(),L"",progId))
    {
        return OLP_ERROR_WIN_NOT_REGISTERED;
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
        return OLP_ERROR_WIN_PROGID_NOT_ENABLED;
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
        return OLP_ERROR_WIN_NO_COMMAND;
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
        if(!formatMessage(command,url,L"",formattedCommand))
        {
            return OLP_ERROR_WIN_FORMAT_STR_FAILED;
        }
    }
    if(hasDdeexec)
    {
        if(!formatMessage(ddeexec,url,L"",formattedDdeexec))
        {
            return OLP_ERROR_WIN_FORMAT_STR_FAILED;
        }
    }
    if(hasDdeexecIfExec)
    {
        if(!formatMessage(ddeexecIfExec,url,L"",formattedDdeexecIfExec))
        {
            return OLP_ERROR_WIN_FORMAT_STR_FAILED;
        }
    }

    // try pure DDE first
    if(hasDdeexec)
    {
        if(executeDDE(formattedDdeexec,ddeexecApplication,ddeexecTopic))
        {
            return OLP_ERROR_SUCCESS;
        }
    }

    // create a new process
    if(!executeCommand(formattedCommand))
    {
        return OLP_ERROR_WIN_CREATE_PROCESS_FAILED;
    }

    // send a DDE command after creating the process
    if(hasDdeexec)
    {
        const std::wstring& ddeCMD = hasDdeexecIfExec ? formattedDdeexecIfExec : formattedDdeexec;
        if(!executeDDE(ddeCMD,ddeexecApplication,ddeexecTopic))
        {
            return OLP_ERROR_WIN_DDE_FAILED;
        }
    }

    return OLP_ERROR_SUCCESS;
}

bool PlatformDependentOfficeLauncher::readRegValueSZ(HKEY key, const std::wstring& path, const std::wstring& name, std::wstring& value)
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

bool PlatformDependentOfficeLauncher::readRegValueDWORD(HKEY key, const std::wstring& path, const std::wstring& name, DWORD& value)
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

bool PlatformDependentOfficeLauncher::progIdEnabled(const std::wstring& progid)
{
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

bool PlatformDependentOfficeLauncher::formatMessage(const std::wstring& source, const std::wstring& param1, const std::wstring& param2, std::wstring& dest)
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

bool PlatformDependentOfficeLauncher::formatMessageOld(const std::wstring& source, std::wstring& destMessage, ...)
{
    size_t tempEnvLength = source.size() + 1;  // a rough guess: there are no environment args in the command
    wchar_t *tempEnv = (wchar_t*)malloc(tempEnvLength * sizeof(wchar_t));
    if(tempEnv == NULL)
    {
        return false;
    }
    DWORD result;
    result = ExpandEnvironmentStrings(source.c_str(),tempEnv,tempEnvLength);
    if(result == 0)
    {
        if(tempEnv)
        {
            free(tempEnv);
        }
        return false;
    }
    if(result > tempEnvLength)
    {
        // buffer is too small
        tempEnvLength = result;
        wchar_t *newEnvBuffer = (wchar_t*)realloc(tempEnv, tempEnvLength * sizeof(wchar_t));
        if(newEnvBuffer == NULL)
        {
            if(tempEnv)
            {
                free(tempEnv);
            }
            return false;
        }
        tempEnv = newEnvBuffer;
        result = ExpandEnvironmentStrings(source.c_str(),tempEnv,tempEnvLength);
        if(result == 0)
        {
            if(tempEnv)
            {
                free(tempEnv);
            }
            return false;
        }
    }
    // DWORD_PTR pArgs[] = { (DWORD_PTR)parameter1.c_str(), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    // size_t tempMessageLength = wcslen(tempEnv) + parameter1.size() + 1; // a rough guess
    size_t tempMessageLength = 1024; // a rough guess
    wchar_t *tempMessage = (wchar_t*)malloc(tempMessageLength * sizeof(wchar_t));
    va_list vaList;
    va_start(vaList,destMessage);
    result = FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, // DWORD dwFlags
                           tempEnv,                                   // LPCVOID lpSource
                           0,                                         // DWORD dwMessageId
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // DWORD dwLanguageId
                           tempMessage,                               // LPWSTR lpBuffer
                           tempMessageLength,                         // DWORD nSize
                           &vaList); // va_list *Arguments
    va_end(vaList);
    if(result == 0)
    {
        if(tempEnv)
        {
            free(tempEnv);
        }
        if(tempMessage)
        {
            free(tempMessage);
        }
        return false;
    }
    if(result > tempMessageLength)
    {
        // buffer too small
        tempMessageLength = result;
        wchar_t *newMessageBuffer = (wchar_t*)realloc(tempMessage, tempMessageLength * sizeof(wchar_t));
        if(newMessageBuffer == NULL)
        {
            if(tempEnv)
            {
                free(tempEnv);
            }
            if(tempMessage)
            {
                free(tempMessage);
            }
            return false;
        }
        tempMessage = newMessageBuffer;
        va_list vaList2;
        va_start(vaList2,destMessage);
        result = FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, // DWORD dwFlags
                               tempEnv,                                   // LPCVOID lpSource
                               0,                                         // DWORD dwMessageId
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // DWORD dwLanguageId
                               tempMessage,                               // LPWSTR lpBuffer
                               tempMessageLength,                         // DWORD nSize
                               &vaList);                                  // va_list *Arguments
        va_end(vaList2);
        if(result == 0)
        {
            if(tempEnv)
            {
                free(tempEnv);
            }
            if(tempMessage)
            {
                free(tempMessage);
            }
            return false;
        }
    }
    if(tempEnv)
    {
        free(tempEnv);
    }
    destMessage = tempMessage;
    if(tempMessage)
    {
        free(tempMessage);
    }
    return true;
}

bool PlatformDependentOfficeLauncher::executeCommand(const std::wstring& commandLine)
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

bool PlatformDependentOfficeLauncher::executeDDE(const std::wstring& ddeCommand, const std::wstring& application, const std::wstring& topic)
{
    bool result = false;

    const std::string debug_ddeCommand = FB::wstring_to_utf8(ddeCommand);

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
