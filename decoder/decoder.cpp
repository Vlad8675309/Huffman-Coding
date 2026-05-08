#include "HuffmanCoding.h"
#include "defines.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>


// Function to read the header from input file
bool readHeader(std::ifstream& in, Header& header) {
    in.read(reinterpret_cast<char*>(&header), sizeof(Header));
    return in.good();
}

// Function to read the tree dump from input file
std::vector<uint8_t> readTreeDump(std::ifstream& in, uint16_t tree_size) {
    std::vector<uint8_t> treeDump(tree_size);
    in.read(reinterpret_cast<char*>(treeDump.data()), tree_size);
    return treeDump;
}

// Function to decode the compressed data
void decodeData(std::ifstream& in, std::ofstream& out, Node* root, uint64_t originalSize) {
    if (!root) {
        std::cerr << "Huffman tree root is null." << std::endl;
        exit(1);
    }

    uint8_t block[BLOCK] = { 0 };
    uint32_t blockPos = 0;
    uint32_t blockSize = 0; // Bytes loaded into block

    Node* current = root;
    uint8_t byte = 0;
    int bitsLeft = 0;
    uint64_t symbolsWritten = 0;

    while (symbolsWritten < originalSize) {
        if (bitsLeft == 0) {
            if (blockPos >= blockSize) {
                // Load next block
                in.read(reinterpret_cast<char*>(block), BLOCK);
                blockSize = static_cast<uint32_t>(in.gcount());
                blockPos = 0;
            }
            byte = block[blockPos++];
            bitsLeft = 8;
        }

        bool bit = (byte >> 7) & 1;
        byte <<= 1;
        bitsLeft--;

        current = bit ? current->right : current->left;

        if (!current->left && !current->right) {
            out.put(static_cast<char>(current->symbol));
            current = root;
            symbolsWritten++;
        }
    }
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
            exit(0);
        }
        else { std::cerr << "Unknown option: " << arg << std::endl; exit(1); }
    }

    // Open input file
    std::ifstream in(inputFile, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return 1;
    }

    // Read and verify header
    Header header;
    if (!readHeader(in, header)) {
        std::cerr << "Error reading header" << std::endl;
        return 1;
    }

    if (header.signature != MAGIC) {
        std::cerr << "Invalid file format (wrong magic number)" << std::endl;
        return 1;
    }

    // Get compressed size for statistics
    in.seekg(0, std::ios::end);
    uint64_t compressedSize = in.tellg();
    in.seekg(sizeof(Header), std::ios::beg);

    // Read tree dump and reconstruct Huffman tree
    std::vector<uint8_t> treeDump = readTreeDump(in, header.tree_size);
    HuffmanCoding huffman;
    Node* root = huffman.reconstructTree(treeDump);

    // Open output file and decode data
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return 1;
    }

    // Now write the decompressed data
    decodeData(in, out, root, header.file_size);

    // Close files
    in.close();
    out.close();

    // Print statistics
    if (printStats) {
        std::cerr << "Compressed size: " << compressedSize << " bytes\n"
            << "Decompressed size: " << header.file_size << " bytes\n"
            << "Space saving: "
            << 100.0 * (1.0 - (double)compressedSize / header.file_size)
            << "%\n";
    }

    return 0;
}