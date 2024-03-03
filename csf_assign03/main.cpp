#include <cstring>
#include <fstream>
#include <iostream>
#include <cassert>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::cerr;

int main(int argc, char **argv){

	if (argc != 7) {
		cerr << "ERROR: Not enough arguments present." << endl;
	}

	int num_sets; // number of sets in the cache
	int num_blocks; // number of blocks in the cache
	int num_bytes; // number of bytes per block
	bool write_allocate; // true if write-allocate, false if no-write-allocate
	bool write_through; // true if write-through, false if write-back
	bool lru; // true if lru, false if fifo

	num_sets = std::stoi(argv[1]);

}