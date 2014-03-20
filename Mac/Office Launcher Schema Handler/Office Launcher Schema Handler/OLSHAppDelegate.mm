//
//  OLSHAppDelegate.m
//  Office Launcher Schema Handler
//
//  Created by Stefan Kopf on 22.02.14.
//  Copyright (c) 2014 OfficeLauncher.org. All rights reserved.
//

#import "OLSHAppDelegate.h"

#include <string>
#include <map>
#include <sstream>
#include "SimpleUri.h"
#include "utf8.h"
#include "OfficeLauncherCommons.h"
#include "OfficeLauncherPlugInErrorCodes.h"
#include "PlatformDependentOfficeLauncher.h"

using namespace OfficeLauncherCommons;

#define MAX_URL_LENGTH 1024


@implementation OLSHAppDelegate

std::wstring fromNSString(NSString* in)
{
    NSStringEncoding encodingWcharT = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
    NSData *inData = [in dataUsingEncoding:encodingWcharT];
    return std::wstring((wchar_t*)[inData bytes], [inData length] / sizeof(wchar_t));
}

NSString* toNSString(const std::wstring& in)
{
    NSStringEncoding encodingWcharT = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
    char* data = (char*)in.data();
    size_t size = in.size() * sizeof(wchar_t);
    return [[NSString alloc] initWithBytes:data length:size encoding:encodingWcharT];
}

bool confirmOpen(SimpleUri& decodedUri)
{
    std::wstring msg = L"File name: ";
    msg.append(urlDecodeComplete(decodedUri.getFilename()));
    msg.append(L"\nFrom: ");
    msg.append(urlDecodeComplete(decodedUri.getServer()));
    msg.append(L"\n\nSome files can harm your computer.");
    msg.append(L" If you do not fully trust the source, do not open the file.");
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:@"Do you want to open this file?"];
    [alert setInformativeText:toNSString(msg)];
    [alert setAlertStyle:NSWarningAlertStyle];
    return [alert runModal] == NSAlertFirstButtonReturn;
}

void errorMessage(const std::wstring& message)
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:@"Error"];
    [alert setInformativeText:toNSString(message)];
    [alert setAlertStyle:NSCriticalAlertStyle];
    [alert runModal];
}

void handleSchemaUrl(const std::wstring& schemaHandlerUri)
{
    
    PlatformDependentOfficeLauncher platformOfficeLauncher;
    
    bool readOnly = false;
    std::wstring encodedUrl;
    if(schemaHandlerUri.length() < 21)
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return;
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
        return;
    }
    
    if(encodedUrl.size() > MAX_URL_LENGTH)
    {
        errorMessage(L"URL too long.");
        return;
    }
    SimpleUri uri(encodedUrl);
    if(!uri.isValid())
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return;
    }
    if(!uri.isHttpOrHttpsSchema())
    {
        errorMessage(L"Invalid Office-Launcher URL.");
        return;
    }
    if(!( platformOfficeLauncher.suppressOpenWarning(uri) || confirmOpen(uri)))
    {
        return;
    }
    int result = platformOfficeLauncher.openDocument(encodedUrl, readOnly);
    if(OLP_ERROR_SUCCESS != result)
    {
        errorMessage(L"Failed starting Microsoft Office.");
        return;
    }
}

-(void)applicationWillFinishLaunching:(NSNotification *)notification
{
    [[NSAppleEventManager sharedAppleEventManager] setEventHandler:self andSelector:@selector(handleAppleEvent:withReplyEvent:) forEventClass:kInternetEventClass andEventID:kAEGetURL];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
}

- (void)handleAppleEvent:(NSAppleEventDescriptor *)inEvent withReplyEvent:(NSAppleEventDescriptor *)replyEvent
{
    NSString *uriNsString = [[inEvent paramDescriptorForKeyword:keyDirectObject] stringValue];
    handleSchemaUrl(fromNSString(uriNsString));
    [[NSApplication sharedApplication] terminate:nil];
}

@end
