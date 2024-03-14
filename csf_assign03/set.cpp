#include "set.h"

/* Set constructor that adds numBlocks slots. */
Set::Set(int numBlocks) {
  this->numBlocks = numBlocks;
  for (int i = 0; i < this->numBlocks; ++i) {
    slots.push_back(Slot()); // calls Set constructor
  }
  
}

/* Returns the slot corresponding to the given tag if it
	is both present and valid. */
Slot* Set::isHit(uint32_t tag){
	for (int i = 0; i < numBlocks; i++){
		// the cache line with the desired tag must be both present and valid
		if (slots.at(i).tag == tag && slots.at(i).valid) {
			return &(slots.at(i));
		}
	}
	return nullptr;
}

/* Returns the slot that was the least recently used.
	Note that for directly mapped caches there is not choice
	of slot to return. */
Slot* Set::lru() {
	// we are guaranteed there is at least one slot, because we check parameters
	Slot* least = &slots.at(0);
	for (int i = 0; i < this->numBlocks; ++i) { // for each block in the set
		if (slots.at(i).access_ts < least->access_ts) { // if there was an earlier access time
			least = &slots.at(i); // track the earliest one
		}
	}
	return least;
}

/* Returns the slot that has been in the set for the longest.
	As with lru, for directly mapped caches, there is no choice
	of which block to return. */
Slot* Set::fifo() {
	// identical implementation as lru, but compares load time
	Slot* least = &slots.at(0);
	for (int i = 0; i < this->numBlocks; ++i) { 
		if (slots.at(i).load_ts < least->load_ts) { 
			least = &slots.at(i);
		}
	}
	return least;
}

/* Adds the address to the cache. */
bool Set::add(uint32_t tag, bool is_lru, uint32_t time) {
	// the line that we replace is determined by what replacement strategy
	Slot* target = is_lru ? lru() : fifo();
	bool replacedDirtyBlock = false;
	if (target->valid && target->dirty) {
		replacedDirtyBlock = true;
	}
	target->tag = tag; // update the least recently used block
	target->load_ts = time; // since the block was just loaded
	target->access_ts = time; // since the block was just used
	target->valid = true; // now the slot contains a valid address
	target->dirty = false;
	return replacedDirtyBlock;
}

int Set::howManyDirty(){
	int count = 0; 
	for(int i = 0; i < numBlocks; i++) {
		if(slots.at(i).dirty && slots.at(i).valid) {
			count ++;
		}
	}
	return count;
}

ostream& operator<<(ostream& os, const Set& s) {
	os << "\tSET." << endl;
	for (int i = 0; i < s.numBlocks; ++i) {
		os << s.slots.at(i);
	}
	return os;
}