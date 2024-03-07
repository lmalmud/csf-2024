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

class Slot {
	public:
		uint32_t tag;
		bool valid;
		uint32_t load_ts,
		access_ts;

		
	private:
};

#endif //SLOT_H