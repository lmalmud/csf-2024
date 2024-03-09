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

//each slot = cache block
class Slot {
	public:
		uint32_t tag, load_ts, access_ts;
		bool valid, wasModified;
		Slot();
		
	private:
};

#endif //SLOT_H