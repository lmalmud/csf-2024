#include "cache.h"
#include <cmath>
#include <string>

using std::cout;
using std::endl;

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
		sets.push_back(Set(this->numBlocks)); // calls Set constructor
	}

	/*
	std::cout << "testing...." << std::endl;
	std::cout << "getIndex(0x12345678): " << getIndex(0x12345678) << std::endl;
	std::cout << "getOffset(0x123456): " << getOffset(0x12345678) << std::endl;
	std::cout << "getTag(0x12345678): " << getTag(0x12345678) << std::endl;
	*/
	std::cout << "this->numOffsetBits: " << this->numOffsetBits << std::endl;
	std::cout << "this->numIndexBits: " << this->numIndexBits << std::endl;
	std::cout << "this->numTagBits: " << this->numTagBits << std::endl;
}

/* Returns the part of the 32-bit address that corresponds
   	to the index. It is stored in the middle of the address. 
	Tells you which set to look into. */
uint32_t Cache::getIndex(int address) {
	//std::cout << "(address >> this->numTagBits): " << (address >> this->numTagBits) << std::endl;
	//std::cout << "((1 << this->numIndexBits) - 1): " << ((1 << this->numIndexBits) - 1) << std::endl;
	return (address >> this->numOffsetBits) & ((1 << this->numIndexBits) - 1);
}

/* Returns the part of the 32-bit address that corresponds
	to the offset. It is the rightmost part of the address. */
uint32_t Cache::getOffset(int address) {
	return address & ((1 << this->numOffsetBits) - 1);
}

/* Returns the part of the 32-bit address that corresponds
   to the tag. It is stored at the leftmost part of the address. */
uint32_t Cache::getTag(int address) {
	return address >> (this->numOffsetBits + this->numIndexBits);
}


void Cache::load(uint32_t address){
	int index = getIndex(address);
	int tag = getTag(address);

	cout << "beginning load..." << endl;
	cout << "address: " << address << endl;
	cout << "tag: " << tag << endl;
	cout << "index: " << index << endl;
	Slot* currSlot = sets.at(index).isHit(tag);
	if(currSlot == nullptr){
		handleLoadMiss(address);
	} else {
		handleLoadHit(address, currSlot);
	}

	this->tick(); // update the internal cache clock
}

void Cache::store(uint32_t address){
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
	this->tick(); // update the internal cache clock
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
	if (writeAllocate) {
		handleLoadMiss(address);
		// update stats accordingly for writing to cache
	} else { // if the cache is no-write-allocate, we bypass the cache
		//no-write allocate
		//update stats for writing to memory
		//this->totalCycles += 
	}
}


void Cache::handleLoadHit(int address, Slot* slot){
	// update slot's access time
	this->loadHits++;
	this->totalLoads++;
	slot->access_ts = this->cacheClock;
	this->totalCycles += 1; // loads from the cache only take one cycle
}

void Cache::handleLoadMiss(int address) {
	// need to move the data from main memory into the cache
	// we know that we are going to have to fill up one of the slots- but which one?
	this->loadMisses++;
	this->totalLoads++;

	// add the value to the set
	sets.at(getIndex(address)).add(getTag(address), this->lru);
	
	// loads from memory take 100 cycles for each four byte quantity that is transferred
	this->totalCycles += ((this->bytesPerBlock / 4) * 100);
}

void Cache::tick() {
	this->cacheClock++;
}

void Cache::getStatistics() {
	cout << "Total loads: " << this->totalLoads << endl;
	cout << "Total stores: " << this->totalStores << endl;
	cout << "Load hits: " << this->loadHits << endl;
	cout << "Load misses: " << this->loadMisses << endl;
	cout << "Store hits: " << this->storeHits << endl;
	cout << "Store misses: " << this->storeMisses << endl;
	cout << "Total cycles: " << this->totalCycles << endl;
}