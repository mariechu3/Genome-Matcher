#include <iostream>
#include <string>
#include "Trie.h"

using namespace std;
int main()
{
	Trie<int> a;
	a.insert("CDA", 14);
	a.insert("CDA", 12);
	vector<int> temp = a.find("CDA", true);
	for (int i = 0; i < temp.size(); i++)
		cout << temp[i] << endl;
	cout << "done" << endl;


}