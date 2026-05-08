#pragma once
#include <cstdint>


class Node {
public:

	Node* left; // Pointer to left child.
	Node* right; // Pointer to right child.
	uint8_t symbol; // Node's symbol.
	uint64_t frequency; // Frequency of symbol.


	Node(uint8_t symbol, uint64_t frequency);
	Node(Node* l, Node* r);

};