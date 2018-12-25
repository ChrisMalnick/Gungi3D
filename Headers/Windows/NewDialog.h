/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	NewDialog.h
 *
 * Summary:	Creates, handles, and retrieves data from a custom Windows based
 *			dialog
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

#ifndef NEW_DIALOG_H
#define NEW_DIALOG_H

#include "../resource.h"

#include <windows.h>

#define ABORT -1

class CNewDialog
{
public:
	static BOOL CALLBACK Valid(_In_ HWND hwndDlg)
	{
		if (IsDlgButtonChecked(hwndDlg, IDC_RADIO2) == BST_CHECKED)
		{
			if (GetDlgItemInt(hwndDlg, IDC_EDIT1, NULL, FALSE) == NULL)
				return FALSE;
		}

		if (IsDlgButtonChecked(hwndDlg, IDC_RADIO4) == BST_CHECKED)
		{
			if (GetDlgItemInt(hwndDlg, IDC_EDIT2, NULL, FALSE) == NULL)
				return FALSE;
		}

		return TRUE;
	}

	static VOID CALLBACK Validate(_In_ HWND hwndDlg, _In_ INT nIDDlgItem)
	{
		BOOL bTranslated;
		INT nValue = GetDlgItemInt(hwndDlg, nIDDlgItem, &bTranslated, FALSE);

		if (bTranslated && nValue == NULL)
			SetDlgItemInt(hwndDlg, nIDDlgItem, 1, FALSE);
	}

	static VOID CALLBACK ShowTip(_In_ HWND hwndDlg, _In_ INT nIDDlgItem, _In_ LPCWSTR pszTitle, _In_ LPCWSTR pszText, _In_ INT ttiIcon)
	{
		EDITBALLOONTIP ebt;

		ebt.cbStruct = sizeof(EDITBALLOONTIP);
		ebt.pszTitle = pszTitle;
		ebt.pszText = pszText;
		ebt.ttiIcon = ttiIcon;

		SendDlgItemMessage(hwndDlg, nIDDlgItem, EM_SHOWBALLOONTIP, NULL, (LPARAM)&ebt);
	}

	static INT_PTR CALLBACK DialogProc(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_INITDIALOG:
			CheckRadioButton(hwndDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);

			SendDlgItemMessage(hwndDlg, IDC_EDIT1, EM_SETLIMITTEXT, sizeof(char), NULL);
			SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_SETRANGE, NULL, MAKELONG(9, 1));
			SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_SETPOS, NULL, 1);

			EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SPIN1), FALSE);

			CheckRadioButton(hwndDlg, IDC_RADIO3, IDC_RADIO4, IDC_RADIO3);

			SendDlgItemMessage(hwndDlg, IDC_EDIT2, EM_SETLIMITTEXT, sizeof(char), NULL);
			SendDlgItemMessage(hwndDlg, IDC_SPIN2, UDM_SETRANGE, NULL, MAKELONG(9, 1));
			SendDlgItemMessage(hwndDlg, IDC_SPIN2, UDM_SETPOS, NULL, 1);

			EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT2), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, IDC_SPIN2), FALSE);

			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_RADIO1:
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SPIN1), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDOK), Valid(hwndDlg));

				return TRUE;

			case IDC_RADIO2:
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT1), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SPIN1), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDOK), Valid(hwndDlg));

				return TRUE;

			case IDC_EDIT1:
				Validate(hwndDlg, IDC_EDIT1);
				EnableWindow(GetDlgItem(hwndDlg, IDOK), Valid(hwndDlg));

				return TRUE;

			case IDC_RADIO3:
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT2), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SPIN2), FALSE);
				EnableWindow(GetDlgItem(hwndDlg, IDOK), Valid(hwndDlg));

				return TRUE;

			case IDC_RADIO4:
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT2), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SPIN2), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, IDOK), Valid(hwndDlg));

				return TRUE;

			case IDC_EDIT2:
				Validate(hwndDlg, IDC_EDIT2);
				EnableWindow(GetDlgItem(hwndDlg, IDOK), Valid(hwndDlg));

				return TRUE;

			case IDOK:
				m_result[0] = m_result[1] = NULL;

				if (IsDlgButtonChecked(hwndDlg, IDC_RADIO2) == BST_CHECKED)
					m_result[0] = SendDlgItemMessage(hwndDlg, IDC_SPIN1, UDM_GETPOS, NULL, NULL);

				if (IsDlgButtonChecked(hwndDlg, IDC_RADIO4) == BST_CHECKED)
					m_result[1] = SendDlgItemMessage(hwndDlg, IDC_SPIN2, UDM_GETPOS, NULL, NULL);

				EndDialog(hwndDlg, wParam);
				return TRUE;

			case IDCANCEL:
				m_result[0] = m_result[1] = ABORT;

				EndDialog(hwndDlg, wParam);
				return TRUE;

			case IDCLOSE:
				m_result[0] = m_result[1] = ABORT;

				EndDialog(hwndDlg, wParam);
				return TRUE;
			}

			return TRUE;
		}

		return FALSE;
	}

	INT* Display(HWND hwndOwner)
	{
		DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hwndOwner, (DLGPROC)DialogProc);
		return this->m_result;
	}

private:
	static INT m_result[2];
};

INT CNewDialog::m_result[2];

#endif // NEW_DIALOG_H