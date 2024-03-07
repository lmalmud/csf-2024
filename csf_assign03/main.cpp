#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cassert>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdint>
#include "cache.h"

using std::cout;
using std::endl;
using std::cerr;
using std::stoi; // convert string to intever
using std::string;



int main(int argc, char **argv){

	int test = stoi("0xFF", nullptr, 16);

	if (argc != 7) {
		cerr << "ERROR: Not enough arguments present." << endl;
		return 1;
	}

	int num_sets; // number of sets in the cache
	int num_blocks; // number of blocks in the cache
	int num_bytes; // number of bytes per block
	bool write_allocate; // true if write-allocate, false if no-write-allocate
	bool write_through; // true if write-through, false if write-back
	bool lru; // true if lru, false if fifo

	// fetch values of command-line arguments
	num_sets = stoi(argv[1]);
	num_blocks = stoi(argv[2]);
	num_bytes = stoi(argv[3]);
	write_allocate = strcmp(argv[4],"write-allocate") == 0 ? true : false; // false if no-write-allocate
	if (strcmp(argv[4],"write-allocate") != 0 && strcmp(argv[4],"no-write-allocate") != 0) {
		cerr << "ERROR: must specify either write-allocate or no-write-allocate." << endl;
		return 1;
	}
	write_through = strcmp(argv[5], "write-through") == 0 ? true : false; // false if write-back
	if (strcmp(argv[5], "write-through") != 0 && strcmp(argv[5], "write-back") != 0) {
		cerr << "ERROR: must specify either write-through or write-back." << endl;
		return 1;
	}
	lru = strcmp(argv[6], "lru") == 0 ? true : false; // false if fifo
	if (strcmp(argv[6], "lru") != 0 && strcmp(argv[6], "fifo") != 0) {
		cerr << "ERROR: must specify either lru or fifo." << endl;
		return 1;
	}

	// ensures that all numerical inputs are valid
	if ((num_sets & (num_sets - 1)) != 0 && (num_sets <= 0)) {
		cerr << "ERROR: number of sets is not a positive power of two." << endl;
		return 1;
	}
	if ((num_blocks & (num_blocks - 1)) != 0 && num_blocks <= 0) {
		cerr << "ERROR: number of blocks is not a positive power of two." << endl;
		return 1;
	}
	if ((num_bytes & (num_bytes - 1)) != 0 && num_bytes < 4) {
		cerr << "ERROR: number of bytes is not a positive power of two greater than four." << endl;
		return 1;
	}

	// ensures that there are not conflicting cache settings (write-back and no-write-allocate)
	if (!write_allocate && !write_through) {
		cerr << "ERROR: cannot write-back and no-write-allocate." << endl;
		return 1;
	}

	string line;
	string command;
	string addressString;

	std::stringstream ss;
	Cache* cache = new Cache(num_sets, num_blocks, num_blocks, write_allocate, write_through, lru);

  while (std::getline(std::cin, line)){
		ss << line;
		ss >> command;
		ss >> addressString;
		ss.str("");
		ss.clear();
		if(command.compare("l") == 0){
			cache->load(addressString);
		}
		else if (command.compare("s") == 0) {
			cache->store(addressString);
		}

	}
	cache->getStatistics();

	return 0;
}