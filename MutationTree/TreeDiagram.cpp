#include "stdafx.h"
#include "TreeDiagram.h"
#include "MutationTree.h"


TreeDiagram::TreeDiagram(CList<TreeFactory> * binTreeList)
{
	TreeDiagram();
}


TreeDiagram::~TreeDiagram()
{ 
	releaseMemBitmap();
}

TreeDiagram::TreeDiagram(){
	m_xMax = 0;
	m_yMax = 0;
	m_pDC = &m_memDC;
}


Tile& TreeDiagram::getNode(int index){
	return m_nodeDiagramVector.at(index);
}
int TreeDiagram::leftChild(int parent){
	return m_nodeDiagramVector.at(parent).m_lchild;
}

int TreeDiagram::rightChild(int parent){
	return m_nodeDiagramVector.at(parent).m_rchild;
}

int TreeDiagram::parent(int child){
	return m_nodeDiagramVector.at(child).m_parent;
}


void TreeDiagram::calculateChildXY(const int parentIndex){
	if (-1 == parentIndex)  return;
	
	const Tile& parent = getNode(parentIndex);
	Tile grandad;
	if (theApp.treeFactory.m_iterShowingTree->m_root == parentIndex){  //virtual grandad
		grandad.m_x1 = parent.m_x1 - 3*NodeW/2;
		grandad.m_y1 = 0;
		grandad.m_x2 = grandad.m_x1 +NodeW;
		grandad.m_y2 = grandad.m_y1+NodeH;
	}
	else {
		grandad = getNode(parent.m_parent);
	}

	//Left child recursive
	if (-1 != parent.m_lchild){
		Tile& child = getNode(parent.m_lchild);
		child.m_x1 = parent.m_x1 + 3 * NodeW / 2;
		child.m_y1 = parent.m_y1 + abs(parent.m_y1 - grandad.m_y1) / 2;
		child.m_x2 = child.m_x1 + NodeW;
		child.m_y2 = child.m_y1 + NodeH;
		calculateChildXY(child.m_index);
	}
	
	if (-1 != parent.m_rchild){
		Tile& child = getNode(parent.m_rchild);
		child.m_x1 = parent.m_x1 + 3 * NodeW / 2;
		child.m_y1 = parent.m_y1 - abs(parent.m_y1 - grandad.m_y1) / 2;
		child.m_x2 = child.m_x1 + NodeW;
		child.m_y2 = child.m_y1 + NodeH;
		calculateChildXY(child.m_index);
	}
}

void TreeDiagram::buildNodeDiagramVector(const std::vector<Node>& m_nodeVector){
	int size = (int) m_nodeVector.size();
	m_nodeDiagramVector.resize(size);
	Tile tempTile;
	tempTile.m_x1 = 0;
	tempTile.m_y1 = 0;
	tempTile.m_x2 = 0;
	tempTile.m_y2 = 0;
	
	for (int i = 0; i < size; i++){
		tempTile.m_index = m_nodeVector.at(i).m_index;
		tempTile.m_parent = m_nodeVector.at(i).m_parent;
		tempTile.m_lchild = m_nodeVector.at(i).m_lchild;
		tempTile.m_rchild = m_nodeVector.at(i).m_rchild;
		tempTile.m_gene = theApp.VAFVector.at(i).m_gene;
		m_nodeDiagramVector.at(i) = tempTile;
	}

	//Get nLayers
	int N = 0;
	for (int i = 0; i < size; i++){
		if (m_nodeVector.at(i).m_layer > N) N = m_nodeVector.at(i).m_layer;
	}
	
	Tile& rootTile = getNode(theApp.treeFactory.m_iterShowingTree->m_root);
		
	rootTile.m_x1 = RootX1;
	N = N < MaxDrawLayers ? N : MaxDrawLayers;
	rootTile.m_y1 = int(pow(2, N - 2)*NodeH) - NodeH / 2;
			
	rootTile.m_x2 = rootTile.m_x1+ NodeW;
	rootTile.m_y2 = rootTile.m_y1 + NodeH;

	calculateChildXY(theApp.treeFactory.m_iterShowingTree->m_root);

	getXYRange();

}

void TreeDiagram::drawNode(const int index){
	if (-1 == index) return;
	const Tile& node = getNode(index);
	m_pDC->Rectangle(node.m_x1, node.m_y1, node.m_x2, node.m_y2);
	m_pDC->TextOut(node.m_x1 + TextOffsetX, node.m_y1 + TextOffsetY, CString(node.m_gene.c_str()));
}

//Leftchild and rightchild are different
void TreeDiagram::drawParentChildLine(const int parent, const int child){
	if (-1 == child || -1 == parent) return;
	Tile& parentNode = getNode(parent);
	Tile& childNode = getNode(child);
	if (child == parentNode.m_lchild){
		m_pDC->MoveTo(parentNode.m_x2,parentNode.m_y2);
		m_pDC->LineTo(childNode.m_x1,childNode.m_y1+NodeH/2);
	}
	if (child == parentNode.m_rchild){
		m_pDC->MoveTo(parentNode.m_x2, parentNode.m_y2-NodeH);
		m_pDC->LineTo(childNode.m_x1, childNode.m_y1 + NodeH / 2);
	}
}

void TreeDiagram::drawRecursiveNodes(const int index){
	if (-1 == index) return;
	drawNode(index);
	const Tile& node = getNode(index);
	if (-1 != node.m_lchild){
		drawParentChildLine(index, node.m_lchild);
		drawRecursiveNodes(node.m_lchild);
	}
	if (-1 != node.m_rchild){
		drawParentChildLine(index, node.m_rchild);
		drawRecursiveNodes(node.m_rchild);
	}

}


int TreeDiagram::getNodeIndex(int x, int y){
	int size = (int)m_nodeDiagramVector.size();
	int result = -1;
	for (int i = 0; i < size; i++){
		Tile& node = getNode(i);
		if (-1 == node.m_parent && theApp.treeFactory.m_iterShowingTree->m_root != i) continue;
		if (x >= node.m_x1 && x <= node.m_x2 && y >= node.m_y1 && y <= node.m_y2){
			result = i;
			break;
		} 
	}
	return result;
}

CString TreeDiagram::showNode3Generations(int nodeIndex){
	
	int parent = getNode(nodeIndex).m_parent;
	int brother = -1;
	if (-1 != parent){
		brother = (getNode(parent).m_lchild == nodeIndex) ? getNode(parent).m_rchild : getNode(parent).m_lchild;
	}
	
	int lchild = getNode(nodeIndex).m_lchild;
	int rchild = getNode(nodeIndex).m_rchild;

	VAF NullVAF;
	NullVAF.m_gene = "NULL";
	NullVAF.m_chromosome = " ";
	NullVAF.m_position = " ";
	NullVAF.m_reference = ' ';
	NullVAF.m_alternative = ' ';
	NullVAF.m_depth = 0;
	NullVAF.m_refRead = 0;
	NullVAF.m_altRead = 0;
	NullVAF.m_refFreq = 0;
	NullVAF.m_altFreq = 0;
	NullVAF.m_2pq = 0;

	VAF parentVAF = NullVAF;
	VAF brotherVAF = NullVAF;
	VAF selfVAF = theApp.VAFVector.at(nodeIndex);
	VAF lchildVAF = NullVAF;
	VAF rchildVAF = NullVAF;

	if (-1 != parent) parentVAF = theApp.VAFVector.at(parent);
	if (-1 != brother) brotherVAF = theApp.VAFVector.at(brother);
	if (-1 != lchild) lchildVAF = theApp.VAFVector.at(lchild);
	if (-1 != rchild) rchildVAF = theApp.VAFVector.at(rchild);

	CString nodeText;
	CString temp;
	nodeText += "Node Property of Three Generations:\r\n\r\n";
	temp = _T("\t Parent \t Brother \t SELF \t LChild \t RChild \r\n");
	nodeText += temp;

	temp.Format(_T("GENE:  %s  %s  %s  %s  %s\r\n"), 
		            CString(parentVAF.m_gene.c_str ()), CString(brotherVAF.m_gene.c_str()), CString(selfVAF.m_gene.c_str()),
					CString(lchildVAF.m_gene.c_str()), CString(rchildVAF.m_gene.c_str()));
	nodeText += temp;

	temp.Format(_T("Chrom: \t %s \t %s \t %s \t %s \t %s \r\n"),
		CString(parentVAF.m_chromosome.c_str()), CString(brotherVAF.m_chromosome.c_str()), CString(selfVAF.m_chromosome.c_str()),
		CString(lchildVAF.m_chromosome.c_str()), CString(rchildVAF.m_chromosome.c_str()));
	nodeText += temp;

	/* Do not show
	temp.Format(_T("Position: \t %s \t %s \t %s \t %s \t %s \r\n"),
		parentVAF.m_position, brotherVAF.m_position, selfVAF.m_position,
		lchildVAF.m_position, rchildVAF.m_position);
	nodeText += temp;
	*/
	temp.Format(_T("Ref: \t %c \t %c \t %c \t %c \t %c \r\n"),
		parentVAF.m_reference, brotherVAF.m_reference, selfVAF.m_reference,
		lchildVAF.m_reference, rchildVAF.m_reference);
	nodeText += temp;

	temp.Format(_T("Alt: \t %c \t %c \t %c \t %c \t %c \r\n"),
		parentVAF.m_alternative, brotherVAF.m_alternative, selfVAF.m_alternative,
		lchildVAF.m_alternative, rchildVAF.m_alternative);
	nodeText += temp;

	temp.Format(_T("Depth: \t %d \t %d \t %d \t %d \t %d \r\n"),
		parentVAF.m_depth, brotherVAF.m_depth, selfVAF.m_depth,
		lchildVAF.m_depth, rchildVAF.m_depth);
	nodeText += temp;
	
	temp.Format(_T("RefRead:\t %d \t %d \t %d \t %d \t %d \r\n"),
		parentVAF.m_refRead, brotherVAF.m_refRead, selfVAF.m_refRead,
		lchildVAF.m_refRead, rchildVAF.m_refRead);
	nodeText += temp;
	
	temp.Format(_T("AltRead: \t %d \t %d \t %d \t %d \t %d \r\n"),
		parentVAF.m_altRead, brotherVAF.m_altRead, selfVAF.m_altRead,
		lchildVAF.m_altRead, rchildVAF.m_altRead);
	nodeText += temp;

	temp.Format(_T("RefFreq: \t %.2f \t %.2f \t %.2f \t %.2f \t %.2f \r\n"),
		parentVAF.m_refFreq, brotherVAF.m_refFreq, selfVAF.m_refFreq,
		lchildVAF.m_refFreq, rchildVAF.m_refFreq);
	nodeText += temp;

	temp.Format(_T("AltFreq:\t %.2f \t %.2f \t %.2f \t %.2f \t %.2f \r\n"),
		parentVAF.m_altFreq, brotherVAF.m_altFreq, selfVAF.m_altFreq,
		lchildVAF.m_altFreq, rchildVAF.m_altFreq);
	nodeText += temp;

	double parentErr = brotherVAF.m_altFreq + selfVAF.m_altFreq - parentVAF.m_altFreq;
	double selfErr = lchildVAF.m_altFreq + rchildVAF.m_altFreq - selfVAF.m_altFreq;
	
	temp.Format(_T("Epsilon: \t %.2f \t   \t %.2f \t   \t   \r\n"), parentErr, selfErr);
	nodeText += temp;
	
	return nodeText;
}

void TreeDiagram::getXYRange(){
	const int size = (const int)m_nodeDiagramVector.size();
	m_xMax = 0;
	m_yMax = 0;
	for (int i = 0; i < size; i++){
		Tile& node = getNode(i);
		if (-1 != node.m_parent){
			if (node.m_x2 > m_xMax) m_xMax = node.m_x2;
			if (node.m_y2 > m_yMax) m_yMax = node.m_y2;
		}
	}

	m_xMax = m_xMax + NodeW;
	m_yMax = m_yMax + NodeH;

}

//this method is called after calculate the nodes' coordinates.
void TreeDiagram::createMemBitmap(CDC* pDC){
	m_pDC->CreateCompatibleDC(pDC);
	m_memBitmap.CreateCompatibleBitmap(pDC, m_xMax, m_yMax);
	m_pDC->SelectObject(m_memBitmap);
	m_pDC->FillSolidRect(0, 0, m_xMax, m_yMax, RGB(255, 255, 255));
	
}

void TreeDiagram::releaseMemBitmap(){
	m_pDC->DeleteDC();
	m_memBitmap.DeleteObject();
}


int TreeDiagram::saveDiagram(const unsigned long fileIndex){

	m_imageName = theApp.treeFactory.m_iterShowingTree->buildTreeBasicFilename(fileIndex) + ".jpg";
	CImage image;
	image.Attach(m_memBitmap);
	image.Save(CString(m_imageName.c_str()));
	image.Detach();
	return 1;
}