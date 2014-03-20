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

#include "PlatformDependentOfficeLauncher.h"
#include "../OfficeLauncherPlugInErrorCodes.h"
#include "../OfficeLauncherCommons.h"
#include "utf8.h"
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>

using namespace OfficeLauncherCommons;

PlatformDependentOfficeLauncher::PlatformDependentOfficeLauncher()
{
    m_extensionToBundleMap[".doc"]  = "com.microsoft.Word";
    m_extensionToBundleMap[".docx"] = "com.microsoft.Word";
    m_extensionToBundleMap[".docm"] = "com.microsoft.Word";
    m_extensionToBundleMap[".dot"]  = "com.microsoft.Word";
    m_extensionToBundleMap[".dotx"] = "com.microsoft.Word";
    m_extensionToBundleMap[".dotm"] = "com.microsoft.Word";
    m_extensionToBundleMap[".xls"]  = "com.microsoft.Excel";
    m_extensionToBundleMap[".xlsx"] = "com.microsoft.Excel";
    m_extensionToBundleMap[".xlsb"] = "com.microsoft.Excel";
    m_extensionToBundleMap[".xlsm"] = "com.microsoft.Excel";
    m_extensionToBundleMap[".xlt"]  = "com.microsoft.Excel";
    m_extensionToBundleMap[".xltx"] = "com.microsoft.Excel";
    m_extensionToBundleMap[".xltm"] = "com.microsoft.Excel";
    m_extensionToBundleMap[".ppt"]  = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".pptx"] = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".pot"]  = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".potx"] = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".pptm"] = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".potm"] = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".pps"]  = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".ppsx"] = "com.microsoft.Powerpoint";
    m_extensionToBundleMap[".ppsm"] = "com.microsoft.Powerpoint";
}

bool PlatformDependentOfficeLauncher::suppressOpenWarning(const SimpleUri& decodedUri)
{
    Boolean resultValid;
    Boolean result = CFPreferencesGetAppBooleanValue(CFSTR("SuppressOpenWarning"), CFSTR("com.OfficeLauncherPlugIn"), &resultValid);
    return resultValid && result;
}

int PlatformDependentOfficeLauncher::openDocument(const std::wstring& encodedUrl, bool readOnly)
{
    // get file extension
    SimpleUri uri(encodedUrl);
    if(!uri.isValid())
    {
        return OLP_ERROR_INVALID_URL;
    }
    if(!uri.isHttpOrHttpsSchema())
    {
        return OLP_ERROR_INVALID_URL;
    }

    // get bundleId of associated application
    const std::string url_extension = urlDecodeToUtf8(uri.getFileExtension());
    std::map<std::string, std::string>::iterator itBundleIds = m_extensionToBundleMap.find(url_extension);
    if(itBundleIds == m_extensionToBundleMap.end())
    {
        return OLP_ERROR_MAC_UNHANDLED_FILE_TYPE;
    }
    std::string associatedBundleId = (*itBundleIds).second;

    // convert wstring URL to UniChar string; there is no utf32to16 :-(
    std::basic_string<UniChar> url_unichar;
    std::string url_utf8 = wstring_to_utf8(encodedUrl);
    utf8::utf8to16(url_utf8.begin(), url_utf8.end(), std::back_inserter(url_unichar));
    const UniChar* url_uni = url_unichar.c_str();

    // build the AppleEvent to be sent to the target application
    OSStatus status;
    AppleEvent event;
    status = AEBuildAppleEvent(kCoreEventClass,
                               kAEOpenDocuments,
                               typeApplicationBundleID,
                               associatedBundleId.c_str(),
                               associatedBundleId.length(),
                               kAutoGenerateReturnID,
                               kAnyTransactionID,
                               &event,
                               NULL,
                               "'----':'utxt'(@),'SpFl':'bool'(1),'RdOn':'bool'(@)",
                               url_unichar.length() * 2,
                               url_uni,
                               readOnly);
    if(status != 0)
    {
        return OLP_ERROR_MAC_BUILD_EVENT_FAILED;
    }

    // find the application
    CFStringRef cfsBundleId = CFStringCreateWithCString(NULL, associatedBundleId.c_str(), kCFStringEncodingASCII);
    if(cfsBundleId == NULL)
    {
        AEDisposeDesc(&event);
        return OLP_ERROR_MAC_CFSTRINGCREATE_FAILED;
    }
    FSRef appRef;
    status = LSFindApplicationForInfo(kLSUnknownCreator,
                                      cfsBundleId,
                                      NULL,
                                      &appRef,
                                      NULL);
    CFRelease(cfsBundleId);
    if(status != 0)
    {
        AEDisposeDesc(&event);
        return status;
    }

    // launch the application
    LSApplicationParameters params = {
        0,  // version
        kLSLaunchDefaults, // flags
        &appRef, // application
        NULL, // asyncLaunchRefCon
        NULL, // environment
        NULL, // argv
        &event // initialEvent
    };
    status = LSOpenApplication(&params, NULL);
    if(status != 0)
    {
        AEDisposeDesc(&event);
        return status;
    }

    // dispose event and return result
    AEDisposeDesc(&event);

    // return status;
    return OLP_ERROR_SUCCESS;
}