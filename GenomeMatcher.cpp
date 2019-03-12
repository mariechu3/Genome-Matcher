#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
using namespace std;

class GenomeMatcherImpl
{
public:
	GenomeMatcherImpl(int minSearchLength);
	~GenomeMatcherImpl();
	void addGenome(const Genome& genome);
	int minimumSearchLength() const;
	bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
	bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
	int m_minLength;
	struct Values
	{
	public:
		Values(const Genome* my_genome, int position);
		bool operator< (const Values&b);
		const Genome *m_genome;
		int pos;
	};
	struct reverseCmp
	{
		bool operator()(const int &a, const int &b) const		//custom comparision to put items in multimap in descending order
		{
			return a > b;
		}
	};
	vector<const Genome*> genomes;
	Trie<Values> myValues;
};
GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
	m_minLength = minSearchLength;
}
bool GenomeMatcherImpl::Values::operator< (const Values&b)
{
	return m_genome->name() < b.m_genome->name();		
}
GenomeMatcherImpl::Values::Values(const Genome* my_genome, int position)
	:m_genome(my_genome), pos(position) {}
GenomeMatcherImpl::~GenomeMatcherImpl()
{
	for (int i = 0; i < genomes.size(); i++)
		delete genomes[i];
}
void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	Genome* copy = new Genome(genome);		//dynamically creates a genome object with passed in genome 
	genomes.push_back(copy);
	for (int i = 0; i < genome.length(); i ++)
	{
		string fragment;
		if (genome.extract(i, m_minLength, fragment))
		{
			Values temp(copy, i);
			myValues.insert(fragment, temp);
		}
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_minLength; 
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	if (fragment.size() < minimumLength || minimumLength < m_minLength)
		return false;			
	matches.clear();
	string search = fragment.substr(0, m_minLength);
	vector<Values> temp = myValues.find(search, exactMatchOnly);		//find the first m_minLength items in the fragment using trie
	int mismatch = 0;
	for (int i = 0; i < temp.size(); i++)
	{ 
		mismatch = 0;
		search = "";
		temp[i].m_genome->extract(temp[i].pos, fragment.length(), search);
		int k;
		for (k = 0; k < search.size(); k++)
		{
			if (fragment[k] != search[k]) {
				if (mismatch == 0 && !exactMatchOnly)	//if mismatch allowed, check if there is one or less mismatch
					mismatch++;
				else 
					break;
			}
		}
		DNAMatch add;
		add.genomeName = temp[i].m_genome->name();
		add.position = temp[i].pos;
		if (k >= m_minLength && k >= minimumLength)	//if the fragment matched (exact or snip) longer than the depth of trie
		{
			add.length = k;
			matches.push_back(add);		//add onto the match vector
		}
		else if (m_minLength == minimumLength)		//else add what was found in the trie onto the match vector
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
		if (it != myMap.end())					//if the genome is already in the map
		{
			if(myMap[matches[i].genomeName].length < matches[i].length)	//check if that dna match length is smaller than the other genome's length
				myMap[matches[i].genomeName] = matches[i];
			else if(myMap[matches[i].genomeName].position > matches[i].position)	//check if that dna match position is greater than the other genome's position
				myMap[matches[i].genomeName] = matches[i];
		}
		else
			myMap.insert(pair<string, DNAMatch>(matches[i].genomeName, matches[i]));	//if not found insert the dna match into the map
	}
	matches.clear();
	for (it = myMap.begin(); it != myMap.end(); it++)
	{
		matches.push_back(it->second);	//push all the final dna matches onto the match vector
	}
	if(matches.size()== 0)
		return false;
	std::cout << "done" << endl;
	return true;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
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
				if (matchCount.find(matches[j].genomeName) != matchCount.end())	//if it is found in the map
					matchCount[matches[j].genomeName] ++;	//increase the count of that genome match by one
				else
					matchCount.insert(pair<string,double>(matches[j].genomeName, 1));	//else insert the genome into the map
			}
		}
	}
	for (map<string, double>::iterator it = matchCount.begin(); it != matchCount.end(); it++)
	{
		GenomeMatch a;
		a.genomeName = it->first;
		a.percentMatch = (it->second*100.0)/(double)numSub;
		if(a.percentMatch > matchPercentThreshold)			//if the percent match is larger than the match percent threshold
			ordered.insert(pair<double, GenomeMatch>(a.percentMatch, a));
	}
	for (multimap<double, GenomeMatch, reverseCmp>::iterator it = ordered.begin(); it != ordered.end(); it++)	//this will order it in descending count order but since it was previously in a map, it will also be alphabetically ordered for the names (in ties)
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
