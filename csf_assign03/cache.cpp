#include "cache.h"




Cache::Cache(int numSets, int numBlocks, int bytesPerBlock, bool writeAllocate, bool writeThrough, bool lru){
	this->numSets = numSets;
	this->numBlocks = numBlocks;
	this->bytesPerBlock = bytesPerBlock;
	this->writeAllocate = writeAllocate;
	this->writeThrough = writeThrough;
	this->lru = lru;

}


void Cache::load(std::string addressString){
	uint32_t address;
	address = stoi(addressString, nullptr, 16);
	int tage = getTag(address);
}