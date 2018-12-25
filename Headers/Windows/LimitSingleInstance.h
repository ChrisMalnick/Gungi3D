/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	LimitSingleInstance.h
 *
 * Summary:	Limits Windows based operating systems to running one instance of
 *			the application at a time
 *
 * Origin:	This code is modified from Microsoft's Knowledge Base (KB)
 *			article Q243953
 *			The unmodified source code can be found at the following URL:
 *			https://support.microsoft.com/en-us/help/243953
 *
 * Legal:	Unregistered Copyright (C) 2018 Chris Malnick - All Rights Reserved
 *			Unauthorized duplication, reproduction, modification, and/or
 *			distribution is strictly prohibited
 *			All materials, including, but not limited to, code, resources
 *			(models, textures, etc.), documents, etc. are deliberately
 *			unlicensed
 * ============================================================================
 */

#ifndef LIMIT_SINGLE_INSTANCE_H
#define LIMIT_SINGLE_INSTANCE_H

class CLimitSingleInstance
{
public:
	// Shared name may result in external conflict
	CLimitSingleInstance(TCHAR *pszMutex)
	{
		this->m_hMutex = CreateMutex(NULL, FALSE, pszMutex);
		this->m_dwLastError = GetLastError();
	}

	virtual ~CLimitSingleInstance()
	{
		if (this->m_hMutex)
		{
			CloseHandle(this->m_hMutex);
			this->m_hMutex = NULL;
		}
	}

	inline BOOL Running() { return (this->m_dwLastError == ERROR_ALREADY_EXISTS); }

private:
	DWORD m_dwLastError;
	HANDLE m_hMutex;
};

#endif // LIMIT_SINGLE_INSTANCE_H