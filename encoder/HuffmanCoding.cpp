#include "HuffmanCoding.h"
#include <queue>
#include <stack>
#include <iostream>


struct Compare {
	bool operator()(Node* a, Node* b) {
		return a->frequency > b->frequency;
	}
};

Node* HuffmanCoding::constructTree(const uint64_t histogram[ALPHABET]) {

	std::priority_queue<Node*, std::vector<Node*>, Compare> pq;

	for (int i = 0; i < ALPHABET; ++i) {
		if (histogram[i] > 0) {
			pq.push(new Node(i, histogram[i]));
		}
	}

	while (pq.size() > 1) {
		Node* left = pq.top(); pq.pop();
		Node* right = pq.top(); pq.pop();
		Node* parent = new Node(left, right);
		pq.push(parent);

	}

	return pq.empty() ? nullptr : pq.top();
}


void HuffmanCoding::buildCodeTable(Node* root, Code& currentCode, Code codeTable[ALPHABET]) {
	if (!root) return;

	if (!root->left && !root->right) { 
		codeTable[root->symbol] = currentCode;
		return; 
	}

	if (root->left) {
		currentCode.push(0);
		buildCodeTable(root->left, currentCode, codeTable);
		bool bit;
		currentCode.pop(bit);
	}

	if (root->right) {
		currentCode.push(1);
		buildCodeTable(root->right, currentCode, codeTable);
		bool bit;
		currentCode.pop(bit);
	}

}


Node* HuffmanCoding::reconstructTree(const std::vector<uint8_t>& treeDump) {

	std::stack<Node*> nodeStack;

	for (int i = 0; i < treeDump.size(); ++i) {
		if (treeDump[i] == 'L') {  
			Node* leaf = new Node(treeDump[++i], 0);
			nodeStack.push(leaf);
		}
		else if (treeDump[i] == 'I') {  
			Node* right = nodeStack.top(); nodeStack.pop();
			Node* left = nodeStack.top(); nodeStack.pop();
			Node* parent = new Node(left, right);
			nodeStack.push(parent);
		}
	}

	return nodeStack.top();
}



