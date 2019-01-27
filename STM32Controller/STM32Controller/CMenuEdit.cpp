#include "stdafx.h"
#include "CMenuEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MES_COPY        _T("&Copy")
#define MES_PASTE       _T("&Paste")
#define MES_SELECTALL   _T("Select &All")
#define MES_CLEARALL    _T("Clear &All")

#define ME_SELECTALL    WM_USER + 0x7000
#define ME_CLEARALL		WM_USER + 0x7001	

BEGIN_MESSAGE_MAP(CMenuEdit, CEdit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CMenuEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	SetFocus();
	CMenu menu;
	menu.CreatePopupMenu();
	BOOL bReadOnly = GetStyle() & ES_READONLY;
	DWORD flags = CanUndo() && !bReadOnly ? 0 : MF_GRAYED;

	DWORD sel = GetSel();
	flags = LOWORD(sel) == HIWORD(sel) ? MF_GRAYED : 0;
	menu.InsertMenu(2, MF_BYPOSITION | flags, WM_COPY,	MES_COPY);

	flags = IsClipboardFormatAvailable(CF_TEXT) &&
		!bReadOnly ? 0 : MF_GRAYED;
	menu.InsertMenu(4, MF_BYPOSITION | flags, WM_PASTE,
		MES_PASTE);

	menu.InsertMenu(6, MF_BYPOSITION | MF_SEPARATOR);

	int len = GetWindowTextLength();
	flags = (!len || (LOWORD(sel) == 0 && HIWORD(sel) ==
		len)) ? MF_GRAYED : 0;
	menu.InsertMenu(7, MF_BYPOSITION | flags, ME_SELECTALL,
		MES_SELECTALL);
	menu.InsertMenu(7, MF_BYPOSITION | flags, ME_CLEARALL,
		MES_CLEARALL);
	

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |
		TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL CMenuEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case WM_CUT:
	case WM_COPY:
	case WM_PASTE:
		return SendMessage(LOWORD(wParam));
	case ME_SELECTALL:
		return SendMessage(EM_SETSEL, 0, -1);
	case ME_CLEARALL:
		SetWindowTextW(L"");
		break;
	}

	return CEdit::OnCommand(wParam, lParam);
}