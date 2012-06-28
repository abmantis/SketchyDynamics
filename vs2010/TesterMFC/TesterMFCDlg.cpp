
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
	ON_BN_CLICKED(IDC_BUTTON1, &CTesterMFCDlg::OnBnClickedButton1)
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

	_window = _core.createWindow("test", PhySketch::Vector2::ZERO_XY, 
		PhySketch::Vector2((float)stRect.Width(), (float)stRect.Height()), false, &wndparams);
	_renderer = PhySketch::Renderer::getSingletonPtr();
	_physicsMgr = PhySketch::PhysicsManager::getSingletonPtr();

	_inputListener = new TestInputListener();
	_inputListener->_physicsMgr = _physicsMgr;
	_window->addInputListener(_inputListener);

	{
		PhySketch::Material backgroundMat;
		backgroundMat.setColor(PhySketch::Color(1.0f,1.0f,1.0f,1.0f));
		b2BodyDef backgroundbodyDef;
		backgroundbodyDef.position.Set(0.0f, 0.0f);
		PhySketch::PhysicsBody *backgroundPhyBody = _physicsMgr->createBody(backgroundbodyDef);

		b2PolygonShape backgroundBox;
		backgroundBox.SetAsBox((_renderer->getSceneViewAxisMax() - _renderer->getSceneViewAxisMin()).x*0.5f,
			(_renderer->getSceneViewAxisMax() - _renderer->getSceneViewAxisMin()).y*0.5f);
		b2FixtureDef backgroundFixtureDef;
		backgroundFixtureDef.shape = &backgroundBox;
		backgroundFixtureDef.filter.categoryBits = 0x0;
		backgroundPhyBody->getBox2DBody()->CreateFixture(&backgroundFixtureDef);
		backgroundPhyBody->setFillMaterial(backgroundMat);
		backgroundPhyBody->setLineMaterial(backgroundMat);
		backgroundPhyBody->reconstructPolygons();		
		_physicsMgr->setUnselectableBody(backgroundPhyBody);
	}
	
		
	{
		b2BodyDef bodyDef;
		bodyDef.position.Set(0.0f, -4.0f);
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(7.0f, 0.3f);
		body->CreateFixture(&groundBox, 0.0f);
		PhySketch::PhysicsBody *phyBody = _physicsMgr->createBody(body);
		_physicsMgr->setUnselectableBody(phyBody);
	}

}



void CTesterMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	_core.doOneFrame();
	if(_physicsMgr->isSimulationPaused())
	{
		GetDlgItem(IDC_BT_PAUSE_PLAY)->SetWindowText(L"Play");
	}
	else
	{
		GetDlgItem(IDC_BT_PAUSE_PLAY)->SetWindowText(L"Pause");
	}

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


BOOL CTesterMFCDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->hwnd == _stOpenGL.GetSafeHwnd())
	{
		//std::cout << "lalala" << std::endl;
		::SendMessage(((PhySketch::ApplicationWindow_WGL*)_window)->getWindowHandler(), pMsg->message, pMsg->wParam, pMsg->lParam);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

CTesterMFCDlg::~CTesterMFCDlg()
{
	delete _inputListener;
	_inputListener = nullptr;
}


void CTesterMFCDlg::OnBnClickedButton1()
{
	_physicsMgr->toggleSimulation();
}
