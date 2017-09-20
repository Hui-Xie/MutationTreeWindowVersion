#include "stdafx.h"
#include "CSVFile.h"
#include "VAF_CSVTree_struct.h"
#include "MutationTree.h"
#include <string>
#include "MutationTreeDlg.h"

#define LineLength 300

CSVFile::CSVFile()
{
	m_fileType = 0;
}


CSVFile::~CSVFile()
{

	if (m_inputFile.m_hFile != CFile::hFileNull) m_inputFile.Close();
	if (m_outputFile.m_hFile != CFile::hFileNull) m_outputFile.Close();
	
}



int CSVFile::setFileType(){
	m_fileType = 0;
	char firstLine[LineLength];
	int temp = readFirstLine(firstLine, LineLength);
	if (-1 == temp) return m_fileType;
	
	
	int nCommas = countCommas(firstLine);
	//only open tree file

	//if (VAF9 == nCommas && 0 == getFirstItem(firstLine).compare("GENE")) m_fileType = VAF9;
	//else if (VAF10 == nCommas && 0 == getFirstItem(firstLine).compare("GENE")) m_fileType = VAF10;
	if (TREE11 == nCommas && 0 == getFirstItem(firstLine).compare("TreeID")) m_fileType = TREE11;
	else m_fileType = 0;
	
	return m_fileType;
}

int CSVFile::countCommas(char* lineText){
	char *p = lineText;
	int counter = 0;
	while ('\0' != *p){
		if (',' == *p)  counter++;
		p++;
	}
	return counter;
}

std::string CSVFile::getFirstItem(char* lineText){
	char *p = lineText;
	int counter = 0;
	while (',' != *p && '\0'!=*p){
		counter++;
		p++;
	}
	counter= counter+1;
	char* str = new char[counter];
	for (int i = 0; i < counter - 1; i++){
		*(str + i) = *(lineText + i);
	}
	*(str + counter - 1) = '\0';
	std::string item(str);
	delete[] str;
	//item = item.TrimLeft().TrimRight();
	return item;
}

void CSVFile::removeAllSpaces(char* buffer, int nSize){
	char* iter = buffer;
	int nCount = 0;
	while ('\0' == *iter ){
		if (' ' == *iter){
			memcpy(iter, iter + 1, nSize - nCount - 1);
		}
		nCount++;
		iter++;
	}

}

//result: p+result will point to The Carriage 
int  CSVFile::getOffsetToCarriage(char* p){
	char* iter = p;
	int nCount = 0;
	while ('\n' != *iter && '\0' != *iter){
		nCount++;
		iter++;
	}
	return nCount;
}

void  CSVFile::initilizeVAFNode(VAF& node){

	node.m_reference = ' ';
	node.m_alternative = ' ';
	node.m_depth = 0;
	node.m_refRead = 0;
	node.m_altRead = 0;
	node.m_refFreq = 0;
	node.m_altFreq = 0;
	node.m_2pq = 0;

}

void CSVFile::initilizeBinNode(Node& node){
	node.m_index = -1;
	node.m_layer = -1;
	node.m_treeID = "";
	node.m_parent = -1;
	node.m_lchild = -1;
	node.m_rchild = -1;
}

//return the pointer in the next line
char* CSVFile::parseVAFLine(char* p, VAF& node){
	
	if ('\0' == *p) return p;

	initilizeVAFNode(node);

	char* iter = p;
	int nItem = 0;
	while (true){
		int itemOffset = 0;
		char *itemBeginning = iter;
		while (',' != *iter && '\n' != *iter && '\0' != *iter){
			itemOffset++;
			iter++;
		}
		nItem++;
		int itemLength = itemOffset + 1;
		char* item = new char[itemLength];
		memcpy(item, itemBeginning, itemOffset);
		item[itemLength - 1] = '\0';
				
		switch (nItem){
		case 1:
			node.m_gene = std::string(item);
			break;
		case 2:
			node.m_chromosome = std::string(item);
			break;
		case 3:
			node.m_position = std::string(item);
			break;
		case 4:
			node.m_reference = *item;
			break;
		case 5:
			node.m_alternative = *item;
			break;
		case 6:
			node.m_depth = atoi(item);
			break;
		case 7:
			node.m_refRead = atoi(item);
			break;
		case 8:
			node.m_altRead = atoi(item);
			break;
		case 9:
			node.m_refFreq = roundf((float)atof(item), 2);
			break;
		case 10:
			node.m_altFreq = roundf((float)atof(item), 2);
			break;
		case 11:
			node.m_2pq = roundf((float)atof(item), 2);
			break;
		default:
			CString text;
			text.Format(_T("There is an error in Parsing VAF line. item: %s."), item);
			((CMutationTreeDlg*)(theApp.m_pMainWnd))->m_editStatusBar.SetWindowText(text);
		}
		delete[] item;

		if ('\n' == *iter)  return ++iter; //point to next line
		else if ('\0' == *iter)  return iter;
		else iter++; //current iter pointing to comma
	}

}


char* CSVFile::parseTreeLine(char* p, VAF& vafNode, Node& binNode){

	if ('\0' == *p) return p;

	initilizeVAFNode(vafNode);
	initilizeBinNode(binNode);

	char* iter = p;
	int nItem = 0;
	while (true){
		int itemOffset = 0;
		char *itemBeginning = iter;
		while (',' != *iter && '\n' != *iter && '\0' != *iter){
			itemOffset++;
			iter++;
		}
		nItem++;
		int itemLength = itemOffset + 1;
		char* item = new char[itemLength];
		memcpy(item, itemBeginning, itemOffset);
		item[itemLength - 1] = '\0';

		switch (nItem){
		case 1:
			binNode.m_treeID = std::string(item);
			break;
		case 2:
			binNode.m_layer = atoi(item);
			break;
		case 3:
			vafNode.m_gene = std::string(item);
			break;
		case 4:
			vafNode.m_chromosome = std::string(item);
			break;
		case 5:
			vafNode.m_position = std::string(item);
			break;
		case 6:
			vafNode.m_reference = *item;
			break;
		case 7:
			vafNode.m_alternative = *item;
			break;
		case 8:
			vafNode.m_depth = atoi(item);
			break;
		case 9:
			vafNode.m_refRead = atoi(item);
			break;
		case 10:
			vafNode.m_altRead = atoi(item);
			break;
		case 11:
			vafNode.m_refFreq = roundf((float)atof(item),2);
			break;
		case 12:
			vafNode.m_altFreq = roundf((float)atof(item),2);
			binNode.m_altFreq = roundf((float)atof(item),2);
			break;
		default:
			CString text;
			text.Format(_T("There is an error in Parsing Tree line. item: %s."), item);
			((CMutationTreeDlg*)(theApp.m_pMainWnd))->m_editStatusBar.SetWindowText(text);
		}
		delete[] item;

		if ('\n' == *iter)  return ++iter; //point to next line
		else if ('\0' == *iter)  return iter;
		else iter++; //current iter pointing to comma
	}
}

//return the effective length of first line
int CSVFile::readFirstLine(char* pLine,const int size){
	FILE* pFile = NULL;
	fopen_s(&pFile, m_inputFilename.c_str(), "rb");
	if (NULL == pFile) {
		AfxMessageBox(_T("Sorry. Can not open file. If it is open, please close it and retry."));
		return -1;
	}
	fread(pLine,sizeof(char),size,pFile);
	fclose(pFile);

	
	int length = 0;

	bool isFirstLine = true;
	for (int i = 0; i < size; i++){
		if ('\n' == *(pLine + i) && isFirstLine) {
			isFirstLine = false;
			length = i + 1;
			continue;
		}
		if (!isFirstLine)  *(pLine + i) = '\0';
	}
	return length;
}

float CSVFile::roundf(float x, int nDecimal)
{
	int ploid = 1;
	for (int i = 0;i < nDecimal;i++) {
		ploid = ploid * 10;
	} 
	return round(x*ploid)/ploid;
}

int CSVFile::readFileData()
{
	m_inputFile.Open(CString(m_inputFilename.c_str()), CFile::modeRead);
    int nFileSize = (int)m_inputFile.GetLength();
	char* buffer = new char[nFileSize+1];
	int readSize = m_inputFile.Read(buffer, nFileSize);
	m_inputFile.Close();

	*(buffer + readSize) = '\0';
	char* p = buffer;
	removeAllSpaces(buffer, nFileSize + 1);
	int lineOffset = 0;
	lineOffset = getOffsetToCarriage(p);
	p += lineOffset + 1; //let p point to first Non-head data row;

	theApp.VAFVector.clear();
	theApp.treeFactory.initialize();
			
	if (VAF9 == m_fileType || VAF10 == m_fileType){
		while ('\0' != *p){
			VAF newNode;
			p = parseVAFLine(p, newNode);
			theApp.VAFVector.push_back(newNode);
		}
		theApp.VAFVector.shrink_to_fit();
		sortVAFVector();
	}

	if (TREE11 == m_fileType){
		int index = 0;
		std::vector<Node> nodeVector;
		while ('\0' != *p){
			VAF vafNode;
			Node binNode;
			p = parseTreeLine(p, vafNode,binNode);
			theApp.VAFVector.push_back(vafNode);
			binNode.m_index = index++;
			nodeVector.push_back(binNode);
		}
		theApp.VAFVector.shrink_to_fit();
		theApp.treeFactory.initializeVAFVector(nodeVector);
	}
			
	delete[] buffer;
	
	return 0;
}


//in descending order of m_altFreq
void CSVFile::sortVAFVector(){
	int size = (int) theApp.VAFVector.size();
	for (int i = 0; i < size - 1; i++){
		int max = i;
		for (int j = i + 1; j < size; j++){
			if (theApp.VAFVector.at(j).m_altFreq > theApp.VAFVector.at(max).m_altFreq) max = j;
		}

		if (max != i){
			VAF temp;
			temp = theApp.VAFVector.at(i);
			theApp.VAFVector.at(i) = theApp.VAFVector.at(max);
			theApp.VAFVector.at(max) = temp;
		}
	}
}


