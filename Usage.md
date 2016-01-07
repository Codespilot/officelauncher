# Plug-In Setup and Configuration #

## Mac OS-X ##

### Setup ###
Open the `OfficelauncherPlugIn.dmg` and Drag the "Office launcher Plug-In" icon on the "Internet PlugIns" icon. The Plug-In will be installed for all users on the machine.

### Configuration ###
The Plug-In displays a warning message when a file from the Internet is opened. To suppress this warning message, you need to set a preference value for each user. Execute this command in the Terminal to suppress this warning message:
```
defaults -currentHost write com.OfficelauncherPlugIn SuppressOpenWarning true
```
Enter this command to reactivate warning messages again:
```
defaults -currentHost write com.OfficelauncherPlugIn SuppressOpenWarning false
```
After this preference value has been set, you need to restart the browser.

## Microsoft Windows ##

### Setup ###
Run the `OfficeLauncherPlugin.msi` installer. The Plug-In is installed into the AppData folder in the user home. It does not require administrative privileges.

### Configuration ###
The Plug-In displays a warning message when a file from the Internet is opened.  To suppress this warning message, you need to set a DWORD value in the Registry for each user. Create the new Key `Office Launcher Plug-In` under `HKEY_CURRENT_USER\Software`. In this key, create a new DWORD value named `SuppressOpenWarning` and set its value to 1.