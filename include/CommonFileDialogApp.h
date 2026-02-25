#pragma once

#include <windows.h>      // For common windows data types and function headers
#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <commctrl.h>
#include <new>

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")

const COMDLG_FILTERSPEC c_rgAnySameType = { L"Any Document (*.*)", L"*.*" };

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
    {L"Word Document (*.doc)",       L"*.doc"},
    {L"Web Page (*.htm; *.html)",    L"*.htm;*.html"},
    {L"Text Document (*.txt)",       L"*.txt"},
    {L"All Documents (*.*)",         L"*.*"}
};

// Indices of file types
#define INDEX_WORDDOC 1
#define INDEX_WEBPAGE 2
#define INDEX_TEXTDOC 3

// Controls
#define CONTROL_GROUP           2000
#define CONTROL_RADIOBUTTONLIST 2
#define CONTROL_RADIOBUTTON1    1
#define CONTROL_RADIOBUTTON2    2       // It is OK for this to have the same ID as CONTROL_RADIOBUTTONLIST,
                                        // because it is a child control under CONTROL_RADIOBUTTONLIST

// IDs for the Task Dialog Buttons
#define IDC_BASICFILEOPEN                       100
#define IDC_ADDITEMSTOCUSTOMPLACES              101
#define IDC_ADDCUSTOMCONTROLS                   102
#define IDC_SETDEFAULTVALUESFORPROPERTIES       103
#define IDC_WRITEPROPERTIESUSINGHANDLERS        104
#define IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS 105

class CDialogEventHandler : public IFileDialogEvents,
    public IFileDialogControlEvents
{
public:
    CDialogEventHandler();

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);

    IFACEMETHODIMP_(ULONG) AddRef();

    IFACEMETHODIMP_(ULONG) Release();

    // IFileDialogEvents methods
    IFACEMETHODIMP OnFileOk(IFileDialog*);
    IFACEMETHODIMP OnFolderChange(IFileDialog*);
    IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*);
    IFACEMETHODIMP OnHelp(IFileDialog*);
    IFACEMETHODIMP OnSelectionChange(IFileDialog*);
    IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*);
    IFACEMETHODIMP OnTypeChange(IFileDialog* pfd);
    IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*);

    // IFileDialogControlEvents methods
    IFACEMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc, DWORD dwIDCtl, DWORD dwIDItem);
    IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD);
    IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD, BOOL);
    IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD);

    // Instance creation helper
    static HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv);

private:
    ~CDialogEventHandler() {};
    long _cRef;
};

class FileHelperClass {
public:
    // A helper function that converts UNICODE data to ANSI and writes it to the given file.
    // We write in ANSI format to make it easier to open the output file in Notepad.
    static HRESULT _WriteDataToFile(HANDLE hFile, PCWSTR pszDataIn);

    //Helper function to write property / value into a custom file format.
    //We are inventing a dummy format here:
    //[APPDATA]
    //xxxxxx
    //[ENDAPPDATA]
    //[PROPERTY]foo=bar[ENDPROPERTY]
    //[PROPERTY]foo2=bar2[ENDPROPERTY]
    static HRESULT _WritePropertyToCustomFile(PCWSTR pszFileName, PCWSTR pszPropertyName, PCWSTR pszValue);

    // Helper function to write dummy content to a custom file format.
    //
    // We are inventing a dummy format here:
    // [APPDATA]
    // xxxxxx
    // [ENDAPPDATA]
    // [PROPERTY]foo=bar[ENDPROPERTY]
    // [PROPERTY]foo2=bar2[ENDPROPERTY]
    static HRESULT _WriteDataToCustomFile(PCWSTR pszFileName);
};
