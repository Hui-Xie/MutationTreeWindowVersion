#pragma once
#include "VAF_CSVTree_struct.h"
#include "TreeFactory.h"

#define VAF9  9
#define VAF10 10
#define TREE11 11

/*
return value: 0, Error file format

m_filetype: VAF9:VAF file without 2pq; 9 commas;
GENE,CHROM,POS,REF,ALT,DEPTH,REF_READ_NUM,ALT_READ_NUM,REF_FREQ,ALT_FREQ\n

m_filetype: VAF10:VAF file with 2pq; 10 commas;
GENE,CHROM,POS,REF,ALT,DEPTH,REF_READ_NUM,ALT_READ_NUM,REF_FREQ,ALT_FREQ,2pq\n

m_filetype: TREE11: Tree File; 11 commas;
TreeID,Layer,GeneName,Chromosome,Position,Reference,Alternative,Depth,RefRead,AltRead,RefFreq,AltFreq\n
*/

class CSVFile
{
public:
	std::string m_inputFilename;
	CFile m_inputFile;
	CFile m_outputFile;
	std::string m_outputFileName;
		
	int m_fileType; 


public:
	CSVFile();
	~CSVFile();

	int readFileData();
	int setFileType();
	void sortVAFVector();
	


private:
	int countCommas(char* lineText);
	std::string getFirstItem(char* lineText);
	int getOffsetToCarriage(char* p);
	void removeAllSpaces(char* buffer, int nSize);
	char* parseVAFLine(char* p, VAF& node);
	char* parseTreeLine(char* p, VAF& vafNode, Node& binNode);
	void initilizeVAFNode(VAF& node);
	void initilizeBinNode(Node& node);

	int readFirstLine(char* pLine, const int size);

	float roundf(float x, int nDecimal);
	
};

