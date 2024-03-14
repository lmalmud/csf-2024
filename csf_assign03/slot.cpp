#include "slot.h"

/* Default constructor for Slot. */
Slot::Slot() {
    this->load_ts = 0;
    this->access_ts = 0;
    this->dirty = false;
    this->valid = false;
    this->tag = 0; // does not matter what tag value is initialized to, because slot is not valid
}

ostream& operator<<(ostream& os, const Slot& s) {
    os << "\t\tSLOT. TAG: " << s.tag << ", DIRTY: " << s.dirty << ", VALID: " << s.valid << ", LOAD_TS: " << s.load_ts << ", ACCESS_TS: " << s.access_ts << endl;
    return os;
}