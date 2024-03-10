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
		Set(int numBlocks, int bytesPerBlock);
		bool isHit(int tag);
		
	private:
		int numBlocks;
};

#endif //SET_H