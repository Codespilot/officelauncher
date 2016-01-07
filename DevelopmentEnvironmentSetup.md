# Introduction #

The OfficeLauncher browser Plug-In is built with the FireBreath cross-platform plugin architecture (http://firebreath.org).


# Setup development environment #

Follow thse steps depending on your operating system:
  * Get FireBreath. The latest plugin has been compiled with version 1.7.0-0-gdf8659e http://www.firebreath.org/display/documentation/Download
  * Prepare firebreath properly depending on the build platform (e.g. install cmake, wix, ...). See [this](http://www.firebreath.org/display/documentation/Building+FireBreath+Plugins#BuildingFireBreathPlugins-ConfiguringandBuildingonvariousplatforms) FireBreath page for details depending on your platform
  * Windows only: There is a problem with the latest WiX version, so that WiX cannot be detected by cmake. See [this](https://groups.google.com/forum/#!topic/firebreath-dev/04UjOJtyvV4) blog post for details. In short, you have to add this section in front of your  `prep*.cmd` files:
```
@echo off
setlocal
rem --- check for trailing backslash
set _test=%WIX:~-1,1%
if NOT %_test% == \ (
    goto OK
)
rem --- remove trailing backslash
set WIX=%WIX:~0,-1%
:OK
set WIX_ROOT_DIR=%WIX%
rem --- call the "prep" script
```
  * Create two new folders `fbprojects` and `fbbuild` next to the `firebreath` folder:
```
|
+-- firebreath
|
+-- fbprojects
|
+-- fbbuild
```
  * In the folder `fbprojects`, clone this repository in the folder `OfficeLauncherPlugIn`:
```
git clone https://code.google.com/p/officelauncher/ OfficeLauncherPlugIn
```
> Your directory structure should look like this:
```
|
+-- firebreath
|
+-- fbprojects
|      |
|      +-- OfficeLauncherPlugIn
|
+-- fbbuild
```
  * Run this command in the top folder:
> Windows:
```
firebreath\prep2010.cmd fbprojects fbbuild
```
> OS-X:
```
./firebreath/prepmac.sh fbprojects fbbuild
```
> This will prepare the Visual Studio (Windows) or XCode (OS-X) projects
  * Build the Plug-In:
    * Windows: Open the solution file `fbbuild\FireBreath.sln` in Visual Studio and build the solution. You will find the plugin and the installer in `fbbuild\bin\OfficelauncherPlugIn\Debug`
    * OS-X: Change into the `fbbuild` folder and run the command `xcodebuild`. You will find the plugin and the installer in `fbbuild/projects/OfficelauncherPlugIn/Debug`


# Release build #
Windows:

Switch "Solution configuration" to "Release"

OS-X:

run `xcodebuild -configuration Release`


# Code signing #
Release builds of this Plug-In should be signed with a valid code signing certificate. The build files have already been prepared to perform code signing during build. Adopt the projects as follows and re-build in Release mode:

Windows:

Add this command as "Post-Build Event" of project "OfficelauncherPlugIn" for configuration "Release":
```
"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\signtool.exe" sign /v /d "Office Launcher Plug-In" /du http://officelauncher.org /t http://timestamp.globalsign.com/scripts/timestamp.dll "$(TargetPath)"
```
Insert this command into the "Post Build Event" of project "OfficelauncherPlugIn\_WiXInstall" for configuration "Release" after the WiX linker:
```
"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Bin\signtool.exe" sign /v /d "Office Launcher Plug-In Installer" /du http://officelauncher.org /t http://timestamp.globalsign.com/scripts/timestamp.dll "<your-path-to-fbbuild>/bin/OfficeLauncherPlugIn/$(Configuration)/OfficeLauncherPlugIn.msi"
```

OS-X:

In `fbprojects/OfficelauncherPlugIn/PluginConfig.cmake`, uncomment the line `FBMAC_CODE_SIGNING_AUTHOROTY` and set the correct authority (must be in your keyring).

In `fbprojects/OfficelauncherPlugIn/Mac/projectDefs.cmake`, uncomment the line `include(Mac/codesigning.cmake)`. Re-run the `prepmac` script and re-build.