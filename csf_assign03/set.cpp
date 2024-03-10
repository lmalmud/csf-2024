#include "set.h"

Set::Set(int numBlocks, int bytesPerBlock) {
  this->numBlocks = numBlocks;
  for (int i = 0; i < numBlocks; ++i) {
    slots.push_back(Slot(bytesPerBlock)); // calls Set constructor
  }
  
}

//not sure if this is the correct logic
Slot* Set::isHit(int tag){
	for (int i = 0; i < numBlocks; i++){
		if(slots.at(i).tag == tag){
			return &(slots.at(i));
		}
	}
	return nullptr;
}