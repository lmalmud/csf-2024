#include "cache.h"
#include <cmath>

Cache::Cache(int numSets, int numBlocks, int bytesPerBlock, bool writeAllocate, bool writeThrough, bool lru) {
	this->numSets = numSets;
	this->numBlocks = numBlocks;
	this->bytesPerBlock = bytesPerBlock;
	this->writeAllocate = writeAllocate;
	this->writeThrough = writeThrough;
	this->lru = lru;

	this->numOffsetBits = (int) std::log2(this->bytesPerBlock); // bytesPerBlock = block size
	this->numIndexBits = (int) std::log2(this->numBlocks); // numBlocks = slots
	this->numTagBits = 32 - this->numOffsetBits - this->numIndexBits;

	// create sets
	for (int i = 0; i < this->numSets; ++i) {
		sets.push_back(Set(this->numBlocks, this->bytesPerBlock)); // calls Set constructor
	}

	/*
	std::cout << "testing...." << std::endl;
	std::cout << "getIndex(0x12345678): " << getIndex(0x12345678) << std::endl;
	std::cout << "getOffset(0x123456): " << getOffset(0x12345678) << std::endl;
	std::cout << "getTag(0x12345678): " << getTag(0x12345678) << std::endl;
	std::cout << "this->numOffsetBits: " << this->numOffsetBits << std::endl;
	std::cout << "this->numIndexBits: " << this->numIndexBits << std::endl;
	std::cout << "this->numTagBits: " << this->numTagBits << std::endl;
	*/

}

/* Returns the part of the 32-bit address that corresponds
   to the index. It is stored in the middle of the address. */
uint32_t Cache::getIndex(int address) {
	//std::cout << "(address >> this->numTagBits): " << (address >> this->numTagBits) << std::endl;
	//std::cout << "((1 << this->numIndexBits) - 1): " << ((1 << this->numIndexBits) - 1) << std::endl;
	return (address >> this->numTagBits) & ((1 << this->numIndexBits) - 1);
}

uint32_t Cache::getOffset(int address) {
	return address >> (this->numTagBits + this->numIndexBits);
}

/* Returns the part of the 32-bit address that corresponds
   to the tag. It is stored at the end of the address. */
uint32_t Cache::getTag(int address) {
	return address & ((1 << numTagBits) - 1);
}


void Cache::load(std::string addressString){
	uint32_t address;
	address = stoi(addressString, nullptr, 16);
	int index = getIndex(address);
	int tag = getTag(address);
}