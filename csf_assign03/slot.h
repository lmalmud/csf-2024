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

using std::ostream;
using std::endl;

class Slot { // note that a slot is the same as a block
	public:
		uint32_t tag;
		bool dirty; // true if cache block has been modified
		bool valid; // true if cache block have stuff we care about in it 
		uint32_t load_ts; // the time that the slot was added
		uint32_t access_ts; // must be modified each time

		friend ostream& operator<<(ostream& os, const Slot& s);

		Slot();
	
	private:
};

#endif //SLOT_H