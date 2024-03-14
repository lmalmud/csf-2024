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
void Set::add(uint32_t tag, bool is_lru, uint32_t time) {
	// the line that we replace is determined by what replacement strategy
	Slot* target = is_lru ? lru() : fifo();
	target->tag = tag; // update the least recently used block
	target->load_ts = time; // since the block was just loaded
	updateAccess(target); // need to also update all accesses
	target->valid = true; // now the slot contains a valid address
}

/* Given the slot that was most recently accessed, sets
	its access_ts to 0 and increments all other accesses
	accordingly. */
void Set::updateAccess(Slot* mostRecent) {
	for(auto slot = this->slots.begin(); slot != this->slots.end(); ++slot) {
		if (slot->tag != mostRecent->tag) { // slots are uniquely identified by their tags
			slot->access_ts++;
		} else {
			mostRecent->access_ts = 0; // FIXME
		}
	}
}

ostream& operator<<(ostream& os, const Set& s) {
	os << "\tSET." << endl;
	for (int i = 0; i < s.numBlocks; ++i) {
		os << s.slots.at(i);
	}
	return os;
}