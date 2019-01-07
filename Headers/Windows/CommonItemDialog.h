/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	CommonItemDialog.h
 * 
 * Summary:	Creates, handles, and retrieves data from standard Windows based
 *		dialogs
 * 
 * Origin:	This code is modified from Microsoft's CommonFileDialogApp.cpp -
 *		part of a collection of open source samples on Windows programming
 *		The unmodified source code can be found at the following URL:
 *		https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/winui/shell/appplatform/commonfiledialog/CommonFileDialogApp.cpp
 * 
 * Legal:	Unregistered Copyright (C) 2018 Chris Malnick - All Rights Reserved
 *		Unauthorized duplication, reproduction, modification, and/or
 *		distribution is strictly prohibited
 *		All materials, including, but not limited to, code, resources
 *		(models, textures, etc.), documents, etc. are deliberately
 *		unlicensed
 * ============================================================================
 */

#ifndef COMMON_ITEM_DIALOG_H
#define COMMON_ITEM_DIALOG_H

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

#include <shlobj.h>
#include <shlwapi.h>

#include <iostream>

// File type indicies
// ------------------
#define INDEX_DAT 1

// File types
// ----------
const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
	{ L"Game Data (*.dat)", L"*.dat" },
	{ L"All Documents (*.*)", L"*.*" },
};

// File dialog event handler
// -------------------------
class CDialogEventHandler : public IFileDialogEvents, public IFileDialogControlEvents
{
public:
	// IUnknown methods
	// ----------------
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(CDialogEventHandler, IFileDialogEvents),
			QITABENT(CDialogEventHandler, IFileDialogControlEvents),
			{ 0 },
#pragma warning(suppress:4838)
		};

		return QISearch(this, qit, riid, ppv);
	}

	inline IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&this->m_cRef); }

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&this->m_cRef);

		if (!cRef)
			delete this;

		return cRef;
	}

	// IFileDialogEvents methods
	// -------------------------
	inline IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
	inline IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
	inline IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
	inline IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
	inline IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
	inline IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
	inline IFACEMETHODIMP OnTypeChange(IFileDialog *pfd) { return S_OK; };
	inline IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

	// IFileDialogControlEvents methods
	// --------------------------------
	inline IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; };
	inline IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
	inline IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
	inline IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

	CDialogEventHandler() : m_cRef(1) {};
private:
	virtual ~CDialogEventHandler() {};
	long m_cRef;
};

// Instance creation helper
// ------------------------
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
	*ppv = NULL;

	CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
	HRESULT hr = (pDialogEventHandler ? S_OK : E_OUTOFMEMORY);

	if (SUCCEEDED(hr))
	{
		hr = pDialogEventHandler->QueryInterface(riid, ppv);
		pDialogEventHandler->Release();
	}

	return hr;
}

// Windows dialogs
// ---------------
PWSTR FileDialog(HWND hwndOwner, REFCLSID rclsid)
{
	// Cocreate dialog object
	// ----------------------
	IFileDialog *pfd = NULL;

	HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::INSTANCE::CREATE_FAILED" << std::endl;
		return NULL;
	}

	// Create event handler object
	// ---------------------------
	IFileDialogEvents *pfde = NULL;

	hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::EVENT_HANDLER::CREATE_FAILED" << std::endl;
		return NULL;
	}

	// Hook event handler object to dialog object
	// ------------------------------------------
	DWORD dwCookie;

	hr = pfd->Advise(pfde, &dwCookie);

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::EVENT_HANDLER::HOOK_FAILED" << std::endl;
		return NULL;
	}

	// Set dialog options
	// ------------------
	DWORD dwFlags;

	hr = pfd->GetOptions(&dwFlags); // Before setting options, get pre-existing ones to avoid overriding them

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::OPTIONS::GET_FAILED" << std::endl;
		return NULL;
	}

	hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM); // Get shell items only for file system items

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::OPTIONS::SET_FAILED" << std::endl;
		return NULL;
	}

	hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes); // Set file types to display only (1-based array)

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::FILE_TYPES::SET_FAILED" << std::endl;
		return NULL;
	}

	hr = pfd->SetFileTypeIndex(INDEX_DAT); // Set selected file type index

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::FILE_TYPE_INDEX::SET_FAILED" << std::endl;
		return NULL;
	}

	hr = pfd->SetDefaultExtension(L"dat"); // Set default extension

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::DEFAULT_EXTENSION::SET_FAILED" << std::endl;
		return NULL;
	}

	// Display dialog
	// --------------
	hr = pfd->Show(hwndOwner);

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::SHOW_FAILED" << std::endl;
		return NULL;
	}

	IShellItem *psiResult;

	hr = pfd->GetResult(&psiResult); // Obtain result when appropriate button is clicked

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_DIALOG::RESULT::GET_FAILED" << std::endl;
		return NULL;
	}

	// Get file path
	// -------------
	PWSTR pszFilePath = NULL;

	hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COMMON_ITEM_DIALOG::FILE_PATH::GET_FAILED" << std::endl;
		return NULL;
	}

	CoTaskMemFree(pszFilePath);

	psiResult->Release();

	pfd->Unadvise(dwCookie); // Unhook event handler

	pfde->Release();
	pfd->Release();

	return pszFilePath;
}

BOOL WarningDialog(HWND hwndOwner, PCWSTR pszMainInstruction, PCWSTR pszContent)
{
	int nButton;
	TaskDialog(hwndOwner, NULL, L"Warning", pszMainInstruction, pszContent, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, TD_WARNING_ICON, &nButton);

	return (nButton == IDYES);
}

inline VOID ErrorDialog(HWND hwndOwner, PCWSTR pszMainInstruction, PCWSTR pszContent) { TaskDialog(hwndOwner, NULL, L"Error", pszMainInstruction, pszContent, TDCBF_CLOSE_BUTTON, TD_ERROR_ICON, NULL); }

#endif // COMMON_ITEM_DIALOG_H
