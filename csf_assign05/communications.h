#ifndef COMMUNICATIONS
#define COMMUNICATIONS

#include "message.h"
#include <string>
#include "csapp.h"

int read_response(rio_t* fd, Message* msg);

int send_message(int fd, Message* msg, std::string encoded_msg);

void test();

#endif //COMMUNICATIONS