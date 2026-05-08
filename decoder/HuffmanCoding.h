#pragma once
#include "Node.h"
#include "Code.h"
#include "defines.h"
#include <fstream>
#include <vector>


class HuffmanCoding {
public:

	Node* constructTree(const uint64_t histogram[ALPHABET]);
	void buildCodeTable(Node* root, Code& currentCode, Code codeTable[ALPHABET]);
	Node* reconstructTree(const std::vector<uint8_t>& treeDump);

};