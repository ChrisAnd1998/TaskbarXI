// TaskbarXIMFCGUIDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TaskbarXIMFCGUI.h"
#include "TaskbarXIMFCGUIDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <regex>

#define BUFFER 8192

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CTaskbarXIMFCGUIDlg dialog

CTaskbarXIMFCGUIDlg::CTaskbarXIMFCGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TASKBARXIMFCGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTaskbarXIMFCGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, cornerradius);
	DDX_Control(pDX, IDC_EDIT2, expandspeedval);
	DDX_Control(pDX, IDC_EDIT3, shrinkspeedval);
	DDX_Control(pDX, IDC_CHECK1, square);
	DDX_Control(pDX, IDC_CHECK2, radius);
	DDX_Control(pDX, IDC_CHECK3, ignoremax);
	DDX_Control(pDX, IDC_CHECK4, notray);
	DDX_Control(pDX, IDC_CHECK5, hidetraywnd);
	DDX_Control(pDX, IDC_CHECK7, sticky);
	DDX_Control(pDX, IDC_CHECK8, expandspeed);
	DDX_Control(pDX, IDC_CHECK9, shrinkspeed);
	DDX_Control(pDX, IDC_CHECK10, smoothresize);
	DDX_Control(pDX, IDC_CHECK11, runatboot);
	DDX_Control(pDX, IDC_CHECK6, consolee);

	DDX_Control(pDX, IDC_EDIT4, github);
	DDX_Control(pDX, IDC_EDIT5, website);
	DDX_Control(pDX, IDC_EDIT6, author);
	DDX_Control(pDX, IDC_EDIT7, versionval);
	DDX_Control(pDX, IDC_EDIT8, credits);
}

BEGIN_MESSAGE_MAP(CTaskbarXIMFCGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTaskbarXIMFCGUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTaskbarXIMFCGUIDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CTaskbarXIMFCGUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTaskbarXIMFCGUIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CTaskbarXIMFCGUIDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CTaskbarXIMFCGUIDlg::OnBnClickedButton3)
END_MESSAGE_MAP()

// CTaskbarXIMFCGUIDlg message handlers

BOOL CTaskbarXIMFCGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTaskbarXIMFCGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void remove_startup() {
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("");
	std::string cur_dir = std::string(buffer);// (argv[0]);
	//cur_dir.replace(cur_dir.find("TaskbarXIMFCGUI.exe"), sizeof("TaskbarXIMFCGUI.exe") - 1, "TaskbarXI.exe");

	if (cur_dir.find("40210ChrisAndriessen") != std::string::npos) {
		// Application is store version.
		std::string tt = "";
		WinExec(("powershell.exe Remove-ItemProperty -path 'HKCU:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run' -Name 'TaskbarXI'" + tt).c_str(), SW_HIDE);
	}
	else {
		HKEY hkey = NULL;
		LONG createStatus = RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
		LONG status = RegDeleteValue(hkey, L"TaskbarXI");
		RegCloseKey(hkey);
	}


}

void CTaskbarXIMFCGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	//CString x;
	//m_edit.GetWindowText(x);   // to Get editbox text

	auto x = _T("0");
	cornerradius.SetWindowText(x);

	auto x2 = _T("0");
	expandspeedval.SetWindowText(x2);

	auto x3 = _T("0");
	shrinkspeedval.SetWindowText(x3);


	auto x4 = _T("https://github.com/ChrisAnd1998/TaskbarXI");
	github.SetWindowText(x4);

	auto x5 = _T("https://chrisandriessen.nl/taskbarxi");
	website.SetWindowText(x5);

	auto x6 = _T("Chris Andriessen");
	author.SetWindowText(x6);

	auto x7 = _T("1.0.4.0");
	versionval.SetWindowText(x7);

	auto x8 = _T("Github: @PrincessAkira, @officialLennox,\n @dmitryaleshin");
	credits.SetWindowText(x8);




	
	DWORD dwType = REG_SZ;
	char value[2048];
	DWORD BufferSize = 2048;
	RegGetValueA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "TaskbarXI", RRF_RT_ANY, &dwType, &value, &BufferSize);

	
	std::string strtmp = value;

	

	if (BufferSize != 1024) {
		runatboot.SetCheck(1);
	}

	if (strtmp.find("-square") != std::string::npos) {
		square.SetCheck(1);
	}

	if (strtmp.find("-radius") != std::string::npos) {
		radius.SetCheck(1);

		std::regex regexp("-radius [0-9]+");
		std::smatch m;
		std::regex_search(strtmp, m, regexp);
		auto valm = m.str(0);
		std::string valstr(valm.begin(), valm.end());
		valstr.replace(valstr.find("-radius "), sizeof("-radius ") - 1, "");
		std::wstring val(valstr.begin(), valstr.end());
		auto ttt = val.c_str();
		cornerradius.SetWindowText((LPCTSTR)ttt);
	}

	if (strtmp.find("-ignoremax") != std::string::npos) {
		ignoremax.SetCheck(1);
	}

	if (strtmp.find("-hidetraywnd") != std::string::npos) {
		hidetraywnd.SetCheck(1);
	}

	if (strtmp.find("-sticky") != std::string::npos) {
		sticky.SetCheck(1);
	}

	if (strtmp.find("-smoothresize") != std::string::npos) {
		smoothresize.SetCheck(1);
	}

	if (strtmp.find("-notray") != std::string::npos) {
		notray.SetCheck(1);
	}

	if (strtmp.find("-expandspeed") != std::string::npos) {
		expandspeed.SetCheck(1);

		std::regex regexp("-expandspeed [0-9]+");
		std::smatch m;
		std::regex_search(strtmp, m, regexp);
		auto valm = m.str(0);
		std::string valstr(valm.begin(), valm.end());
		valstr.replace(valstr.find("-expandspeed "), sizeof("-expandspeed ") - 1, "");
		std::wstring val(valstr.begin(), valstr.end());
		auto ttt = val.c_str();
		expandspeedval.SetWindowText((LPCTSTR)ttt);
	}

	if (strtmp.find("-shrinkspeed") != std::string::npos) {
		shrinkspeed.SetCheck(1);

		std::regex regexp("-shrinkspeed [0-9]+");
		std::smatch m;
		std::regex_search(strtmp, m, regexp);
		auto valm = m.str(0);
		std::string valstr(valm.begin(), valm.end());
		valstr.replace(valstr.find("-shrinkspeed "), sizeof("-shrinkspeed ") - 1, "");
		std::wstring val(valstr.begin(), valstr.end());
		auto ttt = val.c_str();
		shrinkspeedval.SetWindowText((LPCTSTR)ttt);
	}


}



// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTaskbarXIMFCGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTaskbarXIMFCGUIDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void CTaskbarXIMFCGUIDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CTaskbarXIMFCGUIDlg::OnBnClickedButton1()
{
	std::string cur_cmd = "";

	

	if (square.GetCheck() == 1) {
		cur_cmd.append(" -square");
	}

	if (radius.GetCheck() == 1) {
		
		CString(x);
		cornerradius.GetWindowText(x);

		std::string s2 = CStringA(x);

		cur_cmd.append(" -radius " + s2);
		
	}

	if (ignoremax.GetCheck() == 1) {
		cur_cmd.append(" -ignoremax");
	}

	if (hidetraywnd.GetCheck() == 1) {
		cur_cmd.append(" -hidetraywnd");
	}

	if (notray.GetCheck() == 1) {
		cur_cmd.append(" -notray");
	}

	if (consolee.GetCheck() == 1) {
		cur_cmd.append(" -console");
	}

	if (sticky.GetCheck() == 1) {
		cur_cmd.append(" -sticky");
	}

	if (expandspeed.GetCheck() == 1) {
		cur_cmd.append(" -expandspeed ");
		CString(x);
		expandspeedval.GetWindowText(x);

		std::wstring chars = L"";
		chars += x;
		std::string val(chars.begin(), chars.end());

		cur_cmd.append(val);
	}

	if (shrinkspeed.GetCheck() == 1) {
		cur_cmd.append(" -shrinkspeed ");
		CString(x);
		shrinkspeedval.GetWindowText(x);

		std::wstring chars = L"";
		chars += x;
		std::string val(chars.begin(), chars.end());

		cur_cmd.append(val);
	}

	if (smoothresize.GetCheck() == 1) {
		cur_cmd.append(" -smoothresize");
	}

	if (runatboot.GetCheck() == 1) {
		cur_cmd.append(" -createstartup");
	}
	else {
		remove_startup();
	}
 

	std::wstring chars = L"";
	chars += (wchar_t)34;
	std::string quote(chars.begin(), chars.end());

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("");

	std::string cur_dir = std::string(buffer);// (argv[0]);

	cur_dir.replace(cur_dir.find("TaskbarXIMFCGUI.exe"), sizeof("TaskbarXIMFCGUI.exe") - 1, "TaskbarXI.exe");

	//std::wstring stemp = std::wstring(cur_dir.begin(), cur_dir.end());
	//LPCWSTR sw = stemp.c_str();
	//MessageBox(NULL, sw, NULL);

	if (cur_dir.find("40210ChrisAndriessen") != std::string::npos) {
		// Application is store version.
		WinExec(("Explorer.exe taskbarxi: " + cur_cmd).c_str(), SW_HIDE);
	}
	else {
		WinExec((quote + cur_dir + quote + cur_cmd).c_str(), SW_HIDE);
	}
}

void CTaskbarXIMFCGUIDlg::OnBnClickedButton2()
{
	std::string cur_cmd = "";

	std::wstring chars = L"";
	chars += (wchar_t)34;
	std::string quote(chars.begin(), chars.end());

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("");

	std::string cur_dir = std::string(buffer);// (argv[0]);

	cur_dir.replace(cur_dir.find("TaskbarXIMFCGUI.exe"), sizeof("TaskbarXIMFCGUI.exe") - 1, "TaskbarXI.exe");

	if (cur_dir.find("40210ChrisAndriessen") != std::string::npos) {
		// Application is store version.
		WinExec(("Explorer.exe taskbarxi: -stop" + cur_cmd).c_str(), SW_HIDE);
	}
	else {
		WinExec((quote + cur_dir + quote + "-stop").c_str(), SW_HIDE);
	}
}

void CTaskbarXIMFCGUIDlg::OnBnClickedButton4()
{
	remove_startup();
	MessageBox(L"Startup entry removed!", L"TaskbarXI Configurator", MB_OK);
}


void CTaskbarXIMFCGUIDlg::OnBnClickedButton3()
{
	exit(0);
}



