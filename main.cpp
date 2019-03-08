#include <iostream>
#include<fstream>
#include <vector>
#include <string>
#include "Trie.h"
#include "provided.h"

using namespace std;

int main()
{
	string filename = "C:/Users/marie/Desktop/CS32/Gee-nomics/data/tester.txt";
	ifstream strm(filename);
	if (!strm)
	{
		cout << "cannot open" << filename << endl;
		return 0;
	}
	vector<Genome> vg;
	bool success = Genome::load(strm, vg);
	if (success)
	{
		cout << "loaded" << vg.size() << "genomes successfully: " << endl;
		for (int k = 0; k !=vg.size(); k++)
			cout << vg[k].name() << endl;
	}
	else
		cout << "Error loading genome data" << endl;
	string answer;
	string an;
	/*cout << "my answer: ";
	getline(cin, answer);
	cout << "The size was: " << answer.size();
	Trie<int> a;
	a.insert("CDA", 14);
	a.insert("CDA", 12);
	a.insert("qDj", 7);
	a.insert("CAA", 11);
	a.insert("CDD", 100);
	a.insert("aDa", 3);
	vector<int> temp = a.find("CDA", false);
	for (int i = 0; i < temp.size(); i++)
		cout << temp[i] << endl;
	cout << "done" << endl;
	*/

//asked if my recursion is ok for trie
	//ask if bases over 80 return false in genome cpp
}