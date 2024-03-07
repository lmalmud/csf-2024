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
		uint32_t getTag(int address);
		int numSets,  numBlocks,  bytesPerBlock;
		bool writeAllocate, writeThrough, lru;


	public:
		Cache(int numSets, int numBlocks, int bytesPerBlock, bool writeAllocate, bool writeThrough, bool lru);
		void getStatistics();

		void load(std::string addressString);

		void store(std::string addressString);

};

#endif // CACHE_H


