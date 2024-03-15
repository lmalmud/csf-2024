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

using std::ostream;
using std::endl;

class Set {
	public:
		std::vector<Slot> slots;
		Set(int numBlocks);
		Slot* isHit(uint32_t tag);
		bool add(uint32_t tag, bool is_lru, uint32_t time);
		int howManyDirty();

		friend ostream& operator<<(ostream& os, const Set& s);
		
	private:
		int numBlocks; // the number of slots n the vector

		Slot* lru();
		Slot* fifo();
	
};

#endif //SET_H