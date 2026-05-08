#include "Node.h"
#include <cstdint>


Node::Node(uint8_t symbol, uint64_t frequency) {
	this->symbol = symbol;
	this->frequency = frequency;
	this->left = nullptr;
	this->right = nullptr;

}

Node::Node(Node* l, Node* r) {
	this->left = l;
	this->right = r;
	this->symbol = '$';
	this->frequency = l->frequency + r->frequency;

}