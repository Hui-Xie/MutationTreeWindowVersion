#include "stdafx.h"
#include "OpenCsvDlg.h"
#include "MutationTree.h"


OpenCsvDlg::OpenCsvDlg(CWnd* pParent) : CDialogEx(OpenCsvDlg::IDD, pParent)
{
	
}


OpenCsvDlg::~OpenCsvDlg()
{
}

void OpenCsvDlg::DoDataExchange(CDataExchange* pDX)	// DDX/DDV support
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCSHELLTREE1, m_treePath);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_MFCSHELLLIST1, m_fileList);
	DDX_Control(pDX, IDC_COMBO1, m_comboxStatus);
}

BOOL OpenCsvDlg::OnInitDialog(){
	
	CDialogEx::OnInitDialog();
	m_fileList.SetItemTypes(SHCONTF_NONFOLDERS);
	int chracterWidth = 8;
	m_fileList.SetColumnWidth(0, 40 * chracterWidth);//Name
	m_fileList.SetColumnWidth(1, 10 * chracterWidth);//Size
	m_fileList.SetColumnWidth(2, 19 * chracterWidth);//type
	m_fileList.SetColumnWidth(3, 23 * chracterWidth);//Modify time

	m_comboxStatus.SetWindowText(_T("Please Choose a directory and then Choose a CSV file."));

	if (theApp.curDir.IsEmpty()){
		LPTSTR path = theApp.curDir.GetBuffer(MAX_PATH);
		GetCurrentDirectory(MAX_PATH, path);
		m_treePath.SelectPath(path);
	}
		
	return true;
}


BEGIN_MESSAGE_MAP(OpenCsvDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &OpenCsvDlg::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_MFCSHELLTREE1, &OpenCsvDlg::OnTvnSelchangedMfcshelltree1)
END_MESSAGE_MAP()

void OpenCsvDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int nFiles = m_fileList.GetSelectedCount();
	if (1 != nFiles){
		AfxMessageBox(_T("You must select only one file."));
		return;
	}

	m_fileList.GetCurrentFolder(theApp.curDir);
	SetCurrentDirectory(theApp.curDir.GetBuffer());

	POSITION pos = m_fileList.GetFirstSelectedItemPosition();
	std::string fileName = CStringA(m_fileList.GetItemText(m_fileList.GetNextSelectedItem(pos), 0)).GetString();
	//avoid Windows hiding file extension name
	if (0 != fileName.substr(fileName.length()-4,4).compare(".csv")) fileName += ".csv";
	theApp.csvFile.m_inputFilename = fileName;
	
	if (0 == theApp.csvFile.setFileType()){
		AfxMessageBox(_T("Sorry. File Type error. We need a csv TREE file with specific format."));
		return;
	}
	theApp.csvFile.readFileData();
	
	CDialogEx::OnOK();
}


void OpenCsvDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	if (!theApp.curDir.IsEmpty()){
		m_treePath.SelectPath(theApp.curDir.GetBuffer(MAX_PATH));
	}
	

}

void OpenCsvDlg::filterFileDisplay(){
	CString strFileType;
	CString const acceptedFileType("Comma Separated Values File");
	for (int i = 0; i < m_fileList.GetItemCount(); i++){
		strFileType = m_fileList.GetItemText(i, 2);
		if (-1 == strFileType.Find(acceptedFileType)){
			m_fileList.DeleteItem(i);
			i--;
		}
	}
}


void OpenCsvDlg::OnTvnSelchangedMfcshelltree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CString strPath;
	if (m_treePath.GetItemPath(strPath)){
		m_fileList.DisplayFolder(strPath);
		filterFileDisplay();
		m_fileList.Sort(3); //sort by ascending of modifying time;
	}

	*pResult = 0;
}
