#
# Copyright (C) 2012-2013 Alfresco Software Limited.
#
# This file is part of Alfresco
#
# Alfresco is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Alfresco is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Alfresco. If not, see <http://www.gnu.org/licenses/>.
#
# Plugin Configuration file for Office Launcher Plug-In
#

set(PLUGIN_NAME "OfficeLauncherPlugIn")
set(PLUGIN_PREFIX "OLP")
set(COMPANY_NAME "OfficeLauncherOrg")

# ActiveX constants:
set(FBTYPELIB_NAME OfficeLauncherPlugInLib)
set(FBTYPELIB_DESC "OfficeLauncherPlugIn 1.0 Type Library")
set(IFBControl_DESC "OfficeLauncherPlugIn Control Interface")
set(FBControl_DESC "OfficeLauncherPlugIn Control Class")
set(IFBComJavascriptObject_DESC "OfficeLauncherPlugIn IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "OfficeLauncherPlugIn ComJavascriptObject Class")
set(IFBComEventSource_DESC "OfficeLauncherPlugIn IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID ff011ba3-dd68-5625-aca9-3e385b190850)
set(IFBControl_GUID 7cfdf382-75da-5f1b-826f-84dde98cd96c)
set(FBControl_GUID c9d851f2-9f71-576f-bbcc-b06225ef12ce)
set(IFBComJavascriptObject_GUID dc94edd0-821c-59bd-b9ac-d9787c82ccb1)
set(FBComJavascriptObject_GUID 71433ac7-1045-5865-bdc2-547f3566255e)
set(IFBComEventSource_GUID b73e995e-2981-5c3b-a675-d73d9c1c64f3)
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID 76b9ce55-9033-584e-8d31-9c7eef6d7ab4)
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID 491a18b0-b3fe-5ff9-a438-fbb4bbbba496)
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "OfficeLauncherOrg.OpenDocuments")
set(MOZILLA_PLUGINID "officelauncher.org/OfficeLauncherPlugIn")

# strings
set(FBSTRING_CompanyName "OfficeLauncher.org")
set(FBSTRING_PluginDescription "Launch productivity applications")
set(FBSTRING_PLUGIN_VERSION "0.8.4.0")
set(FBSTRING_LegalCopyright "Copyright 2013 OfficeLauncher.org")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "Office Launcher Plug-In")
set(FBSTRING_FileExtents "")
#if ( FB_PLATFORM_ARCH_32 )
#    set(FBSTRING_PluginName "Office Launcher Plug-In")  # No 32bit postfix to maintain backward compatability.
#else ( FB_PLATFORM_ARCH_32 )
#    set(FBSTRING_PluginName "Office Launcher Plug-In_${FB_PLATFORM_ARCH_NAME}")
#endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_PluginName "${PLUGIN_NAME}")
set(FBSTRING_MIMEType "application/x-officelauncher")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 0)
set(FBMAC_USE_COREGRAPHICS 0)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

#set(FBMAC_CODE_SIGNING_AUTHOROTY "enter-code-signing-authoroty-here")

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

#add_boost_library(filesystem)