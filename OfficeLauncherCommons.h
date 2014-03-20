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

#ifndef H_OfficeLauncherCommons
#define H_OfficeLauncherCommons

namespace OfficeLauncherCommons
{

std::wstring utf8_to_wstring(const std::string& src);

std::string wstring_to_utf8(const std::wstring& src);

std::string urlDecodeToUtf8(const std::wstring &src);

std::wstring urlDecodeComplete(const std::wstring &src);

//std::wstring urlDecodeComponent(const std::wstring &src);

};

#endif // H_OfficeLauncherCommons