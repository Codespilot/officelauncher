# About #

The OfficeLauncher.org project comes with a JavaScript component that can be used to integrate OfficeLauncher into your own web applications. You can get this JavaScript component [here](https://code.google.com/p/officelauncher/source/browse/OfficeLauncher.js).

This JS component is capable of loading the original Microsoft Plug-In or the Officelauncher.org Plug-In with both technologies, either Active-X or NPAPI. It detects the client environment (browser, operating system …) and chooses the best matching method based on a customizable set of rules.


# Integration #

  1. Add `OfficeLauncher.js` to your web page
  1. Instantiate the OfficeLauncher:
```
    var ol = new OfficeLauncher();
```
  1. Open a document by URL for viewing
```
    ol.ViewDocument("http://example.com/foo/Document.docx");
```
> or editing
```
    ol.EditDocument("http://example.com/foo/Document.docx");
```
  1. Check for success
```
    if(ol.EditDocument("http://example.com/foo/Document.docx"))
    {
        alert("Successful");
    }
    else
    {
        alert("Failed");
    }
```


# Debug #

You can activate console logging in the OfficeLauncher object. This will write debug information to the JavaScript web console:
```
    ol.setConsoleLoggingEnabled(true);
```


# Examples / Test #

Please check the file `TestOfficeLauncher.html` in the OfficeLauncher project for reference.


# Configuration #
The OfficeLauncher JavaScript component decides which method to use based on a customizable rule set. This set of rules can be extended or replaced by your own rules. This is done by
```
    ol.addRules("npapi.firefox=sp,ol;npapi.chrome=ol")
```
or
```
    ol.setRules("ol,sp")
```
Both methods take a rule set definition string as input. This string is made up of a semicolon separated list of single rules:
```
    ruleSet = rule | rule “;” ruleSet
    Examples: rule1;rule2;rule3 or rule1
```
Each rule consists of a selector and a list of methods:
```
    rule = selector "=" listOfMethods
    listOfMethods = method | method "," listOfMethods
    Examples: ol,sp or ol
```
Valid methods are:
```
    "ol" The OfficeLauncher.org Plug-In
    "sp" Microsoft’s SharePoint Browser Plug-In
```
The selector is a point separated list of narrowing conditions defining the environment. It is made up of:
```
    selector = technology “.” browser “.” operatingSystem
```
with
```
    technology = “ax” | “npapi” (for ActiveX or Netscape Plug-In API)
    browser = “firefox” | “ie” | “chrome” | “safari”
    operatingSystem = “win” | “mac”
```
An empty selector is allowed and catches all. If the selector is empty, the equals sign between selector and listOfMethods can be omitted.

The set of rules is searched for a matching rule starting from the most specific selector containing all three parts down to the empty selector. When a matching rule is found, the list of methods is searched from left to right until a  plug-in is found that is available on the current system.

Examples:
```
npapi.chrome.mac=ol;sp,ol
```
Use only the OfficeLauncher Plug-In in Chrome on Mac. On all other platforms, try to use the Microsoft Plug-In first and if not available use the Officelauncher Plug-In.
```
sp,ol;npapi=ol,sp;npapi.chrome.mac=ol;npapi.firefox=sp
```
Use only the Microsoft Plug-In in Firefox on all operating systems. In Chrome on Mac, use only the OfficeLauncher Plug-In. In all other NPAPI based Browsers (Opera, Safari), try to use the OfficeLauncher Plug-In first and if not available try to use the Microsoft Plug-In. In all other environments, try to use Microsoft’s Plug-In first and if not available use the OfficeLauncher Plug-In.