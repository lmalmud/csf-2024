#include <cstring>
#include <fstream>
#include <iostream>
#include <cassert>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdint>

#ifndef SLOT_H
#define SLOT_H

class Slot { // note that a slot is the same as a block
	public:
		uint32_t tag;
		//dirty = cache block has been modified
		//valid = does cache block have stuff we care about in it 
		bool dirty, valid; 
		uint32_t load_ts, access_ts;
		int bytes_per_block;

		Slot(int bytes_per_block);

		
	private:
};

#endif //SLOT_H