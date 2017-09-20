
// MutationTreeDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxvslistbox.h"
#include "OpenCsvDlg.h"
#include "afxcmn.h"

#define ScrollRange 30


// CMutationTreeDlg dialog
class CMutationTreeDlg : public CDialogEx
{
// Construction
public:
	CMutationTreeDlg(CWnd* pParent = NULL);	// standard constructor
	~CMutationTreeDlg();

// Dialog Data
	enum { IDD = IDD_MUTATIONTREE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	OpenCsvDlg * csvDlg;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnOpenFile;
	CButton m_btnStop;
	CButton m_btnConstraints;
	CButton m_btnBuildTree;
	CButton m_btnSaveTree;
	CButton m_btnSaveTreeDiagram;
	CButton m_btnPriviousTree;
	CButton m_btnNextTree;
	CEdit m_editFile;
	CEdit m_editStatusBar;
	CStatic m_staticDiagram;
	CScrollBar m_HScrollbar;
	CScrollBar m_VScrollbar;
	int m_xScroll;
	int m_yScroll;
	float m_xFactor;//(memDCx-ClientW)/ScrollRange
	float m_yFactor;//(memDCy-ClientH)/scrollRange
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedGenerateTree();

	void showTreeStatistics();
	bool m_isBirdView; 
		
	CEdit m_editTreeProperty;
	afx_msg void OnBnClickedConstraint();
	CEdit m_editNodesProperty;
	afx_msg void OnSingleClickedTree();

	void drawTree();
		
	afx_msg void OnBnClickedNextTree();
	
	void showTree();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSaveFile();
	afx_msg void OnStnDblclickDiagram();
	void clientToMemDC(long &x, long&y);
	void updateScrollBarPos(int x, int y);
	afx_msg void OnBnClickedSaveDiagram();
	void openFileFromCmdLine();

private:
	CDC m_memBkDC; //for screen switcch
	CBitmap m_memBkBitmap;
	void createMemBkBitmap();
	void releaseMemBkBitmap();
	RECT getDiagramClientRect();
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedPreviousTree();
	void showCurTree();
	void processInternalMessage();
	
	afx_msg void OnBnClickedStopComputation();
};
