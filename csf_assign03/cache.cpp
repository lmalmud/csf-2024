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
	*/
	std::cout << "numSets: " << this->numSets << std::endl;
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

	// this->tick(); // update the internal cache clock
}

/* Updates the appropriate number of cycles for writing to memroy. */
void Cache::writeToMemory() {
	// loads from memory take 100 cycles for each four byte quantity that is transferred
	this->totalCycles += ((this->bytesPerBlock / 4) * 100);
}

void Cache::store(uint32_t address){
	// std::cout << "storing..." << std::endl;
	int index = getIndex(address);
	int tag = getTag(address);

	Slot* currSlot = sets.at(index).isHit(tag);
	this->totalStores++;
	// cout << getIndex(address) << endl; //for displaying the index
	if (currSlot == nullptr) {
		this->storeMisses++;
		handleStoreMiss(address);
	} else {
		this->storeHits++;
		// cout << getTag(address) << "\t" << currSlot->tag << endl;
		handleStoreHit(address, currSlot);
	}
	// this->tick(); // update the internal cache clock
}

/* This occurs when you are trying to rewrite a value, and it is
	in the cache. */
void Cache::handleStoreHit(int address, Slot* slot) {
	// std::cout << "store hit..." << std::endl;
	// this->storeHits++; // increment hits

	// the following operations occur whether it is write-allocate or not
	slot->access_ts = cacheClock; // update cache slot time accessed
	// this->sets.at(getIndex(address)).updateAccess(slot); // adjust the lru times of everything
	this->totalCycles += 1; // add one cycle for writing to cache
	
	if (this->writeThrough) { // none of the blocks will be dirty
		writeToMemory();
	} else {
		slot->dirty = true; // update dirty bit: now inconsistent with main memory
	}
	this->tick();
}

/* This occurs when you are trying to rewrite a value, but it is
	not in the cache. */
void Cache::handleStoreMiss(int address) {
	// std::cout << "store miss..." << std::endl;
	// this->storeMisses++;
	if (writeAllocate) { // note that on write-allocate, we do not write to memory
		handleLoadMiss(address);
		// sets.at(getIndex(address)).add(getTag(address), this->lru, this->cacheClock);
		// this->totalCycles += 1; // writing to the cache takes one cycle
	} else {
		// if the cache is no-write-allocate since we bypass the cache
		writeToMemory(); // update stats for writing to memory
	}	
	this->tick();
}


void Cache::handleLoadHit(int address, Slot* slot) {
	// std::cout << "load hit..." << std::endl;
	// update slot's access time
	// this->loadHits++;
	// QUESTION: is the load time updated if the item was already in the cache?
	if(slot->valid == 1) {
		slot->access_ts = this->cacheClock;
	}
	else{
		slot->load_ts = this->cacheClock; // set the time that the slot was added
	}
	// this->sets.at(getIndex(address)).updateAccess(slot); // adjust the lru times of everything
	this->totalCycles += 1; // loads from the cache only take one cycle
	this->tick();
}

void Cache::handleLoadMiss(int address) {
	// std::cout << "load miss..." << std::endl;
	// need to move the data from main memory into the cache
	// we know that we are going to have to fill up one of the slots- but which one?
	// this->loadMisses++;

	// add the value to the set
	
	bool replacedDirtyBlock = sets.at(getIndex(address)).add(getTag(address), this->lru, this->cacheClock);
	this->totalCycles++;
	writeToMemory();

	if(!writeThrough && replacedDirtyBlock) {
		writeToMemory(); //if write back and we just evicted a dirty block need to account for writing it to main mem
	}
	
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