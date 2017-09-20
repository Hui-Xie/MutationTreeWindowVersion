#pragma once
#include <string>


class Constraints
{
public:
	int m_RootConstraintEps;
	int m_TriConstraintEps;
	int m_LeastRichness;
	int m_tempRootConstraintEps; //uncommitted Root Constraint epsilon
	int m_tempTriConstraintEps;//uncommitted Triangular Constraint epsilon
	int m_tempLeastRichness;
	
	//1 = DFS; 2 = BFS; 3=
	int m_type; 

public:
	Constraints();
	~Constraints();

	std::string getTypeName();
};

