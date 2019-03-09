#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
	GenomeImpl(const string& nm, const string& sequence);
	static bool load(istream& genomeSource, vector<Genome>& genomes);
	int length() const;
	string name() const;
	bool extract(int position, int length, string& fragment) const;
private:
	string m_name;
	string m_DNA;
};
GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
	m_name = nm;
	m_DNA = sequence;
}
bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	string temp, tempBase, tempName, bases = "";
	bool valid = false;
	bool lastLine = false;
	genomes.clear(); // getting rid of whatever was originally in genomes
	while (getline(genomeSource, temp))
	{
		/*if (temp.size() == 0)
		{
			lastLine = true;
			continue;
		}*/
		if (temp.size() == 0)
			return false;
		if (temp.at(0) == '>')
		{
			if (bases.size() != 0 && temp.size() != 0)
			{
				genomes.push_back(Genome(tempName, bases));
				bases = "";
				tempName = "";
			}
			if (temp.size() == 1)	//if there was no name after the <
				return false;
			tempName = temp.substr(1);
			valid = false;
			continue;
		}
		else
		{
			//lastLine = false;
			tempBase = "";
			tempBase = temp;
			//int tempBaseSize = tempBase.size();
			for (int i = 0; i < tempBase.size(); i++)
			{
				if (toupper(tempBase[i]) != 'A' && toupper(tempBase[i]) != 'C' && toupper(tempBase[i]) != 'T' && toupper(tempBase[i]) != 'G' && toupper(tempBase[i]) != 'N')	//if it is an invalid base char
					return false;
				bases += tempBase[i];
				valid = true;		//base exists after a name
			}
			//genomeSource.ignore(1, '\n');
		}
		//temp = "";
		
		cout << temp <<endl;
	}
	/*if (lastLine == false)
		return false;
		*/
	if (bases.size() != 0 && tempName.size() != 0)		//this will add the last genome onto the vector
	{
		genomes.push_back(Genome(tempName, bases));
		bases = "";
		tempName = "";
	}
	if (valid == false)		//if there were no base sequence after a name at the end
		return false;
	return true;
}
/*
bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	char c;
	string tempBase,tempName, bases = "";
	bool valid = false;
	genomes.clear(); // getting rid of whatever was originally in genomes
	while (genomeSource.get(c))
	{
		if (c == '>')
		{
			if (bases.size() != 0 && tempName.size() != 0)
			{
				genomes.push_back(Genome(tempName, bases));
				bases = "";
				tempName = "";
			}
			getline(genomeSource, tempName);
			if (tempName.size() == 1)	//if there was no name after the <
				return false;
			valid = false;
			continue;
		}
		else
		{
			tempBase = "";
			getline(genomeSource, tempBase);
			tempBase = c + tempBase;
			int tempBaseSize = tempBase.size();
			for (int i = 0; i < tempBase.size(); i++)
			{
				if (toupper(tempBase[i]) != 'A' && toupper(tempBase[i]) != 'C' && toupper(tempBase[i]) != 'T' && toupper(tempBase[i]) != 'G' && toupper(tempBase[i]) != 'N')	//if it is an invalid base char
					return false;
				bases += tempBase[i];
				valid = true;		//base exists after a name
			}
			//genomeSource.ignore(1, '\n');
		}	
	}
	if (bases.size() != 0 && tempName.size() != 0)		//this will add the last genome onto the vector
	{
		genomes.push_back(Genome(tempName, bases));
		bases = "";
		tempName = "";
	}
	if (valid == false)		//if there were no base sequence after a name at the end
		return false;
	return true;
}
*/
/*bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes)
{
	char c;
	string tempName, bases = "";
	bool nameFirst = false;
	genomes.clear(); // getting rid of whatever was originally in genomes
	while (genomeSource.get(c))
	{
		if (c == '>')
		{
			if (bases.size() != 0)		//if there are bases, when c == > that means it is the end of the base sequence
			{
				genomes.push_back(Genome(tempName, bases));	//adding a new genome with correct name and dna sequence
				tempName = "";	//reset values of name and bases sequence
				bases = "";
				nameFirst = false;	//sets the name first to false
			}
			getline(genomeSource, tempName);
			if (tempName.size() == 1)	//if there was no name after the <
				return false;
			nameFirst = true;
			continue;
		}
		if (toupper(c) != 'A' && toupper(c) != 'C' && toupper(c) != 'T' && toupper(c) != 'G' && toupper(c) != 'N')	//if it is an invalid base char
			return false;
		else
			if (nameFirst && (toupper(c) == 'A' || toupper(c) == 'C' || toupper(c) == 'T' || toupper(c) == 'G' || toupper(c) == 'N'))
			bases += toupper(c);
	}
	//if (nameFirst == true)		//if there were no base sequence after the names at the end
		//return false;
	return true;
}
*/

int GenomeImpl::length() const
{
	return m_DNA.size();  
}

string GenomeImpl::name() const
{
	return m_name;  
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
	if (position < 0 || length < 0)	//what would length 0 count as
		return false;
	if (position + length > m_DNA.length())
		return false;
	fragment = m_DNA.substr(position, length); 
	return true;
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
	m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
	delete m_impl;
}

Genome::Genome(const Genome& other)
{
	m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
	GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
	delete m_impl;
	m_impl = newImpl;
	return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes)
{
	return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
	return m_impl->length();
}

string Genome::name() const
{
	return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
	return m_impl->extract(position, length, fragment);
}
