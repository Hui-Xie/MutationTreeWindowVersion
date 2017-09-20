
// MutationTreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MutationTree.h"
#include "MutationTreeDlg.h"
#include "afxdialogex.h"
#include "OpenCsvDlg.h"
#include "ConstraintsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define StatusBarHeight 30
#define PictureTopX    415
#define PictureTopY    15
#define ScrollBarW    20
#define SinkerW       5




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMutationTreeDlg dialog



CMutationTreeDlg::CMutationTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMutationTreeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	csvDlg = NULL;
	m_isBirdView = false;
	m_xScroll = 0;
	m_yScroll = 0;
	m_xFactor = 0;
	m_yFactor = 0;
}


CMutationTreeDlg::~CMutationTreeDlg(){
	if (NULL != csvDlg) delete csvDlg;
	releaseMemBkBitmap();
}



void CMutationTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btnOpenFile);
	DDX_Control(pDX, IDC_BUTTON2, m_btnConstraints);
	DDX_Control(pDX, IDC_BUTTON3, m_btnBuildTree);
	DDX_Control(pDX, IDC_BUTTON4, m_btnSaveTree);
	DDX_Control(pDX, IDC_BUTTON5, m_btnSaveTreeDiagram);
	DDX_Control(pDX, IDC_BUTTON6, m_btnPriviousTree);
	DDX_Control(pDX, IDC_BUTTON7, m_btnNextTree);
	DDX_Control(pDX, IDC_EDIT1, m_editFile);
	DDX_Control(pDX, IDC_EDIT3, m_editStatusBar);
	DDX_Control(pDX, IDC_STATIC1, m_staticDiagram);
	DDX_Control(pDX, IDC_EDIT4, m_editTreeProperty);
	DDX_Control(pDX, IDC_EDIT5, m_editNodesProperty);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_HScrollbar);
	DDX_Control(pDX, IDC_SCROLLBAR2, m_VScrollbar);
	DDX_Control(pDX, IDC_BUTTON9, m_btnStop);
}

BEGIN_MESSAGE_MAP(CMutationTreeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CMutationTreeDlg::OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_BUTTON3, &CMutationTreeDlg::OnBnClickedGenerateTree)
	ON_BN_CLICKED(IDC_BUTTON2, &CMutationTreeDlg::OnBnClickedConstraint)
	ON_STN_CLICKED(IDC_STATIC1, &CMutationTreeDlg::OnSingleClickedTree)
	ON_BN_CLICKED(IDC_BUTTON7, &CMutationTreeDlg::OnBnClickedNextTree)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON4, &CMutationTreeDlg::OnBnClickedSaveFile)
	ON_STN_DBLCLK(IDC_STATIC1, &CMutationTreeDlg::OnStnDblclickDiagram)
	ON_BN_CLICKED(IDC_BUTTON5, &CMutationTreeDlg::OnBnClickedSaveDiagram)
	ON_WM_RBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON6, &CMutationTreeDlg::OnBnClickedPreviousTree)
	ON_BN_CLICKED(IDC_BUTTON9, &CMutationTreeDlg::OnBnClickedStopComputation)
END_MESSAGE_MAP()


// CMutationTreeDlg message handlers

BOOL CMutationTreeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_SHOWMAXIMIZED);
			
	// TODO: Add extra initialization here
	m_staticDiagram.ModifyStyle(0,SS_NOTIFY);
	createMemBkBitmap(); //Create a maximum background white screen memeory DC 
	
	m_HScrollbar.SetScrollRange(0, ScrollRange);
	m_VScrollbar.SetScrollRange(0, ScrollRange);

	m_editStatusBar.SetWindowText(_T("Please first open a csv VAF file or a csv tree file."));

	theApp.treeFactory.m_pMainWindow = this;

	openFileFromCmdLine();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMutationTreeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMutationTreeDlg::OnPaint()
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
HCURSOR CMutationTreeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMutationTreeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	
	// Set Controls to Maximize
	if (NULL != m_editStatusBar){
		m_editStatusBar.SetWindowPos(NULL, 2, cy - StatusBarHeight-2, cx - 2, StatusBarHeight,
		                         	SWP_SHOWWINDOW);
	}

	if (NULL != m_staticDiagram){
		m_staticDiagram.SetWindowPos(NULL, PictureTopX, PictureTopY, cx - PictureTopX - PictureTopY / 2, int(cy - PictureTopY - StatusBarHeight*1.3),
		                            SWP_SHOWWINDOW);

		RECT rect;
		m_staticDiagram.GetWindowRect(&rect);
		this->ScreenToClient(&rect);
		
		if (NULL != m_HScrollbar){
			m_HScrollbar.SetOwner(&m_staticDiagram);
			m_HScrollbar.SetWindowPos(&CWnd::wndTop, rect.left + SinkerW, rect.bottom - ScrollBarW - SinkerW, rect.right - rect.left-2*SinkerW, ScrollBarW,
				SWP_DRAWFRAME | SWP_SHOWWINDOW);
		}
		
		if (NULL != m_VScrollbar){
			m_VScrollbar.SetOwner(&m_staticDiagram);
			m_VScrollbar.SetWindowPos(&CWnd::wndTop, rect.right - ScrollBarW - SinkerW, rect.top + SinkerW, ScrollBarW, rect.bottom - rect.top - ScrollBarW - 2 * SinkerW,
				SWP_DRAWFRAME | SWP_SHOWWINDOW);
		}

		showTree();
	}
}


void CMutationTreeDlg::OnBnClickedOpenFile()
{
	OpenCsvDlg csvDlg(this);
	INT_PTR nResponse = csvDlg.DoModal();
	if (nResponse == IDOK)
	{
		CString statusText;
		statusText.Format(_T("Retieved a csv file of %d nodes."), theApp.VAFVector.size());
		m_editStatusBar.SetWindowText(statusText);

		//display current dir
		TCHAR currentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDir);
		statusText.Format(_T("Current directory: %s"), currentDir);
		m_editFile.SetWindowText(statusText);
	}
	else if (nResponse == IDCANCEL)
	{
		return;
	}
	else 
	{
		TRACE(traceAppMsg, 0, "Warning: Open CSV File Dialog Error.\n");
	}
	
	
}


void CMutationTreeDlg::OnBnClickedGenerateTree()
{
	// TODO: Add your control notification handler code here
	//TreeFactory binTree;
	theApp.stopComputation = false;

	if (0 == theApp.VAFVector.size()) {
		m_editStatusBar.SetWindowText(_T("Sorry. Please first open a csv file."));
		return;
	}
	if (VAF9 == theApp.csvFile.m_fileType || VAF10 == theApp.csvFile.m_fileType){
		m_btnNextTree.EnableWindow(true);
		m_btnPriviousTree.EnableWindow(true);
		
		theApp.treeFactory.buildBFSTrees();
		theApp.treeFactory.sortTrees();

		CString statusText;
		if (0 == theApp.treeFactory.m_totalResutlTree) {
			statusText.Format(_T("NO tree generated. Relaxing Consraint may produce more trees."));
		}
		else {
			statusText.Format(_T("Total %d trees generated. The all tree statistics file with postfix Statistics.csv is in the directory %s."),
				theApp.treeFactory.m_totalResutlTree, theApp.curDir);
		}
		m_editStatusBar.SetWindowText(statusText);

		int numPrintTrees = (theApp.treeFactory.m_totalResutlTree > NumBestTrees) ? NumBestTrees : theApp.treeFactory.m_totalResutlTree;
		theApp.treeFactory.printTrees(numPrintTrees);

	}

	if (TREE11 == theApp.csvFile.m_fileType){
		m_btnNextTree.EnableWindow(false);
		m_btnPriviousTree.EnableWindow(false);

		theApp.treeFactory.m_treeList.front().buildNodeRelationFromID();
		theApp.treeFactory.setBasicFilename();
		theApp.treeFactory.m_treeList.front().statistic();
		theApp.treeFactory.m_iterShowingTree = theApp.treeFactory.m_treeList.begin();
		theApp.treeFactory.m_curTreeIndex = 0;
	}
		
	showCurTree();
	
}

void CMutationTreeDlg::showCurTree() {
	drawTree();
	showTreeStatistics();
	showTree();
}

void CMutationTreeDlg::createMemBkBitmap(){
	m_memBkDC.CreateCompatibleDC(m_staticDiagram.GetDC());
	RECT rect = getDiagramClientRect();
	m_memBkBitmap.CreateCompatibleBitmap(m_staticDiagram.GetDC(), rect.right, rect.bottom);
	m_memBkDC.SelectObject(m_memBkBitmap);
	m_memBkDC.FillSolidRect(0, 0, rect.right, rect.bottom, RGB(255, 255, 255));
}


void CMutationTreeDlg::releaseMemBkBitmap(){
	m_memBkDC.DeleteDC();
	m_memBkBitmap.DeleteObject();
}

RECT CMutationTreeDlg::getDiagramClientRect(){
	RECT rect;
	m_staticDiagram.GetClientRect(&rect);
	rect.bottom -= ScrollBarW;
	rect.right -= ScrollBarW;
	return rect;
}


void CMutationTreeDlg::drawTree(){

	if (theApp.treeFactory.isCurrentIteratorPointingToEnd()) return;

	if (0 == theApp.treeFactory.m_iterShowingTree->m_nodeVector.size()) return;
	theApp.treeDiagram.buildNodeDiagramVector(theApp.treeFactory.m_iterShowingTree->m_nodeVector);
	theApp.treeDiagram.releaseMemBitmap();
	theApp.treeDiagram.createMemBitmap(m_staticDiagram.GetDC());
	theApp.treeDiagram.drawRecursiveNodes(theApp.treeFactory.m_iterShowingTree->m_root);
}

void CMutationTreeDlg::showTree(){
	
	RECT rect = getDiagramClientRect();
	m_xFactor = float(max((theApp.treeDiagram.m_xMax - rect.right)*1.0 / ScrollRange, 0));
	m_yFactor = float(max((theApp.treeDiagram.m_yMax - rect.bottom)*1.0 / ScrollRange, 0));

	m_staticDiagram.GetDC()->BitBlt(0, 0, rect.right, rect.bottom, &m_memBkDC, 0, 0, SRCCOPY);

	if (false == m_isBirdView){
		m_staticDiagram.GetDC()->BitBlt(0, 0, rect.right, rect.bottom,
		            	theApp.treeDiagram.m_pDC, int(m_xScroll*m_xFactor), int(m_yScroll*m_yFactor), SRCCOPY);
	}
	else { // true == m_isBirdView
		m_staticDiagram.GetDC()->StretchBlt(0, 0, rect.right, rect.bottom,
	      		theApp.treeDiagram.m_pDC, 0, 0, theApp.treeDiagram.m_xMax, theApp.treeDiagram.m_yMax, SRCCOPY);
		m_xScroll = 0;
		m_yScroll = 0;
	}

	updateScrollBarPos(m_xScroll, m_yScroll);
	//m_editStatusBar.SetWindowText(_T("Double Click shows a birdview diagram; Single Click shows the larger diagram."));
}


void CMutationTreeDlg::OnStnDblclickDiagram()
{
	// TODO: Add your control notification handler code here
	m_isBirdView = true;
	showTree();
}

void CMutationTreeDlg::OnSingleClickedTree()
{
	// TODO: Add your control notification handler code here
	CPoint cursorP;
	GetCursorPos(&cursorP);
	m_staticDiagram.ScreenToClient(&cursorP);
		
	if (true == m_isBirdView){
		m_isBirdView = false;
		//convert client coordiantes to pDC picture
		RECT rect = getDiagramClientRect();
		int x1 = max(int(cursorP.x *1.0 / rect.right*theApp.treeDiagram.m_xMax) - rect.right / 2, 0);
		int y1 = max(int(cursorP.y *1.0 / rect.bottom*theApp.treeDiagram.m_yMax) - rect.bottom / 2, 0);

		x1 = min(x1, theApp.treeDiagram.m_xMax - rect.right);
		y1 = min(y1, theApp.treeDiagram.m_yMax - rect.bottom);
		if (x1 <= 0)  x1 = 0;
		if (y1 <= 0)  y1 = 0;
		
		m_xScroll = (0 == m_xFactor)? 0 : int(x1 / m_xFactor);
		m_yScroll = (0 == m_yFactor)? 0 : int(y1 / m_yFactor);
		showTree();
	}
	else {// when m_isBirdView is false, display 3 generations 
		clientToMemDC(cursorP.x, cursorP.y);
		int clickNode = theApp.treeDiagram.getNodeIndex(cursorP.x, cursorP.y);
		if (-1 == clickNode){
			m_editStatusBar.SetWindowText(_T("Single Click inside a node box shows its information."));
		}
		else {
			CString nodesText = theApp.treeDiagram.showNode3Generations(clickNode);
			m_editNodesProperty.SetWindowText(nodesText);
		}
	}

}

void CMutationTreeDlg::clientToMemDC(long &x, long&y){
	x = x + int(m_xScroll*m_xFactor);
	y = y + int(m_yScroll*m_yFactor);
}

void CMutationTreeDlg::updateScrollBarPos(int x,int y){
	m_xScroll = x;
	m_yScroll = y;
	m_HScrollbar.SetScrollPos(x);
	m_VScrollbar.SetScrollPos(y);
}


void CMutationTreeDlg::showTreeStatistics(){
	
	if (theApp.treeFactory.isCurrentIteratorPointingToEnd()) return;
	
	CString propertyText;
	CString temp;
	propertyText += "Tree Property:\r\n";
		
	if (TREE11 == theApp.csvFile.m_fileType) {
		propertyText += "===Tree csv File===\r\n";
	}
	else {
		temp.Format(_T("Tree Index: %lu in %lu \r\n"), 
			              theApp.treeFactory.getCurrentTredIndex(), theApp.treeFactory.getTotalResultTree());
		propertyText += temp;
		temp.Format(_T("Tree Constructing Method: %s\r\n"), CString(theApp.constraint.getTypeName().c_str()));
		propertyText += temp;
		temp.Format(_T("Root Constraint Epsilon:  %.2f\r\n"), theApp.constraint.m_RootConstraintEps / 100.0f);
		propertyText += temp;
		temp.Format(_T("Triangular Constraint Epsilon:  %.2f\r\n"), theApp.constraint.m_TriConstraintEps/100.0f);
		propertyText += temp;
		temp.Format(_T("Least Richness of a Tree:  %.2f\r\n\r\n"), theApp.constraint.m_LeastRichness / 100.0f);
		propertyText += temp;

	}

	temp.Format(_T("File Name: %s\r\n"), CString(theApp.csvFile.m_inputFilename.c_str()));
	propertyText += temp;
	temp.Format(_T("Node Num:		%d\r\n"), theApp.treeFactory.m_iterShowingTree->m_nTreeNodes);
	propertyText += temp;
	temp.Format(_T("Layer Num:		%d\r\n"), theApp.treeFactory.m_iterShowingTree->m_nLayers);
	propertyText += temp;
	temp.Format(_T("Internal Nodes Num:	%d\r\n"), theApp.treeFactory.m_iterShowingTree->m_nInternalNodes);
	propertyText += temp;
	temp.Format(_T("Remaining Nodes Num:	%d\r\n"), theApp.treeFactory.m_iterShowingTree->m_NumRemainingNodes);
	propertyText += temp;
	temp.Format(_T("Min Node Epsilon:		%.2f\r\n"), theApp.treeFactory.m_iterShowingTree->m_minEpsilon);
	propertyText += temp;
	temp.Format(_T("Max Node Epsilon:		%.2f\r\n"), theApp.treeFactory.m_iterShowingTree->m_maxEpsilon);
	propertyText += temp;
	temp.Format(_T("Average Nodes Epsilon	%.2f\r\n"), theApp.treeFactory.m_iterShowingTree->m_averageEpsilon);
	propertyText += temp;

	m_editTreeProperty.SetWindowText(propertyText);

}



void CMutationTreeDlg::OnBnClickedConstraint()
{
	// TODO: Add your control notification handler code here
	ConstraintsDlg dlg(this);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		if (theApp.constraint.m_TriConstraintEps != theApp.constraint.m_tempTriConstraintEps){
			theApp.constraint.m_TriConstraintEps = theApp.constraint.m_tempTriConstraintEps;
		}

		if (theApp.constraint.m_RootConstraintEps != theApp.constraint.m_tempRootConstraintEps) {
			theApp.constraint.m_RootConstraintEps = theApp.constraint.m_tempRootConstraintEps;
		}

		if (theApp.constraint.m_LeastRichness != theApp.constraint.m_tempLeastRichness) {
			theApp.constraint.m_LeastRichness = theApp.constraint.m_tempLeastRichness;
		}


		CString statusText;
		statusText.Format(_T("Now Root constaint epsilon = %d%%, Triangular constraint epsilon = %d%%, Least Richness of a Tree = %d%%"),
		                    	theApp.constraint.m_RootConstraintEps,theApp.constraint.m_TriConstraintEps,
		                     	theApp.constraint.m_LeastRichness);
		m_editStatusBar.SetWindowText(statusText);
	}
	else// if (nResponse == IDCANCEL)
	{
		return;
	}
	
}


void CMutationTreeDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int CurPos = m_VScrollbar.GetScrollPos();

	RECT rect = getDiagramClientRect();
	int page = 0;
	if (m_yFactor > rect.bottom) page = 2;
	if (m_yFactor>0 && m_yFactor <= rect.bottom) page = int(rect.bottom / m_yFactor);
	
	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_TOP:      // Scroll to far up.
		CurPos = 0;
		break;

	case SB_BOTTOM:      // Scroll to far down.
		CurPos = ScrollRange;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINEUP:      // Scroll up.
		if (CurPos > 0) 	CurPos = max(CurPos-1, 0);
		break;

	case SB_LINEDOWN:   // Scroll down.
		if (CurPos < ScrollRange) 	CurPos = min(CurPos + 1, ScrollRange);
		break;

	case SB_PAGEUP:    // Scroll one page up.
		if (CurPos > 0)             CurPos = max(0, CurPos - page);
		break;

	case SB_PAGEDOWN: 	      // Scroll one page down
		if (CurPos < ScrollRange) 	CurPos = min(ScrollRange, CurPos + page);
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;

	default:
		//return;
		break;
	}

	// Set the new position of the thumb (scroll box).
	m_VScrollbar.SetScrollPos(CurPos);

	if (CurPos != m_yScroll){
		m_yScroll = CurPos;
		showTree();
	}
		
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}
void CMutationTreeDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetKeyState(VK_LCONTROL) & 0x8000){
		m_editStatusBar.SetWindowText(_T("Author: Hui Xie (SheenXH@gmail.com), Donghai Dai (donghai-dai@uiowa.edu)"));
	}
	CDialogEx::OnRButtonDblClk(nFlags, point);
}
void CMutationTreeDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int CurPos = m_HScrollbar.GetScrollPos();

	RECT rect = getDiagramClientRect();
	int page = 0;
	if (m_xFactor > rect.right) page = 2;
	if (m_xFactor>0 && m_xFactor <= rect.right) page = int(rect.right / m_xFactor);
	
	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left
		CurPos = 0;
		break;

	case SB_RIGHT:      // Scroll to far right.
		CurPos = ScrollRange;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (CurPos > 0) 	CurPos = max(CurPos - 1, 0);
		break;

	case SB_LINERIGHT:   // Scroll down.
		if (CurPos < ScrollRange) CurPos = min(CurPos + 1, ScrollRange);
		break;

	case SB_PAGELEFT:    // Scroll one page LEFT.
		if (CurPos > 0)             CurPos = max(0, CurPos - page);
		break;

	case SB_PAGERIGHT: 	      // Scroll one page LEFT
		if (CurPos < ScrollRange) 	CurPos = min(ScrollRange, CurPos + page);
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;

	default:
		//return;
		break;
	}

	// Set the new position of the thumb (scroll box).
	m_HScrollbar.SetScrollPos(CurPos);
	if (CurPos != m_xScroll){
		m_xScroll = CurPos;
		showTree();
	}
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMutationTreeDlg::OnBnClickedSaveFile()
{
	// TODO: Add your control notification handler code here
	if (theApp.treeFactory.isCurrentIteratorPointingToEnd() ||0 == theApp.treeFactory.m_iterShowingTree->m_nodeVector.size()) {
		m_editStatusBar.SetWindowText(_T("Sorry. No data to save." ));
		return;
	}

	if (TREE11 == theApp.csvFile.m_fileType){
		m_editStatusBar.SetWindowText(_T("Inputing csv file is a Tree file. No need to save."));
		return;
	}

	unsigned long treeIndex = theApp.treeFactory.getCurrentTredIndex();
	std::string filename = theApp.treeFactory.m_iterShowingTree->saveCsvTree(treeIndex);

	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	CString statusText;
	statusText.Format(_T("Tree csv file %s was saved in the directory: %s."), CString(filename.c_str()), currentDir);
	m_editStatusBar.SetWindowText(statusText);
}

void CMutationTreeDlg::OnBnClickedSaveDiagram()
{
	// TODO: Add your control notification handler code here
	if (theApp.treeFactory.isCurrentIteratorPointingToEnd() ||0 == theApp.treeFactory.m_iterShowingTree->m_nodeVector.size()){
		m_editStatusBar.SetWindowText(_T("Sorry, Currently there is No file to save."));
		return;
	}

	theApp.treeDiagram.saveDiagram(theApp.treeFactory.getCurrentTredIndex());
	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	CString statusText;
	statusText.Format(_T("Tree Image file %s was saved in the directory: %s."), CString(theApp.treeDiagram.m_imageName.c_str()), currentDir);
	m_editStatusBar.SetWindowText(statusText);

}

void CMutationTreeDlg::OnBnClickedPreviousTree()
{
	if (TREE11 == theApp.csvFile.m_fileType) return;
	
	if (theApp.treeFactory.m_iterShowingTree != theApp.treeFactory.m_treeList.begin()) {
		theApp.treeFactory.m_iterShowingTree--;
		if (theApp.treeFactory.m_iterShowingTree != theApp.treeFactory.m_treeList.begin()) {
			theApp.treeFactory.m_curTreeIndex--;
		}
		else {
			theApp.treeFactory.m_curTreeIndex = 0;
		}
		
	}
	else {
		m_btnPriviousTree.EnableWindow(false);
	}
	m_btnNextTree.EnableWindow(true);
	showCurTree();
}

void CMutationTreeDlg::OnBnClickedNextTree()
{
	if (TREE11 == theApp.csvFile.m_fileType) return;

	if (theApp.treeFactory.m_iterShowingTree != theApp.treeFactory.m_treeList.end()) {
		theApp.treeFactory.m_iterShowingTree++;
		if (theApp.treeFactory.m_iterShowingTree != theApp.treeFactory.m_treeList.end()) {
			theApp.treeFactory.m_curTreeIndex++;
		}
	}
	else {
		m_btnNextTree.EnableWindow(false);
	}
	m_btnPriviousTree.EnableWindow(true);
	showCurTree();
}

void CMutationTreeDlg::processInternalMessage() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}



void CMutationTreeDlg::OnBnClickedStopComputation()
{
	// TODO: Add your control notification handler code here
	theApp.stopComputation = true;

}

void CMutationTreeDlg::openFileFromCmdLine() {

	if (theApp.m_lpCmdLine[0] == _T('\0') || NULL == theApp.m_lpCmdLine) return;
	char filename[400];
	int i = 0;
	for (i = 0;i < 400;++i) filename[i] = '\0';
	i = 0;
	while (theApp.m_lpCmdLine[i] != _T('\0')) {
		filename[i] = theApp.m_lpCmdLine[i];
		++i;
	}

	std::string fullFilename = std::string(filename);

	//find file dir
	std::size_t slashPos = fullFilename.rfind(std::string("\\"));
	std::string dirStr = fullFilename.substr(1, slashPos - 1);
	SetCurrentDirectory(CString(dirStr.c_str()).GetBuffer());

	std::size_t csvPos = fullFilename.rfind(std::string(".csv"));
	std::size_t len = csvPos + 3 - slashPos;

	theApp.csvFile.m_inputFilename = fullFilename.substr(slashPos + 1, len);

	if (0 == theApp.csvFile.setFileType()) {
		AfxMessageBox(_T("Sorry. File Type error. We need a csv TREE file with specific format."));
		return;
	}
	theApp.csvFile.readFileData();
	PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), (LPARAM)m_hWnd);
	theApp.m_lpCmdLine[0] = _T('\0');
}
