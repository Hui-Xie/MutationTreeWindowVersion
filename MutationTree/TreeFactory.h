#pragma once
#include <list>
#include <vector>
#include "VAF_CSVTree_struct.h"
#include "MutationTreeDlg.h"

#define NumBestTrees 300

class Node{
public:
	Node();
	~Node();

	int m_index;
	int m_layer;
	std::string m_treeID;
	int m_parent; //vector index in the  m_nodeVector, also the index in VAFVector
	int m_lchild; //vector index in the  m_nodeVector, also the index in VAFVector
	int m_rchild; //vector index in the  m_nodeVector, also the index in VAFVector
	float m_altFreq;//Only keep 2 decimals
	Node& operator =(const Node& other);
	
};

class Tree {
public:
	Tree();
	~Tree();

	int m_root;
	int m_nLayers;//total layers
	int m_nTreeNodes; //total nodes
	int m_nInternalNodes;
	int m_NumRemainingNodes;
	float m_richness; //m_richness = m_nTreeNodes/ncandidateNodes

	float m_sumSquareEps;  //the sum of square of triangulare epsilon 
	float m_averageEpsilon;//square root of the average of square deviation
	float m_minEpsilon;
	float m_maxEpsilon;

	float m_bestTreeNorm; //m_bestTreeNorm = m_sumSquareEps/(m_richness*m_richness), the smaller is better
	
	std::vector<Node> m_nodeVector;  //only nodes with parent unqual -1 are inside the binary tree
	
	Tree& operator =(Tree& other);
	std::list<int> m_heap;

	void initialize();
	void initializeStatisticsProperty();
	int getRootFromNodeVector();
	

	void extendTreeBFS();
	float getTriConstraintEps();
	float getLeastRichness();
	bool spareNodesExceedLeastRichness(int const index1, int const index2);
	void bfsExploreTree(std::list<Tree>* pTreeList);
	void initializeRootNode(int root);
	void addChildrenNodes(const int parent, const int lc, const int rc);

	void extendTreeDFS(int parent); // don't use remainingNode version 

	void buildTree(); //from root to build Tree

	Node& leftChild(int parent);
	Node& rightChild(int parent);
	Node& parent(int child);
	Node& getNode(int index);
	bool isLeaf(int index);
	float getTriangularEpsilon(int index);

	void statistic();

	void buildNodeRelationFromID();

	std::string saveCsvTree(const unsigned long fileIndex);
	std::string buildTreeBasicFilename(unsigned long fileIndex);
	

	//void copyBinTreeTo(TreeFactory& binTree);

private:
	std::string contructTreeID(int Parent, int LorR); //LorR: 1: Left Child; 2: Right Child;
	void putAllCandidatesinBinTree();

};

class TreeFactory
{
public:
	std::list<Tree> m_treeList;
	std::list<Tree>::iterator  m_iterShowingTree;
	unsigned long m_curTreeIndex;

	CMutationTreeDlg* m_pMainWindow;
	std::string m_basicFilename;
	unsigned long m_totalResutlTree;
	
private:
	std::list<Tree>::iterator m_curComputingTreeIter;
	bool are2TreesSameProperty(const Tree & first, const Tree & second);
	bool are2TreesSameNodes(const Tree & first, const Tree & second);
	unsigned long m_nFinishedComputationTree;

	std::list<Tree>::iterator getWorstInBestTrees();
	bool updateTheWorstInBestTrees();
	bool isLhsBetterTree(const Tree & lhs, const Tree & rhs);
	

public:
	TreeFactory();
	~TreeFactory();

	void initialize();

	void initializeVAFVector(std::vector<VAF>& vafVector);
	void initializeVAFVector(std::vector<Node>& nodeVector);

	static bool compareAverageEpsilon(const Tree& lhs, const Tree& rhs);
	static bool compareBestTreeNorm(const Tree& lhs, const Tree& rhs);

	//delete isomorphic or odd tree
	bool deleteIsomorphicOrOddTree();
	int buildBFSTrees();
	void initialRootAndHeapOfTreeList();
	void sortTrees();
	void printTrees(int num);
	std::string printAllStatistics();
	std::list<Tree>::iterator getShowingTreeIterator(const int index);
	void showExploreTreeInformation();

	void setBasicFilename();

	unsigned long getCurrentTredIndex();
	unsigned long getTotalResultTree();
	bool isCurrentIteratorPointingToEnd();

	void deleteOddTrees();

	std::string buildTreeBasicFilename(void);


	

};

