#if _MSC_VER > 1000
#pragma once
#include "afxwin.h"


class CMenuEdit : public CEdit
{
public:
	CMenuEdit() {};

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	DECLARE_MESSAGE_MAP()
};
#endif  