#include <cstring>
#include <fstream>
#include <iostream>
#include <cassert>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdint>
#include "slot.h"
#include "set.h"

#ifndef CACHE_H
#define CACHE_H


class Cache {
	private:
		std::vector<Set> sets;
		
		int numSets,  numBlocks,  bytesPerBlock;
		bool writeAllocate; // if false, then no-write-alocate
		bool writeThrough; // if false, then writeBack
		bool lru; // if false, then fifo

		int numOffsetBits, numIndexBits, numTagBits;
		uint32_t cacheClock;
		
		//statistics:
		uint32_t totalLoads;
		uint32_t totalStores;
		uint32_t loadHits;
		uint32_t loadMisses;
		uint32_t storeHits;
		uint32_t storeMisses;
		uint32_t totalCycles;

		uint32_t getIndex(int address);
		uint32_t getTag(int address);
		uint32_t getOffset(int offset);

		void handleLoadMiss(int address);

		void handleLoadHit(int address, Slot* slot);

		void handleStoreMiss(int address);

		void handleStoreHit(int address, Slot* slot);

		void tick();


		friend ostream& operator<<(ostream& os, const Cache& c);


	public:
		
		Cache(int numSets, int numBlocks, int bytesPerBlock, bool writeAllocate, bool writeThrough, bool lru);
		
		void getStatistics();
		
		void load(uint32_t address);
		
		void store(uint32_t address);

		void writeToMemory();

		void cleanUpCache();
};

#endif // CACHE_H