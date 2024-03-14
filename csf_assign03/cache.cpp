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
	this->numIndexBits = (int) std::log2(this->numSets); // numBlocks = slots
	this->numTagBits = 32 - this->numOffsetBits - this->numIndexBits;
	
	this->cacheClock = 1;

	// create sets
	for (int i = 0; i < this->numSets; ++i) {
		sets.push_back(Set(this->numBlocks)); // calls Set constructor
	}

	/*
	std::cout << "testing...." << std::endl;
	std::cout << "getIndex(0x12345678): " << getIndex(0x12345678) << std::endl;
	std::cout << "getOffset(0x123456): " << getOffset(0x12345678) << std::endl;
	std::cout << "getTag(0x12345678): " << getTag(0x12345678) << std::endl;
	std::cout << "numSets: " << this->numSets << std::endl;
	std::cout << "this->numOffsetBits: " << this->numOffsetBits << std::endl;
	std::cout << "this->numIndexBits: " << this->numIndexBits << std::endl;
	std::cout << "this->numTagBits: " << this->numTagBits << std::endl;
	*/
	
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
	// std::cout << "loading..." << std::endl;
	int index = getIndex(address);
	int tag = getTag(address);

	Slot* currSlot = sets.at(index).isHit(tag);
	this->totalLoads++; // always increment total loads
	// cout << getIndex(address) << endl; //for displaying the index
	if(currSlot == nullptr){
		this->loadMisses++;
		handleLoadMiss(address); // will increment loadMisses
	} else {
		// cout << getTag(address) << "\t" << currSlot->tag << endl;
		this->loadHits++;
		handleLoadHit(address, currSlot); // will increment loadHits
	}
}

/* Updates the appropriate number of cycles for writing to memroy. */
void Cache::writeToMemory() {
	// loads from memory take 100 cycles for each four byte quantity that is transferred
	this->totalCycles += ((this->bytesPerBlock / 4) * 100);
}

/* Given an address, adds it to the cache, assuming that it is consistent
	with memory. Evicts dirty blocks and updates memory count appropriately. 
	Does not assume that main memory is acccessed in order to write initial value. */
void Cache::overwriteBlock(int address) {
	bool replacedDirtyBlock = sets.at(getIndex(address)).add(getTag(address), this->lru, this->cacheClock);
	this->totalCycles++;
	if (!writeThrough && replacedDirtyBlock) {
		writeToMemory(); // if write back and we just evicted a dirty block need to account for writing it to main mem
	}
}

void Cache::store(uint32_t address){

	int index = getIndex(address);
	int tag = getTag(address);

	Slot* currSlot = sets.at(index).isHit(tag);
	this->totalStores++;

	if (currSlot == nullptr) {
		this->storeMisses++;
		handleStoreMiss(address);
	} else {
		this->storeHits++;
		handleStoreHit(address, currSlot);
	}
}

/* This occurs when you are trying to rewrite a value, and it is
	in the cache. */
void Cache::handleStoreHit(int address, Slot* slot) {

	// the following operations occur whether it is write-allocate or not
	slot->access_ts = cacheClock; // update cache slot time accessed
	slot->load_ts = cacheClock; // update since a new value was loaded (check me NOT SURE- do we count same tag but new value as new?)
	this->totalCycles += 1; // add one cycle for writing to cache
	
	overwriteBlock(address); // always write to the cache
	if (this->writeThrough) {
		writeToMemory(); // also write to memory
	}
	this->tick();
}

/* This occurs when you are trying to rewrite a value, but it is
	not in the cache. */
void Cache::handleStoreMiss(int address) {
	writeToMemory(); // on a write miss, you always have to load from memory first
	if (writeAllocate) { // MUST: load into cache, update line in cache
		// note that we do not have to write to memory once more because we already have the value from memory
		overwriteBlock(address); // handles both loading into cache and updating line
	}
	this->tick();
}


void Cache::handleLoadHit(int address, Slot* slot) {
	slot->access_ts = this->cacheClock;
	this->totalCycles += 1; // loads from the cache only take one cycle
	this->tick();
}

void Cache::handleLoadMiss(int address) {
	overwriteBlock(address);
	writeToMemory(); // the time it takes to load the value from main memory
	this->tick();
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

//not sure if we want this but it goes through at the end of the cache's life and stores any dirty blocks
void Cache::cleanUpCache(){
	if(writeThrough){
		return;
	}
	int numDirty = 0;
	for(int i = 0; i < numSets; i++) {
		numDirty += sets.at(i).howManyDirty();
	}
	this->totalCycles += numDirty * ((this->bytesPerBlock / 4) * 100);
	cout << numDirty << endl;
}

ostream& operator<<(ostream& os, const Cache& c) {
	os << "CACHE. TIME: " << c.cacheClock << endl;
	for (int i = 0; i < c.numSets; ++i) {
		os << i << endl;
		os << c.sets.at(i);
	}
	return os;
}