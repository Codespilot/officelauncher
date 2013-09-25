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

 #ifndef H_OfficeLauncherPlugInErrorCodes
#define H_OfficeLauncherPlugInErrorCodes

#define OLP_ERROR_SUCCESS                    0L
#define OLP_ERROR_URL_TOO_LONG              -1
#define OLP_ERROR_USER_REJECTED             -2
#define OLP_ERROR_INVALID_URL               -3

#define OLP_ERROR_WIN_NOT_REGISTERED        -10
#define OLP_ERROR_WIN_PROGID_NOT_ENABLED    -11
#define OLP_ERROR_WIN_NO_COMMAND            -12
#define OLP_ERROR_WIN_FORMAT_STR_FAILED     -13
#define OLP_ERROR_WIN_CREATE_PROCESS_FAILED -14
#define OLP_ERROR_WIN_DDE_FAILED            -15

#define OLP_ERROR_MAC_UNHANDLED_FILE_TYPE   -10
#define OLP_ERROR_MAC_BUILD_EVENT_FAILED    -11
#define OLP_ERROR_MAC_CFSTRINGCREATE_FAILED -12

#endif // H_OfficeLauncherPlugInErrorCodes