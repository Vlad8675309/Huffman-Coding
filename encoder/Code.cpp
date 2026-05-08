#include "Code.h"
#include <cstdint>
#include <string>

Code::Code() {
	this->top = 0;
	memset(this->bits, 0, sizeof(bits));
}

bool Code::isEmpty() const {
	return top == 0;
}

bool Code::isFull() const {
	return top >= MAX_CODE_SIZE;
}

uint32_t Code::size() const {
	return top;
}

bool Code::push(bool bit) {
	if (isFull()) {
		return false;
	}
	
	bits[top++] = bit;
	return true;
	
}

bool Code::pop(bool &bit) {
	if (isEmpty()) {
		return false;
	}
	bit = bits[--top];
	return true;
}
