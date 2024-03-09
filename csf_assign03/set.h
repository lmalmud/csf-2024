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

		Set(int numSlots);

		//index is used to find which slot
		//if the valid bit is false, it is a hit
		//if the tag matches it is a hit
		//else it is a miss
		bool isHit(int tag, int index);

		


		
	private:
};

#endif //SET_H