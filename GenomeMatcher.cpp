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
	struct reverseCmp
	{
		bool operator()(const int &a, const int &b)
		{
			return a > b;
		}
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
	int mismatch = 0;
		for (int i = 0; i < temp.size(); i++)
		{ 
			mismatch = 0;
			search = "";
			(temp[i].m_genome).extract(temp[i].pos, fragment.size(), search);
			int k;
			for (k = 0; k < search.size(); k++)
			{
				if (fragment[k] != search[k]) {
					if (mismatch == 0 && !exactMatchOnly)
						mismatch++;
					else 
						break;
				}
			}
			DNAMatch add;
			add.genomeName = temp[i].m_genome.name();
			add.position = temp[i].pos;
			if (k >= m_minLength && k >=minimumLength)
			{
				add.length = k;
				matches.push_back(add);
			}
			else if (m_minLength == minimumLength)
			{
				add.length = m_minLength;
				matches.push_back(add);
			}
		}
	map<string, DNAMatch> myMap;
	map<string, DNAMatch>::iterator it;
	for (int i = 0; i < matches.size(); i++)
	{
		it = myMap.find(matches[i].genomeName);
		if (it != myMap.end())
		{
			if(myMap[matches[i].genomeName].length < matches[i].length)
				myMap[matches[i].genomeName] = matches[i];
			else if(myMap[matches[i].genomeName].position > matches[i].position)
				myMap[matches[i].genomeName] = matches[i];
		}
		else
			myMap.insert(pair<string, DNAMatch>(matches[i].genomeName, matches[i]));
	}
	matches.clear();
	for (it = myMap.begin(); it != myMap.end(); it++)
	{
		matches.push_back(it->second);
	}
	if(matches.size()== 0)
		return false;
	return true;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	cout << "called" << endl;
	if(fragmentMatchLength < m_minLength)
		return false;
	map<string, double> matchCount;
	multimap<double, GenomeMatch, reverseCmp> ordered;
	int numSub = query.length() / fragmentMatchLength;
	for (int i = 0; i < numSub; i++)
	{
		string temp;
		query.extract(i*fragmentMatchLength, fragmentMatchLength,temp);
		vector<DNAMatch> matches;
		if (findGenomesWithThisDNA(temp, temp.size(), exactMatchOnly, matches))
		{
			for (int j = 0; j < matches.size(); j++)
			{
				if (matchCount.find(matches[j].genomeName) != matchCount.end())
					matchCount[matches[j].genomeName] ++;
				else
					matchCount.insert(pair<string,double>(matches[j].genomeName, 1));
			}
		}
	}
	for (map<string, double>::iterator it = matchCount.begin(); it != matchCount.end(); it++)
	{
		GenomeMatch a;
		a.genomeName = it->first;
		a.percentMatch = (it->second*100.0)/(double)numSub;
		ordered.insert(pair<double, GenomeMatch>(a.percentMatch, a));
	}
	for (multimap<double, GenomeMatch, reverseCmp>::iterator it = ordered.begin(); it != ordered.end(); it++)
	{
		results.push_back(it->second);
	}
	if (results.size() == 0)
		return false;
	return true;
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
