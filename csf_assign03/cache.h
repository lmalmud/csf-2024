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
		bool writeAllocate, writeThrough, lru;

		uint32_t getIndex(int address);
		uint32_t getTag(int address);

		int numOffsetBits, numIndexBits, numTagBits;

		void handleLoadMiss(int address);

		void handleLoadHit(int address);

		void handleStoreMiss(int address);

		void handleStoreHit(int address);


	public:
		
		Cache(int numSets, int numBlocks, int bytesPerBlock, bool writeAllocate, bool writeThrough, bool lru);
		
		void getStatistics();
		
		void load(std::string addressString);
		
		void store(std::string addressString);


};

#endif // CACHE_H