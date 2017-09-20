#include "stdafx.h"
#include "Constraints.h"
#include <string>


Constraints::Constraints()
{
	m_RootConstraintEps = 2;
	m_TriConstraintEps = 2;
	m_LeastRichness = 90;
	m_tempTriConstraintEps = 2 + m_TriConstraintEps;
	m_tempRootConstraintEps = 2 + m_RootConstraintEps;
	m_tempLeastRichness = m_LeastRichness - 2;
	m_type = 2;//default BFS
}


Constraints::~Constraints()
{
}



//1 = DFS; 2 = BFS; 3=
std::string Constraints::getTypeName(){
	switch (m_type){
	case 1:
		return std::string("DFS");
	case 2:
		return std::string("BFS");
	default:
		return std::string("");
	}
	return std::string("");

}
