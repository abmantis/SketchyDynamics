
// TesterMFCDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "BPCtrlAnchorMap.h"

#include "PhySketchCore.h"
#include "PhySketchApplicationWindow.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsManager.h"

// CTesterMFCDlg dialog
class CTesterMFCDlg : public CDialogEx
{
// Construction
public:
	CTesterMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTERMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	void initPhySketch();

	PhySketch::Core _core;
	PhySketch::ApplicationWindow *_window;
	PhySketch::Renderer *_renderer;
	PhySketch::PhysicsManager *_physicsMgr;
//	TestInputListener *_inputListener;

protected:
	HICON m_hIcon;

	DECLARE_ANCHOR_MAP()

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic _stOpenGL;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
