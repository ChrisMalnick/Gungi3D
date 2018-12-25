/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	WindowsUI.h
 *
 * Summary:	Invokes and handles various Windows based user interface components
 *
 * Origin:	N/A
 *
 * Legal:	Unregistered Copyright (C) 2018 Chris Malnick - All Rights Reserved
 *			Unauthorized duplication, reproduction, modification, and/or
 *			distribution is strictly prohibited
 *			All materials, including, but not limited to, code, resources
 *			(models, textures, etc.), documents, etc. are deliberately
 *			unlicensed
 * ============================================================================
 */

#ifndef WINDOWS_UI_H
#define WINDOWS_UI_H

#include "Windows/CommonItemDialog.h"
#include "Windows/NewDialog.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>

std::string UTF16toUTF8(const std::wstring &src)
{
	if (src.empty())
		return "";

	int src_size = static_cast<int>(src.size());
	int dest_size = WideCharToMultiByte(CP_UTF8, 0, &src[0], src_size, NULL, 0, NULL, NULL);

	std::string dest(dest_size, 0);
	WideCharToMultiByte(CP_UTF8, 0, &src[0], src_size, &dest[0], dest_size, NULL, NULL);

	return dest;
}

std::string KnownFolderPath(const KNOWNFOLDERID &rfid)
{
	IKnownFolderManager *pkfm = NULL;

	HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COM::KNOWN_FOLDER_MANAGER::INSTANCE::CREATE_FAILED" << std::endl;
		return NULL;
	}

	IKnownFolder *pkf = NULL;

	hr = pkfm->GetFolder(rfid, &pkf);

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COM::KNOWN_FOLDER_MANAGER::KNOWN_FOLDER::GET_FAILED" << std::endl;
		return NULL;
	}

	PWSTR pszPath = NULL;

	hr = pkf->GetPath(0, &pszPath);

	if (FAILED(hr))
	{
		std::cerr << "ERROR::COM::KNOWN_FOLDER::PATH::GET_FAILED" << std::endl;
		return NULL;
	}

	return UTF16toUTF8(pszPath);
}

// Windows dialog callbacks
// ------------------------
int* windows_new_callback(GLFWwindow *p_window_ptr)
{
	CNewDialog dialog;
	return dialog.Display(glfwGetWin32Window(p_window_ptr));
}

inline bool windows_exit_callback(GLFWwindow *p_window_ptr) { return WarningDialog(glfwGetWin32Window(p_window_ptr), L"Now Exiting", L"Do you wish to continue?"); }

std::string windows_open_callback(GLFWwindow *p_window_ptr)
{
	PWSTR path = FileDialog(glfwGetWin32Window(p_window_ptr), CLSID_FileOpenDialog);

	if (path == NULL)
		return "";

	PWSTR extension = std::wcsrchr(path, '.');

	if (std::wcscmp(extension, L".dat"))
	{
		ErrorDialog(glfwGetWin32Window(p_window_ptr), L"Incompatible File Type", L"Only files of type .dat are compatible.");
		return "";
	}

	return UTF16toUTF8(path);
}

std::string windows_save_callback(GLFWwindow *p_window_ptr)
{
	PWSTR path = FileDialog(glfwGetWin32Window(p_window_ptr), CLSID_FileSaveDialog);

	if (path == NULL)
		return "";
	
	return UTF16toUTF8(path);
}

#endif // WINDOWS_UI_H