#pragma once
#include <cstdint>
#define ALPHABET 256 // ASCII + Extended ASCII.
#define MAX_CODE_SIZE (ALPHABET / 8) // Bytes for a maximum, 256-bit code.

class Code {
public:

	uint32_t top;
	uint8_t bits[MAX_CODE_SIZE];

	Code();
	bool isEmpty() const;
	bool isFull() const;
	uint32_t size() const;

	bool push(bool bit);
	bool pop(bool& bit);

};
