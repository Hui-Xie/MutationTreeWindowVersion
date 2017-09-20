#include "stdafx.h"
#include "ConstraintsDlg.h"
#include  "MutationTree.h"


ConstraintsDlg::ConstraintsDlg(CWnd* pParent) : CDialogEx(ConstraintsDlg::IDD, pParent)
{
}


ConstraintsDlg::~ConstraintsDlg()
{
}

void ConstraintsDlg::DoDataExchange(CDataExchange* pDX)	// DDX/DDV support
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_EDIT1, m_editTriConstraintEps);
	DDX_Control(pDX, IDC_EDIT6, m_editRootConstraintEps);
	DDX_Control(pDX, IDC_EDIT7, m_editLeastRichness);
}


BEGIN_MESSAGE_MAP(ConstraintsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &ConstraintsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ConstraintsDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT1, &ConstraintsDlg::OnEnChangeEditTriEpsilon)
	ON_BN_CLICKED(IDC_RADIOBFS, &ConstraintsDlg::OnBnClickedRadiobfs)
	ON_BN_CLICKED(IDC_RADIODFS, &ConstraintsDlg::OnBnClickedRadiodfs)
	ON_EN_CHANGE(IDC_EDIT6, &ConstraintsDlg::OnEnChangeEditRootEpsilon)
	ON_EN_CHANGE(IDC_EDIT7, &ConstraintsDlg::OnEnChangeLeastRichness)
END_MESSAGE_MAP()


BOOL ConstraintsDlg::OnInitDialog(){
	CDialogEx::OnInitDialog();
	
	
	

	return true;
}


void ConstraintsDlg::OnShowWindow(BOOL bShow, UINT nStatus){
	CDialogEx::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
	CString temp;
	temp.Format(_T("%d"), theApp.constraint.m_TriConstraintEps);
	m_editTriConstraintEps.SetWindowText(temp);

	temp.Format(_T("%d"), theApp.constraint.m_RootConstraintEps);
	m_editRootConstraintEps.SetWindowText(temp);

	temp.Format(_T("%d"), theApp.constraint.m_LeastRichness);
	m_editLeastRichness.SetWindowText(temp);

	
	if (1 == theApp.constraint.m_type){
		OnBnClickedRadiodfs();
	}
	if (2 == theApp.constraint.m_type){
		OnBnClickedRadiobfs();
	}
}

void ConstraintsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	CDialogEx::OnOK();
}


void ConstraintsDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void ConstraintsDlg::OnEnChangeEditRootEpsilon()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	CString text;
	m_editRootConstraintEps.GetWindowText(text);
	theApp.constraint.m_tempRootConstraintEps = int(_tstoi((LPCTSTR)text));
}



void ConstraintsDlg::OnEnChangeEditTriEpsilon()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	CString text;
	m_editTriConstraintEps.GetWindowText(text);
	theApp.constraint.m_tempTriConstraintEps = int(_tstoi((LPCTSTR)text));
}


void ConstraintsDlg::OnBnClickedRadiobfs()
{
	// TODO: Add your control notification handler code here
	((CButton*)GetDlgItem(IDC_RADIOBFS))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADIODFS))->SetCheck(BST_UNCHECKED);
	theApp.constraint.m_type = 2;
}


void ConstraintsDlg::OnBnClickedRadiodfs()
{
	// TODO: Add your control notification handler code here
	((CButton*)GetDlgItem(IDC_RADIOBFS))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIODFS))->SetCheck(BST_CHECKED);
	theApp.constraint.m_type = 1;
}




void ConstraintsDlg::OnEnChangeLeastRichness()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here


	CString text;
	m_editLeastRichness.GetWindowText(text);
	theApp.constraint.m_tempLeastRichness = int(_tstoi((LPCTSTR)text));
}
