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
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "OfficeLauncherPlugIn.h"

#ifdef FB_WIN
#include "Win/PlatformDependentOfficeLauncher.h"
#endif
#ifdef FB_MACOSX
#include "Mac/PlatformDependentOfficeLauncher.h"
#endif

#ifndef H_OfficeLauncherPlugInAPI
#define H_OfficeLauncherPlugInAPI

#define MAX_URL_LENGTH 1024

class OfficeLauncherPlugInAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn OfficeLauncherPlugInAPI::OfficeLauncherPlugInAPI(const OfficeLauncherPlugInPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    OfficeLauncherPlugInAPI(const OfficeLauncherPlugInPtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
    {
        registerMethod("ViewDocument", make_method(this, &OfficeLauncherPlugInAPI::viewDocument));
        registerMethod("EditDocument", make_method(this, &OfficeLauncherPlugInAPI::editDocument));
        registerProperty("version", make_property(this, &OfficeLauncherPlugInAPI::get_version));
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn OfficeLauncherPlugInAPI::~OfficeLauncherPlugInAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~OfficeLauncherPlugInAPI() {};

    OfficeLauncherPlugInPtr getPlugin();

    long viewDocument(const std::string& url_utf8);
    long editDocument(const std::string& url_utf8);

    std::string get_version();

private:
    OfficeLauncherPlugInWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;
    PlatformDependentOfficeLauncher m_platformOfficeLauncher;

	bool confirmOpen(SimpleUri& decodedUri);

};

#endif // H_OfficeLauncherPlugInAPI