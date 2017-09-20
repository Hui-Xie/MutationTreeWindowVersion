#pragma once
#include "TreeFactory.h"

#define NodeW  98
#define NodeH  22
#define TextOffsetX 3
#define TextOffsetY 1
#define RootX1  5
#define MaxDrawLayers 10

//node in tree diagram
struct Tile {
	int m_index;
	std::string m_gene;
	int m_x1;
	int m_y1;
	int m_x2;
	int m_y2;
	int m_parent; //vector index in the  m_nodeVector, also the index in VAFVector
	int m_lchild; //vector index in the  m_nodeVector, also the index in VAFVector
	int m_rchild; //vector index in the  m_nodeVector, also the index in VAFVector
};


class TreeDiagram
{
public:
	//CList<TreeFactory> * m_binTreeList;
	std::vector<Tile> m_nodeDiagramVector;
	int m_currentTree;
	int m_totalTree;
	CDC m_memDC;
	CBitmap m_memBitmap;
	CDC* m_pDC;
	int m_xMax;
	int m_yMax;
	std::string m_imageName;

	
public:
	TreeDiagram();
	TreeDiagram(CList<TreeFactory> * binTreeList);
	~TreeDiagram();

	void buildNodeDiagramVector(const std::vector<Node>& m_nodeVector);
	void getXYRange();
	
	void drawCurTree(CDC* pDC);
	void drawPreTree(CDC* pDC);
	void drawNextTree(CDC* pDC);

	void drawRecursiveNodes(const int index);
	void createMemBitmap(CDC* pDC);
	void releaseMemBitmap();
	
	int getNodeIndex(int x, int y);
	int saveDiagram(const unsigned long fileIndex);
	
	CString showNode3Generations(int nodeIndex);//show parent, node, children

private:
	void calculateChildXY(const int parentIndex);
	
	void drawNode(const int index);
	void drawParentChildLine(const int parent, const int child);//Leftchild and rightchild are different

	int leftChild(int parent);
	int rightChild(int parent);
	int parent(int child);
	Tile& getNode(int index);

};

