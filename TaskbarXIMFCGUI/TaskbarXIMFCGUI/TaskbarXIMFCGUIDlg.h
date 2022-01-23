
// TaskbarXIMFCGUIDlg.h : header file
//

#pragma once


// CTaskbarXIMFCGUIDlg dialog
class CTaskbarXIMFCGUIDlg : public CDialogEx
{
// Construction
public:
	CTaskbarXIMFCGUIDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBARXIMFCGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit cornerradius;
	CEdit expandspeedval;
	CEdit shrinkspeedval;
	CButton square;
	afx_msg void OnBnClickedButton1();
	CButton radius;
	CButton ignoremax;
	CButton notray;
	CButton hidetraywnd;
	CButton sticky;
	CButton expandspeed;
	CButton shrinkspeed;
	CButton smoothresize;
	CButton runatboot;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	CButton consolee;
	
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	CEdit github;
	CEdit website;
	CEdit author;
	afx_msg void OnEnChangeEdit7();
	CEdit versionval;
	CEdit credits;
	afx_msg void OnBnClickedCheck11();
};
