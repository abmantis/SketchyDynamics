
// TesterMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TesterMFC.h"
#include "TesterMFCDlg.h"
#include "afxdialogex.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchApplicationWindow_WGL.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTesterMFCDlg dialog

BEGIN_ANCHOR_MAP(CTesterMFCDlg)
	ANCHOR_MAP_ENTRY(IDC_ST_OPENGL, ANF_TOP | ANF_LEFT | ANF_BOTTOM | ANF_RIGHT)
END_ANCHOR_MAP()


CTesterMFCDlg::CTesterMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTesterMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTesterMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_OPENGL, _stOpenGL);
}

BEGIN_MESSAGE_MAP(CTesterMFCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTesterMFCDlg message handlers

BOOL CTesterMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	
	initPhySketch();
	SetTimer(1, 17, 0);

	InitAnchors();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTesterMFCDlg::OnPaint()
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
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTesterMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTesterMFCDlg::initPhySketch()
{
	_core.initialise("Log.txt", true, PhySketch::Vector2(0, -10));

	PhySketch::ParameterMap wndparams;
//	wndparams["parent_window"] = PhySketch::toString((long)_stOpenGL.GetSafeHwnd());
	char buff[15];
	sprintf(buff, "%d", (int)_stOpenGL.GetSafeHwnd());
	wndparams["parent_window"] = buff; 
	
	CRect stRect;
	_stOpenGL.GetWindowRect(&stRect);
	ScreenToClient(&stRect);

	_window = _core.createWindow("test", PhySketch::Vector2::ZERO, 
		PhySketch::Vector2(stRect.Width(), stRect.Height()), false, &wndparams);
	_renderer = PhySketch::Renderer::getSingletonPtr();
	_physicsMgr = PhySketch::PhysicsManager::getSingletonPtr();

// 	_inputListener = new TestInputListener();
// 	_inputListener->_physicsMgr = _physicsMgr;
// 	_window->addInputListener(_inputListener);

	b2BodyDef bodyDef;
	bodyDef.position.Set(0.0f, -4.0f);
	b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(7.0f, 0.3f);
	body->CreateFixture(&groundBox, 0.0f);
	PhySketch::PhysicsBody *phyBody = new PhySketch::PhysicsBody(body);
	_physicsMgr->AddBody(phyBody);

}



void CTesterMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	_core.doOneFrame();
	CDialogEx::OnTimer(nIDEvent);
}


void CTesterMFCDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rcWnd;
	GetWindowRect(&rcWnd);

	HandleAnchors(&rcWnd); // you can alternatively pass NULL for &rcWnd

	if(_stOpenGL.GetSafeHwnd() != 0)
	{
		CRect stRect;
		_stOpenGL.GetWindowRect(&stRect);
		ScreenToClient(&stRect);
//		::SendMessage(((PhySketch::ApplicationWindow_WGL*)_window)->getWindowHandler(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(stRect.Width(), stRect.Height()));
		::MoveWindow(((PhySketch::ApplicationWindow_WGL*)_window)->getWindowHandler(), 0, 0, stRect.Width(), stRect.Height(), true);
	}

	// TODO: Add your message handler code here
}
