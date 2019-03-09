#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <iostream>

template<typename ValueType>
class Trie
{
public:
	Trie();
	~Trie();
	void reset();
	void insert(const std::string& key, const ValueType& value);
	std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;

	// C++11 syntax for preventing copying and assignment
	Trie(const Trie&) = delete;
	Trie& operator=(const Trie&) = delete;
private:
	struct Node
	{
		std::vector<ValueType> values;
		char label;
		std::vector<Node*> children;
	}; 
	void cleanup(Node* root);
	void newTrie();
	void insertHelp(const std::string&key, const ValueType&value, Node*cur);
	std::vector<ValueType> findMatch(const std::string&key, Node* cur, bool exactMatchOnly, std::vector<ValueType> &temp, int firstChar) const;
	Node* root;
};

//*IMPLEMENTATIONS OF FUNCTIONS*//
template<typename ValueType>
Trie<ValueType>::Trie()
{
	newTrie();
}
template<typename ValueType>
void Trie<ValueType>::newTrie()
{
	root = new Node;
	root->label = ' ';
}
template<typename ValueType>
Trie<ValueType>::~Trie()
{
	cleanup(root);
	std::cout << "cleaned up " << std::endl;
}
template<typename ValueType>
void Trie<ValueType>::cleanup(Node* root)
{
	if (root == nullptr)
		return;
	for (int i = 0; i < (root->children).size(); i++)
	{
		cleanup(root->children[i]);
	}
	delete root;
}
template<typename ValueType>
void Trie<ValueType>::reset()
{
	cleanup(root);
	newTrie();
}
template<typename ValueType>
void Trie<ValueType>::insert(const std::string&key, const ValueType& value)
{
	insertHelp(key, value, root);
}
template<typename ValueType>
//helper function for insert
void Trie<ValueType>::insertHelp(const std::string&key, const ValueType& value, Node* cur)
{
	if (key.size() == 0)
	{
		cur->values.push_back(value);
		return;
	}
	for (int i = 0; i < (cur->children).size(); i++)
	{
		if (toupper(key[0]) == cur->children[i]->label)
			return insertHelp(key.substr(1), value, cur->children[i]);
	}
	Node* newChild = new Node;
	newChild->label = toupper(key[0]);
	cur->children.push_back(newChild);
	insertHelp(key.substr(1), value, cur->children[(cur->children).size() - 1]);
}
template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly)const
{
	std::vector<ValueType> temp;
	int charNum = 0;
	return findMatch(key, root, exactMatchOnly, temp, charNum);
}
template<typename ValueType>
//helper function for find
std::vector<ValueType> Trie<ValueType>::findMatch(const std::string&key, Node* cur, bool exactMatchOnly, std::vector<ValueType> &temp, int charNum) const
{
	if (key.size() == 0)
	{
		for (int i = 0; i < cur->values.size(); i++)
			temp.push_back(cur->values[i]);			//match found so values in teh vector are pushed onto the result vector
	}
	charNum++;
	for (int i = 0; i < (cur->children).size(); i++)
	{
		if (toupper(key[0]) == cur->children[i]->label)
		{
			findMatch(key.substr(1), cur->children[i], exactMatchOnly, temp, charNum);
		}
		else if (!exactMatchOnly && charNum != 1)											//checking for SNiPs, means at this point there is one mismatch, 																	
		{																					//checks to make sure mismatch is not first character
			findMatch(key.substr(1), cur->children[i], true, temp, charNum);		//calls findMatch to see if there is an exact match after the one mismatch
		}
	}
	return temp;
}
#endif // TRIE_INCLUDED
