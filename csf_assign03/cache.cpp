#include "cache.h"
#include <cmath>
#include <string>

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
	
	this->cacheClock = 0;

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
   to the index. It is stored in the middle of the address. 
	 tells you which set to look into*/
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
	Slot* currSlot = sets.at(index).isHit(tag);
	if(currSlot == nullptr){
		this->loadMisses ++;
		this->totalLoads ++;
		handleLoadMiss(address);
	} else {
		this->loadHits ++;
		this->totalLoads ++;
		handleLoadHit(address, currSlot);
	}
}


void Cache::store(std::string addressString){
	uint32_t address;
	address = stoi(addressString, nullptr, 16);
	int index = getIndex(address);
	int tag = getTag(address);
	Slot* currSlot = sets.at(index).isHit(tag);
	if(currSlot == nullptr){
		this->storeMisses ++;
		this->totalStores ++;
		handleStoreMiss(address);
	} else {
		this->storeHits ++;
		this->totalStores ++;
		handleStoreHit(address, currSlot);
	}
}



void Cache::handleStoreHit(int address, Slot* slot){
	if(writeThrough) {
		//update cache slot time accessed
		//update dirty bit
		slot->access_ts = cacheClock;
		slot->dirty = true;
		//update statistics
		//store to cache stat and to main mem stat
	} else {
		//update cache accessed time and stat for storing to cache
	}
}


void Cache::handleStoreMiss(int address){
	if(writeAllocate) {
		handleLoadMiss(address);
		//update stats accordingly for writing to cache
	} else {
		//no-write allocate
		//update stats for writing to memory
		this->totalCycles += 
	}
}


void Cache::handleLoadHit(int address, Slot* slot){
	//update slot's access time
	slot->access_ts = this->cacheClock;
}

void Cache::handleLoadMiss(int address){
	//no clue what to do LMAO
	
}