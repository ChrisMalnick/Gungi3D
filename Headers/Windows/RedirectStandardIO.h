/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	RedirectStandardIO.h
 *
 * Summary:	Redirects standard input and output to the parent console and
 *			parses and verifies command line arguments
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

#ifndef REDIRECT_STANDARD_IO
#define REDIRECT_STANDARD_IO

#include <shellapi.h>

#include <ctime>

// Returns whether or not specified wide string only contains digits
BOOL Numeric(std::wstring wstr)
{
	for (auto &elem : wstr)
	{
		if (!isdigit(elem))
			return false;
	}

	return true;
}

// Parse command-line arguments and check for debug flag
BOOL Debug()
{
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	for (int i = 1; i < argc; ++i)
	{
		if (wcscmp(argv[i], L"--debug") == 0)
			return true;
	}

	return false;
}

// Parse command-line arguments, check for seed flag, and validate seed value
// Returns current time if no valid seed is specified
UINT Seed()
{
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	for (int i = 1; i < argc - 1; ++i)
	{
		if (wcscmp(argv[i], L"--seed") == 0)
		{
			if (Numeric(argv[i + 1]))
			{
				ULONGLONG seed = std::stoull(argv[i + 1]);
				
				if (seed <= UINT_MAX)
					return static_cast<UINT>(seed);
			}
		}
	}

	return static_cast<UINT>(std::time(NULL));
}

// Redirect stdout and stderr to parent console
VOID RedirectOutput()
{
	if (AttachConsole(ATTACH_PARENT_PROCESS) == 0)
	{
		std::cerr << "ERROR::REDIRECT_STANDARD_IO::ATTACH_CONSOLE::FAILED" << std::endl;
		return;
	}

	FILE *pFile;

	if (freopen_s(&pFile, "CONOUT$", "w", stdout))
	{
		std::cerr << "ERROR::REDIRECT_STANDARD_IO::STDOUT::FAILED" << std::endl;
		return;
	}

	if (freopen_s(&pFile, "CONOUT$", "w", stderr))
	{
		std::cerr << "ERROR::REDIRECT_STANDARD_IO::STDERR::FAILED" << std::endl;
		return;
	}

	std::cout << std::endl;
}

#endif // REDIRECT_STANDARD_IO