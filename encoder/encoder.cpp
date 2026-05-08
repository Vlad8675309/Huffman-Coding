#include "HuffmanCoding.h"
#include "defines.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>


// Function to compute file size
uint64_t getFileSize(const char* filename) {
    try {
        return std::filesystem::file_size(filename);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error getting file size: " << e.what() << std::endl;
        return 0;
    }
}

// Function to compute histogram of file contents
void computeHistogram(const char* filename, uint64_t histogram[ALPHABET]) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    memset(histogram, 0, ALPHABET * sizeof(uint64_t));

    char ch;
    while (file.get(ch)) {
        histogram[static_cast<uint8_t>(ch)]++;
    }

    histogram[0]++;
    histogram[255]++;
}

// Function to write the header to output file
void writeHeader(std::ofstream& out, uint16_t tree_size, uint64_t file_size) {
    Header header;
    header.signature = MAGIC;
    header.tree_size = tree_size;
    header.file_size = file_size;

    out.write(reinterpret_cast<const char*>(&header), sizeof(Header));
}

// Function to perform post-order traversal and write tree to file
void writeTree(Node* root, std::ofstream& out, uint16_t& tree_size) {
    if (!root) return;

    if (!root->left && !root->right) {
        // Leaf node
        out.put('L');
        out.put(root->symbol);
        tree_size += 2;
    }
    else {
        // Internal node
        writeTree(root->left, out, tree_size);
        writeTree(root->right, out, tree_size);
        out.put('I');
        tree_size += 1;
    }
}

// Function to write compressed data to file

void writeCompressedData(const char* inputFile, std::ofstream& out, Code codeTable[ALPHABET]) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        exit(1);
    }

    uint8_t block[BLOCK] = { 0 };
    uint32_t blockPos = 0; // Position inside block

    uint8_t buffer = 0;    
    int bitsFilled = 0;     // Bits currently filled in buffer (1 byte)

    char ch;
    while (in.get(ch)) {
        Code& code = codeTable[static_cast<uint8_t>(ch)];
        for (uint32_t i = 0; i < code.size(); ++i) {
            buffer = (buffer << 1) | (code.bits[i] & 1);
            bitsFilled++;

            if (bitsFilled == 8) {
                block[blockPos++] = buffer;
                buffer = 0;
                bitsFilled = 0;

                if (blockPos == BLOCK) {
                    out.write(reinterpret_cast<char*>(block), BLOCK);
                    blockPos = 0;
                }
            }
        }
    }

    if (bitsFilled > 0) {
        buffer <<= (8 - bitsFilled);
        block[blockPos++] = buffer;
    }

    if (blockPos > 0) {
        out.write(reinterpret_cast<char*>(block), blockPos);
    }

    in.close();
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " -i <input> -o <output>" << std::endl;
        return 1;
    }

    const char* inputFile = nullptr;
    const char* outputFile = nullptr;
    bool printStats = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) inputFile = argv[++i];
        else if (arg == "-o" && i + 1 < argc) outputFile = argv[++i];
        else if (arg == "-s") printStats = true;
        else if (arg == "-h") { 
            std::cout << "Huffman Coding\n"
                << "Usage: " << argv[0] << " [options]\n"
                << "Options:\n"
                << "  -h               Show this help message\n"
                << "  -i <file name>   Input file to encode\n"
                << "  -o <file name>   Output file\n"
                << "  -s               Print compression statistics\n";
            exit(0); }
        else { std::cerr << "Unknown option: " << arg << std::endl; exit(1); }
    }

    // Compute histogram
    uint64_t histogram[ALPHABET] = { 0 };
    computeHistogram(inputFile, histogram);
    uint64_t originalSize = getFileSize(inputFile);

    // Construct Huffman tree
    HuffmanCoding huffman;
    Node* root = huffman.constructTree(histogram);

    // Build code table
    Code codeTable[ALPHABET];
    Code currentCode;
    huffman.buildCodeTable(root, currentCode, codeTable);

    // Write to output file
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return 1;
    }

    // First write a temporary empty header 
    Header tempHeader = { 0 };
    out.write(reinterpret_cast<const char*>(&tempHeader), sizeof(Header));

    // Write the tree and calculate its size
    uint16_t tree_size = 0;
    writeTree(root, out, tree_size);

    // Now write the compressed data
    writeCompressedData(inputFile, out, codeTable);

    // Get the final file size
    out.close();
    uint64_t compressedSize = getFileSize(outputFile);

    // Now reopen and update the header with the final values
    std::fstream outUpdate(outputFile, std::ios::in | std::ios::out | std::ios::binary);
    Header header;
    header.signature = MAGIC;
    header.tree_size = tree_size;
    header.file_size = originalSize;
    outUpdate.seekp(0);
    outUpdate.write(reinterpret_cast<const char*>(&header), sizeof(Header));
    outUpdate.close();

    // Print statistics
    if (printStats) {
        std::cerr << "Original size: " << originalSize << " bytes\n"
            << "Compressed size: " << compressedSize << " bytes\n"
            << "Space saving: "
            << 100.0 * (1.0 - (double)compressedSize / originalSize)
            << "%\n";
    }

    return 0;
}