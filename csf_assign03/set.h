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

#ifndef SET_H
#define SET_H

class Set {
	public:
		std::vector<Slot> slots;
		Set(int numBlocks);
		Slot* isHit(uint32_t tag);
		void add(uint32_t tag, bool is_lru);
		
	private:
		int numBlocks; // the number of slots n the vector

		Slot* lru();
		Slot* fifo();

		void updateAccess(Slot* mostRecent);
		
};

#endif //SET_H