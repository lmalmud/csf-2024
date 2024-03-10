#include "slot.h"

/* Default constructor for Slot. */
Slot::Slot() {
    this->load_ts = 0;
    this->access_ts = 0;
    this->dirty = false;
    this->valid = false;
    this->tag = 0; // does not matter what tag value is initialized to, because slot is not valid
}