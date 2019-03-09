#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
using namespace std;
//use algorithsm
//genomes would be two separate if found at two separate positions
//can two genomes have the same name -> if yes just check if the DNA sequence length is the same in the find
class GenomeMatcherImpl
{
public:
	GenomeMatcherImpl(int minSearchLength);
	void addGenome(const Genome& genome);
	int minimumSearchLength() const;
	bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
	bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
	int m_minLength;
	struct Values
	{
	public:
		Values(Genome my_genome, int position);
		bool operator< (const Values&b);
		Genome* getGenome();
	//private:
		Genome m_genome;
		int pos;
	};
	vector<Genome> genomes;
	Trie<Values> myValues;
};
GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
	m_minLength = minSearchLength;
}
bool GenomeMatcherImpl::Values::operator< (const Values&b)
{
	return m_genome.name() < b.m_genome.name();		
}
GenomeMatcherImpl::Values::Values(Genome my_genome, int position)
	:m_genome(my_genome), pos(position) {}
Genome* GenomeMatcherImpl::Values::getGenome()
{
	return &m_genome;
}
void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	genomes.push_back(genome);
	
	for (int i = 0; i < genome.length(); i ++)
	{
		string fragment;
		if (genome.extract(i, m_minLength, fragment))
		{
			Values temp(genome, i);
			cout << fragment << endl;
			myValues.insert(fragment, temp);
		}
	}
	vector<Values> temp = myValues.find("ACC", true);		//just testing if function works
	cout << (temp[0].getGenome())->name()<< endl;
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_minLength;  // This compiles, but may not be correct
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	if (fragment.size() < minimumLength || minimumLength < m_minLength)
		return false;			//ask about page 23 number 1 and 2
	matches.clear();
	string search = fragment.substr(0, m_minLength);
	vector<Values> temp = myValues.find(search, exactMatchOnly);
	//vector<DNAMatch> tempMatches;
	if (fragment.size() > m_minLength)
	{
		for (int i = 0; i < temp.size(); i++)
		{
			int mismatch = 0;
			search = "";
			(temp[i].m_genome).extract(temp[i].pos, fragment.size(), search);
			int k;
			for (k = 0; k < fragment.size(); k++)
			{
				if (fragment[k] != search[k] && mismatch == 0)
					mismatch++;
				else if (fragment[k] != search[k] && mismatch > 0)
				{
					break;
				}
			}
			DNAMatch add;
			add.genomeName = temp[i].m_genome.name();
			add.position = temp[i].pos;
			if (k > m_minLength)
				add.length = k;
			else
				add.length = m_minLength;
			matches.push_back(add);
		}
	}
	for (int i = 0; i < matches.size(); i++)
	{
		for (vector<DNAMatch>::iterator it = matches.begin()+i+1; it != matches.end(); it++)
		{
			if (matches[i].genomeName == it->genomeName)
			{
				if (matches[i].length > it->length)
				{
					it = matches.erase(it);
					it--;
				}
				else if (matches[i].position < it->position)
				{
					it = matches.erase(it);
					it--;
				}
			}
		}		 
	}
	for (int i = 0; i < matches.size(); i++)
	{
		for (vector<DNAMatch>::iterator it = matches.begin() + i + 1; it != matches.end(); it++)
		{
			if (matches[i].genomeName == it->genomeName)
			{
				if (matches[i].length > it->length)
				{
					it = matches.erase(it);
					it--;
				}
				else if (matches[i].position < it->position)
				{
					it = matches.erase(it);
					it--;
				}
			}
		}
	}
	if(matches.size()== 0)
		return false;
	return true;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	return false;  // This compiles, but may not be correct
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
	m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
	delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
	m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
	return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
