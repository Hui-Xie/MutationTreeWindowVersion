#pragma once
#include "afxstr.h"
#include <string>

struct VAF
{
	std::string m_gene;
	std::string m_chromosome;
	std::string m_position;
	char	m_reference;
	char	m_alternative;
	int 	m_depth;
	int 	m_refRead;
	int 	m_altRead;
	float   m_refFreq; 
	float 	m_altFreq; 
    float  	m_2pq;    // m_refFreq* m_altFreq

};

