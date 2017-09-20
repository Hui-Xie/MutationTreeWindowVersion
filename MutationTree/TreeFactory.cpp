#include "stdafx.h"
#include "TreeFactory.h"
#include "MutationTree.h"


#define ZeroTriEpsilon 0.0001f
#define MaxBestTreeNorm 1e+9f



Node::Node()
{
	m_index = -1;
	m_layer = -1;
	m_treeID = "";
	m_parent = -1;
	m_lchild = -1;
	m_rchild = -1;
	m_altFreq = 0;
}

Node::~Node()
{
}

Node & Node::operator=(const Node & other)
{
	this->m_index = other.m_index;
	this->m_layer = other.m_layer;
	this->m_treeID = other.m_treeID;
	this->m_parent = other.m_parent;
	this->m_lchild = other.m_lchild;
	this->m_rchild = other.m_rchild;
	this->m_altFreq = other.m_altFreq;
	return *this;
}

Tree::Tree()
{
	m_root = -1;
	m_nLayers = 0;//total layers
	m_nTreeNodes = 0; //total nodes
	m_nInternalNodes = 0;
	m_NumRemainingNodes = 0;
	m_richness = 0.0f;

	m_sumSquareEps = 0.0f;
	m_averageEpsilon = 0.0f;//square root of the average of square deviation
	m_minEpsilon = 0.0f;
	m_maxEpsilon = 0.0f;
	
	m_bestTreeNorm = MaxBestTreeNorm;

	m_nodeVector.clear();  //only nodes with parent unqual -1 are inside the binary tree
	m_heap.clear();
}

Tree::~Tree()
{
	m_nodeVector.clear();
	m_heap.clear();
}

Tree & Tree::operator=(Tree & other)
{
	this->m_root = other.m_root;
	this->m_nLayers = other.m_nLayers;
	this->m_nTreeNodes = other.m_nTreeNodes;
	this->m_nInternalNodes = other.m_nInternalNodes;
	this->m_NumRemainingNodes = other.m_NumRemainingNodes;
	this->m_richness = other.m_richness;

	this->m_averageEpsilon = other.m_averageEpsilon;
	this->m_minEpsilon = other.m_minEpsilon;
	this->m_maxEpsilon = other.m_maxEpsilon;
	this->m_sumSquareEps = other.m_sumSquareEps;

	this->m_bestTreeNorm = other.m_bestTreeNorm;

	this->m_nodeVector.clear();
	for (std::vector<Node>::iterator iter = other.m_nodeVector.begin();
	           iter != other.m_nodeVector.end();iter++) {
		this->m_nodeVector.push_back(*iter);
	}
	
	m_heap.clear();
	for (std::list<int>::iterator iter = other.m_heap.begin(); iter != other.m_heap.end(); iter++) {
		m_heap.push_back(*iter);
	}
	return *this;

}




//LorR: 1: Left Child; 2: Right Child
std::string Tree::contructTreeID(int Parent, int LorR){
	std::string result = m_nodeVector.at(Parent).m_treeID;
	if (1 == LorR) result += "0";
	else if (2 == LorR) result += "1";
	else ASSERT(false);
	return result;
}


void Tree::initialize(){
	m_nodeVector.clear();

}

void Tree::initializeStatisticsProperty() {
	m_nTreeNodes = 0;
	m_nLayers = 0;
	m_NumRemainingNodes = 0;
	m_nInternalNodes = 0;
	m_richness = 0.0f;

	m_minEpsilon = 0.0f;
	m_maxEpsilon = 0.0f;
	m_averageEpsilon = 0.0f; //square root of the average of square deviation
	m_sumSquareEps = 0.0f;

	m_bestTreeNorm = MaxBestTreeNorm;
	
}

int Tree::getRootFromNodeVector()
{
	int const size = (int)m_nodeVector.size();
	for (int i = 0; i < size; i++) {
		if (0 == m_nodeVector.at(i).m_treeID.compare(std::string("1"))) {
			m_root = i;
			break;
		}
	}
	return m_root;
}




//only nodes with parent are inside the binary tree
void Tree::putAllCandidatesinBinTree(){
	int size = int(theApp.VAFVector.size());
	m_nodeVector.clear();
	m_nodeVector.resize(size);
	Node temp;
	for (int i = 0; i < size; i++){
		temp.m_index = i;
		temp.m_altFreq = theApp.VAFVector.at(i).m_altFreq;
		m_nodeVector.at(i) = temp;
	} 
}

void Tree::buildTree(){
	
	if (1 == theApp.constraint.m_type){
		putAllCandidatesinBinTree();
		m_root = 0;
		getNode(m_root).m_layer = 1;
		getNode(m_root).m_treeID = "1";
		//extendTreeDFS(m_root);
		extendTreeDFS(0);
	}

	else if (2 == theApp.constraint.m_type){
		extendTreeBFS();
		//extendTreeBFSinRange();
	}
	else {
		AfxMessageBox(_T("Error: theApp.constraint.m_type is not correct."));
	}

}


void Tree::initializeRootNode(int root) {
	m_heap.clear();
	m_root = root;
	m_heap.push_back(m_root);
	getNode(m_root).m_layer = 1;
	getNode(m_root).m_treeID = std::string("1");
	
}

void Tree::addChildrenNodes(const int parent, const int lc, const int rc) {
	
	Node& nodeP = getNode(parent);
	Node& nodeLc = getNode(lc);
	Node& nodeRc = getNode(rc);
	
	m_heap.push_back(nodeLc.m_index);
	m_heap.push_back(nodeRc.m_index);
	nodeP.m_lchild = nodeLc.m_index;
	nodeP.m_rchild = nodeRc.m_index;
	nodeLc.m_parent = nodeP.m_index;
	nodeRc.m_parent = nodeP.m_index;

	nodeLc.m_layer = nodeP.m_layer + 1;
	nodeRc.m_layer = nodeP.m_layer + 1;

	nodeLc.m_treeID = contructTreeID(nodeP.m_index, 1);
	nodeRc.m_treeID = contructTreeID(nodeP.m_index, 2);
}

float Tree::getTriConstraintEps() {
	if (0 == theApp.constraint.m_TriConstraintEps) return ZeroTriEpsilon;
	else return theApp.constraint.m_TriConstraintEps / 100.0f;
}

float Tree :: getLeastRichness() {
	return theApp.constraint.m_LeastRichness / 100.0f;
}

bool Tree::spareNodesExceedLeastRichness(int const index1, int const index2) {
	//get minimum Index in m_heap
	int size = (int)m_nodeVector.size();
	int minIndex = size;
	std::list<int>::iterator iter = m_heap.begin();
	while (iter != m_heap.end()) {
		if (*iter < minIndex) minIndex = *iter;
		iter++;
	}

	int newIndex = (index1 < index2) ? index1 : index2;
	minIndex = (minIndex < newIndex) ? minIndex : newIndex;

	//get spare nodes number
	int nSpareNodes = 0;
	for (int i = m_root+1; i < minIndex;i++) {
		if (-1 == m_nodeVector.at(i).m_parent) nSpareNodes++;
	}

	if (nSpareNodes*1.0 / size > (1 - getLeastRichness())) return true;
	else return false;
}


void Tree::bfsExploreTree(std::list<Tree>* pTreeList) {
	const int size = (const int)m_nodeVector.size();
	bool exceptionalStop = false;
	while (0 != m_heap.size()) {
		if (exceptionalStop) break;
		int parent = m_heap.front();
		Node& nodeP = getNode(parent);
		m_heap.pop_front();

		float const epsilon = getTriConstraintEps();
		
		//define self m_heap changing
		bool isFindChildren = false;
		int selfP = -1;
		int selfi = -1;
		int selfj = -1;

		Tree newTree = *this;

		for (int i = parent + 1; i < size-1; i++) {
			if (exceptionalStop) break;

			Node& nodei = getNode(i);
			if (-1 != nodei.m_parent) continue;
			for (int j = i + 1; j < size; j++) {
				if (true == theApp.stopComputation) {
					exceptionalStop = true;
					break;
				}
				Node& nodej = getNode(j);
				if (-1 != nodej.m_parent) continue;
				float error = nodei.m_altFreq + nodej.m_altFreq - nodeP.m_altFreq;
				if (abs(error) < epsilon) {
					if (!spareNodesExceedLeastRichness(i, j)) {
						if (false == isFindChildren) {
							pTreeList->push_back(newTree);
							pTreeList->back().addChildrenNodes(parent, j, i);
							isFindChildren = true;
							selfP = parent;
							selfi = i;
							selfj = j;
						}
						else {
							pTreeList->push_back(newTree);
							pTreeList->back().addChildrenNodes(parent, i, j);
							pTreeList->push_back(newTree);
							pTreeList->back().addChildrenNodes(parent, j, i);
						}
					}
		    	}
				else{
					if (error < 0) break;
				}
			}
			
		}

		if (isFindChildren) {
			addChildrenNodes(selfP, selfi, selfj);
		}
	}
}


void Tree::extendTreeBFS(){
	
	putAllCandidatesinBinTree();

	const int size = (const int)m_nodeVector.size();
	
	std::list<int> heap;
	heap.push_back(theApp.treeFactory.m_iterShowingTree->m_root);
	getNode(0).m_layer = 1;
	getNode(0).m_treeID = std::string("1");

	while (0 != heap.size()){
		int parent = heap.front();
		Node& nodeP = getNode(parent);
		heap.pop_front();
		
		int lc = -1;
		int rc = -1;
		float epsilon = getTriConstraintEps();
		
		for (int i = parent + 1; i < size-1; i++){
			Node& nodei = getNode(i);
			if (-1 != nodei.m_parent) continue;
			for (int j = i + 1; j < size; j++){
				Node& nodej = getNode(j);
				if (-1 != nodej.m_parent) continue;

				float error = nodei.m_altFreq + nodej.m_altFreq - nodeP.m_altFreq;
				if (abs(error) < epsilon){
					lc = i;
					rc = j;
					epsilon = abs(error);
				}
				if (error < 0) break;
			}
		}

		if (-1 != lc && -1 != rc && epsilon < getTriConstraintEps()){
			heap.push_back(lc);
			heap.push_back(rc);
			nodeP.m_lchild = lc;
			nodeP.m_rchild = rc;
			getNode(lc).m_parent = parent;
			getNode(rc).m_parent = parent;

			getNode(lc).m_layer = nodeP.m_layer + 1;
			getNode(rc).m_layer = nodeP.m_layer + 1;

			getNode(lc).m_treeID = contructTreeID(parent,1);
			getNode(rc).m_treeID = contructTreeID(parent,2);
		}
	}
}

void Tree::extendTreeDFS(int parent){
	const int size = (const int)m_nodeVector.size();
	if (parent + 1 >= size) return;

	Node& nodeP = getNode(parent);
	int lc = -1;
	int rc = -1;
	float epsilon = getTriConstraintEps();

	for (int i = parent + 1; i < size; i++){
		Node& nodei = getNode(i);
		if (-1 != nodei.m_parent) continue;
		for (int j = i + 1; j < size; j++){
			Node& nodej = getNode(j);
			if (-1 != nodej.m_parent) continue;

			float error = nodei.m_altFreq + nodej.m_altFreq - nodeP.m_altFreq;
			if (abs(error) < epsilon){
				lc = i;
				rc = j;
				epsilon = abs(error);
			}
			if (error < 0) break;
		}
	}

	if (-1 != lc && -1 != rc && epsilon < getTriConstraintEps()){
		nodeP.m_lchild = lc;
		nodeP.m_rchild = rc;
		getNode(lc).m_parent = parent;
		getNode(rc).m_parent = parent;

		getNode(lc).m_layer = nodeP.m_layer + 1;
		getNode(rc).m_layer = nodeP.m_layer + 1;

		getNode(lc).m_treeID = contructTreeID(parent, 1);
		getNode(rc).m_treeID = contructTreeID(parent, 2);

		//recursive call for DFS
		extendTreeDFS(lc);
		extendTreeDFS(rc);
	}

}


Node& Tree::leftChild(int parent){
	return m_nodeVector.at(m_nodeVector.at(parent).m_lchild);
}


Node& Tree::rightChild(int parent){
	return m_nodeVector.at(m_nodeVector.at(parent).m_rchild);
}


Node& Tree::parent(int child){
	return m_nodeVector.at(m_nodeVector.at(child).m_parent);
}

Node& Tree::getNode(int index){
	return m_nodeVector.at(index);
}

void Tree::statistic(){

	int const size = int(m_nodeVector.size());
	if (0 == size)  return;

	initializeStatisticsProperty();
	for (int i = 0; i < size; i++){
		if (-1 != getNode(i).m_parent || m_root == i){
			m_nTreeNodes++;
			int curLayer = getNode(i).m_layer;
			if (curLayer > m_nLayers) m_nLayers = curLayer;
			if (!isLeaf(i)){
				m_nInternalNodes++;
				float eps = getTriangularEpsilon(i);
				if (m_minEpsilon > eps)  m_minEpsilon = eps;
				if (m_maxEpsilon < eps)  m_maxEpsilon = eps;
				m_sumSquareEps += eps*eps;
			}
		}
		else {
			m_NumRemainingNodes++;
		}
	}
	if (0 == m_nInternalNodes) {
		m_averageEpsilon = 1;
	}
	else {
		m_averageEpsilon = sqrt(m_sumSquareEps / m_nInternalNodes);
	}

	m_richness = m_nTreeNodes*1.0f / size;
	m_bestTreeNorm = m_sumSquareEps / (m_richness*m_richness);

}

std::string Tree::buildTreeBasicFilename(unsigned long fileIndex) {
	if (VAF9 == theApp.csvFile.m_fileType || VAF10 == theApp.csvFile.m_fileType) {
		char name[50];
		int num = sprintf_s(name, 50, "_E%d%%Tree%lu", theApp.constraint.m_TriConstraintEps, fileIndex);
		name[num] = '\0';
		std::string filename = theApp.treeFactory.m_basicFilename + "_" + theApp.constraint.getTypeName() + std::string(name);
		return filename;
	}
	else if (TREE11 == theApp.csvFile.m_fileType) {
		return theApp.treeFactory.m_basicFilename;
	}
	else {
		return std::string();
	}
	
}
	

std::string Tree::saveCsvTree(const unsigned long fileIndex) {
	
	std::string filename = buildTreeBasicFilename(fileIndex) + ".csv";

	FILE* pFile = NULL;
	fopen_s(&pFile,filename.c_str(), "w");
		
	//print table head
	int strLen = 0;
	std::string csvHead("TreeID,Layer,GENE,CHROM,POS,REF,ALT,DEPTH,REF_READ_NUM,ALT_READ_NUM,REF_FREQ,ALT_FREQ\n");
	strLen = (int)csvHead.size();
	fwrite(csvHead.c_str(), sizeof(char), strLen,pFile);

	//print data using BFS search
	std::string outLine;
	std::list<int> heap;
	heap.push_back(m_root);//push back root index
	while (0 != heap.size()) {
		int index = heap.front();
		heap.pop_front();
		Node& node = getNode(index);
		VAF& vaf = theApp.VAFVector.at(index);
		if (-1 != node.m_lchild) heap.push_back(node.m_lchild);
		if (-1 != node.m_rchild) heap.push_back(node.m_rchild);

		//construct line text;
		outLine.clear();
		char nucleotides[4];
		nucleotides[3] = '\0';
		sprintf_s(nucleotides, 4,"%c,%c", vaf.m_reference, vaf.m_alternative);
		outLine = node.m_treeID + ","
			+ std::to_string(node.m_layer) + ","
			+ vaf.m_gene + ","
			+ vaf.m_chromosome + ","
			+ vaf.m_position + ","
			+ std::string(nucleotides) + ","
			+ std::to_string(vaf.m_depth) + ","
			+ std::to_string(vaf.m_refRead) + ","
			+ std::to_string(vaf.m_altRead) + ","
			+ std::to_string(vaf.m_refFreq) + ","
			+ std::to_string(vaf.m_altFreq) + "\n";
		strLen = (int)outLine.size();
		fwrite(outLine.c_str(), sizeof(char), strLen, pFile);
	}
	fflush(pFile);
	fclose(pFile);
	return filename;
}

bool Tree::isLeaf(int index){
	if (-1 == getNode(index).m_lchild || -1 == getNode(index).m_rchild) return true;
	else return false;
}

float Tree::getTriangularEpsilon(int index){
	return leftChild(index).m_altFreq + rightChild(index).m_altFreq - getNode(index).m_altFreq;
}

void Tree::buildNodeRelationFromID(){
	getRootFromNodeVector();
	const int size = (const int)m_nodeVector.size();
	for (int i = 0; i < size-1; i++){
		Node& nodei = getNode(i);
		for (int j = i + 1; j < size; j++){
			Node& nodej = getNode(j);
			if (nodej.m_layer == nodei.m_layer + 1){
				//left child
				if (0 == nodej.m_treeID.compare(nodei.m_treeID + "0")) {
					nodei.m_lchild = nodej.m_index;
					nodej.m_parent = nodei.m_index;
				}
				//right child
				if (0 == nodej.m_treeID.compare(nodei.m_treeID + "1")) {
					nodei.m_rchild = nodej.m_index;
					nodej.m_parent = nodei.m_index;
				}
			}
			//if (nodej.m_layer > nodei.m_layer + 1) break;  //remove this sentense for mess node order of tree file.
		}
	}
}



TreeFactory::TreeFactory()
{
	initialize();
}


TreeFactory::~TreeFactory()
{
	m_treeList.clear();
}

void TreeFactory::initialize()
{
	m_basicFilename.clear();
	m_totalResutlTree = 0l;
	m_curTreeIndex = 0;
	m_nFinishedComputationTree = 0l;
	m_treeList.clear();
	
}

void TreeFactory::initializeVAFVector(std::vector<VAF>& vafVector)
{
	int size = int(vafVector.size());
	Tree tree;
	tree.m_nodeVector.resize(size);
	Node temp;
	for (int i = 0; i < size; i++) {
		temp.m_index = i;
		temp.m_altFreq = vafVector.at(i).m_altFreq;
		tree.m_nodeVector.at(i) = temp;
	}
	m_treeList.clear();
	m_treeList.push_back(tree);
	m_totalResutlTree = 0l;
	m_nFinishedComputationTree = 0l;

}

void TreeFactory::initializeVAFVector(std::vector<Node>& nodeVector) {
	m_treeList.clear();
	Tree tree;
	tree.m_nodeVector = nodeVector;
	m_treeList.push_back(tree);
	m_totalResutlTree = 0l;
	m_nFinishedComputationTree = 0l;
}

bool TreeFactory::are2TreesSameProperty(const Tree & first, const Tree & second)
{
	if (first.m_root == second.m_root
		&& first.m_nLayers == second.m_nLayers
		&& first.m_nTreeNodes == second.m_nTreeNodes
		&& first.m_nInternalNodes == second.m_nInternalNodes
		&& first.m_NumRemainingNodes == second.m_NumRemainingNodes
		&& abs(first.m_minEpsilon - second.m_minEpsilon) < ZeroTriEpsilon
		&& abs(first.m_maxEpsilon - second.m_maxEpsilon) < ZeroTriEpsilon
		&& abs(first.m_sumSquareEps - second.m_sumSquareEps) < ZeroTriEpsilon)
	{
		return true;
	}
	else return false;
}

bool TreeFactory::are2TreesSameNodes(const Tree & first, const Tree & second)
{
	const int size = (int)first.m_nodeVector.size();
	for (int i = 0;i < size;i++) {
		if (first.m_nodeVector.at(i).m_parent != second.m_nodeVector.at(i).m_parent) return false;
	}
	return true;
}

std::list<Tree>::iterator TreeFactory::getWorstInBestTrees()
{
	int size = (m_nFinishedComputationTree > NumBestTrees) ? NumBestTrees : m_nFinishedComputationTree;
	std::list<Tree>::iterator iter = m_treeList.begin();
	std::list<Tree>::iterator worst = iter;
	iter++;
	for (int i = 1; i < size && iter != m_treeList.end();i++) {
		if (isLhsBetterTree(*worst, *iter)) {
			worst = iter;
		}
		iter++;
	}
	return worst;
}

bool TreeFactory::updateTheWorstInBestTrees()
{
	if (m_nFinishedComputationTree <= NumBestTrees) return false;
	std::list<Tree>::iterator worst = getWorstInBestTrees();
	if (isLhsBetterTree(*worst, *m_curComputingTreeIter)) {
		m_curComputingTreeIter = m_treeList.erase(m_curComputingTreeIter);
	}
	else {
		m_treeList.erase(worst);
		m_curComputingTreeIter++;
	}
	return true;
}

bool TreeFactory::isLhsBetterTree(const Tree & lhs, const Tree & rhs)
{
	if (lhs.m_bestTreeNorm != rhs.m_bestTreeNorm) return lhs.m_bestTreeNorm < rhs.m_bestTreeNorm;
	else if (lhs.m_nInternalNodes != rhs.m_nInternalNodes) return lhs.m_nInternalNodes > rhs.m_nInternalNodes;
	else if (lhs.m_nLayers != rhs.m_nLayers)  return lhs.m_nLayers < rhs.m_nLayers;
	else return false;
}


bool TreeFactory::deleteIsomorphicOrOddTree()
{
	//delete odd tree
	if (m_curComputingTreeIter->m_richness < m_curComputingTreeIter->getLeastRichness())
	{
		m_curComputingTreeIter = m_treeList.erase(m_curComputingTreeIter);
		return true;
	}


	//delete isomorphic tree
	std::list<Tree>::iterator iter = m_treeList.begin();
	while(iter != m_curComputingTreeIter) {
		if (are2TreesSameProperty(*iter, *m_curComputingTreeIter))
		{
			if (are2TreesSameNodes(*iter, *m_curComputingTreeIter)) {
				//deleter curIsomorphicTree
				m_curComputingTreeIter = m_treeList.erase(m_curComputingTreeIter);
				return true;
			}
		}
		iter++;
	}
	return false;
}

int TreeFactory::buildBFSTrees()
{
	setBasicFilename();
	initializeVAFVector(theApp.VAFVector);
	initialRootAndHeapOfTreeList();
	
	std::list<Tree>::iterator iterTree = m_treeList.begin();
	m_iterShowingTree = m_treeList.begin();
	m_curTreeIndex = 0;
	//int nUpdateInforInterval = 0;
	while (iterTree != m_treeList.end()) {
		if (true == theApp.stopComputation) break;
		iterTree->bfsExploreTree(&m_treeList);
		iterTree->statistic();
		m_nFinishedComputationTree++;

		//delete isomorphic or odd tree
		m_curComputingTreeIter = iterTree;
		if (!deleteIsomorphicOrOddTree()) {
			if (updateTheWorstInBestTrees()) {
				iterTree = m_curComputingTreeIter;
			}
			else {
				iterTree++;
			}
		}
		else {
			iterTree = m_curComputingTreeIter;
		}

		//showing porgress information
		//nUpdateInforInterval++;
		//if (nUpdateInforInterval > 8000) {
			//nUpdateInforInterval = 0;
			showExploreTreeInformation();
		//}
		m_pMainWindow->processInternalMessage(); //show progress of compuation
	}
	return m_totalResutlTree = (unsigned long) m_treeList.size();

}

void TreeFactory::initialRootAndHeapOfTreeList()
{
	if (0 == m_treeList.size()) return;
	
	//precondition: the nodeVector is sorted from largest to smallest
	std::vector<Node>& nodeVector = m_treeList.front().m_nodeVector;
	int const nodeSize = (const int)nodeVector.size();
	float const maxAltFreq = nodeVector.front().m_altFreq;
	float const minAltFreq = maxAltFreq - theApp.constraint.m_RootConstraintEps/100.0f;
	for (int i = 1;i < nodeSize;i++) {
		if (nodeVector.at(i).m_altFreq <= maxAltFreq && nodeVector.at(i).m_altFreq > minAltFreq) {
			m_treeList.push_back(m_treeList.front());
			m_treeList.back().initializeRootNode(i);
		}
		else {
			break;
		}
	}
	m_treeList.front().initializeRootNode(0); //this must put after loop;

}


bool TreeFactory::compareAverageEpsilon(const Tree& lhs, const Tree& rhs) {
	if (lhs.m_averageEpsilon != rhs.m_averageEpsilon) {
		return lhs.m_averageEpsilon < rhs.m_averageEpsilon;
	}
	else {
		return lhs.m_nInternalNodes > rhs.m_nInternalNodes;
	}
	
}

bool TreeFactory::compareBestTreeNorm(const Tree& lhs, const Tree& rhs) {
	if (lhs.m_bestTreeNorm != rhs.m_bestTreeNorm) return lhs.m_bestTreeNorm < rhs.m_bestTreeNorm;
	else if (lhs.m_richness != rhs.m_richness) return lhs.m_richness > rhs.m_richness;
	else if (lhs.m_nInternalNodes != rhs.m_nInternalNodes) return lhs.m_nInternalNodes > rhs.m_nInternalNodes;
	else return lhs.m_nLayers < rhs.m_nLayers;
}


void TreeFactory::sortTrees()
{
	deleteOddTrees(); //after statistic
	//m_treeList.sort(TreeFactory::compareAverageEpsilon);
	m_treeList.sort(TreeFactory::compareBestTreeNorm);
	m_iterShowingTree = m_treeList.begin();
	m_curTreeIndex = 0;
	if (0 != m_treeList.size()) printAllStatistics();
}

void TreeFactory::printTrees(int num)
{
	int i = 0;
	for (std::list<Tree>::iterator iter = m_treeList.begin(); iter != m_treeList.end();iter++) {
		iter->saveCsvTree(i++);
		if (i >= num) break;
	}
}

std::string TreeFactory::printAllStatistics()
{
	if (0 == m_treeList.size()) return std::string("");
	
	std::string filename = buildTreeBasicFilename() + "Statistics.csv";
	
	FILE* pFile = NULL;
	fopen_s(&pFile, filename.c_str(), "w");
	
	//print table head
	int strLen = 0;
	std::string csvHead("BasicFileName,RootGene,"
		                "RootConstraintEps,TriangularConstraintEps,LeastRichness,TreeIndex,"
		                "BestTreeNorm,SumSquareEps,Richness,"
		                "AverageEps,minEps,maxEps,"
		                "Layers,TreeNodes,InternalNodes,"
		                "RemainingNodes,CandidateNodes\n");
	strLen = (int)csvHead.size();
	fwrite(csvHead.c_str(), sizeof(char), strLen, pFile);

	int i = 0;
	float triConstraintEps = theApp.constraint.m_TriConstraintEps / 100.0f;
	float rootConstraintEps = theApp.constraint.m_RootConstraintEps / 100.0f;
	float leastRichness = theApp.constraint.m_LeastRichness / 100.0f;

	for (std::list<Tree>::iterator iter = m_treeList.begin(); iter != m_treeList.end();iter++) {
		std::string outLine = m_basicFilename + "," + theApp.VAFVector.at(iter->m_root).m_gene + ",";
			                
		char str[150];
		sprintf_s(str, 150,"%.4f,%.4f,%.4f,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%d,%d,%d,%d,%d\n", 
			                         rootConstraintEps, triConstraintEps, leastRichness,i++,
			                         iter->m_bestTreeNorm,iter->m_sumSquareEps, iter->m_richness,
			                         iter->m_averageEpsilon, iter->m_minEpsilon, iter->m_maxEpsilon, 
			                         iter->m_nLayers,iter->m_nTreeNodes, iter->m_nInternalNodes, 
			                         iter->m_NumRemainingNodes, (int)iter->m_nodeVector.size());
		outLine.append(std::string(str));
		fwrite(outLine.c_str(), sizeof(char), outLine.length(), pFile);
	}

	fflush(pFile);
	fclose(pFile);

	return filename;

}

std::list<Tree>::iterator TreeFactory::getShowingTreeIterator(const int index)
{
	int i = 0;
	for (std::list<Tree>::iterator iter = m_treeList.begin(); iter != m_treeList.end();iter++) {
		if (index == i++) return iter;
	}
	return m_treeList.end();
}

void TreeFactory::showExploreTreeInformation()
{
	m_totalResutlTree = (unsigned long)m_treeList.size();
	CString statusText;
	statusText.Format(_T("The program has finished %lu trees's computation; The program still is exploring %lu trees."),
		                 m_nFinishedComputationTree, m_totalResutlTree);
	m_pMainWindow->m_editStatusBar.SetWindowText(statusText);
	m_pMainWindow->m_editStatusBar.UpdateWindow();
}

void TreeFactory::setBasicFilename()
{
	int length = (int)theApp.csvFile.m_inputFilename.length();
	m_basicFilename = theApp.csvFile.m_inputFilename.substr(0,length-4);
}

unsigned long TreeFactory::getCurrentTredIndex()
{
	return m_curTreeIndex;
}

unsigned long TreeFactory::getTotalResultTree()
{
	return m_totalResutlTree;
	
}

bool TreeFactory::isCurrentIteratorPointingToEnd()
{
	if (m_iterShowingTree == m_treeList.end()) return true;
	else return false;
}

//use this method after statistics
void TreeFactory::deleteOddTrees()
{
	std::list<Tree>::iterator iter = m_treeList.begin();
	while (iter != m_treeList.end()) {
		int size = iter->m_nTreeNodes;
		if (1 == size || 0 == size) {
			iter = m_treeList.erase(iter);
			continue;
		}
		iter++;
	}
	m_totalResutlTree = (unsigned long) m_treeList.size();
}

std::string TreeFactory::buildTreeBasicFilename(void) {
	char name[50];
	int num = sprintf_s(name, 50, "_E%d%%", theApp.constraint.m_TriConstraintEps);
	name[num] = '\0';
	std::string filename = theApp.treeFactory.m_basicFilename + "_" + theApp.constraint.getTypeName() + std::string(name);
	return filename;
}