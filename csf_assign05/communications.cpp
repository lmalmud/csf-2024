#include "communications.h"
#include "exceptions.h"
#include "message_serialization.h"
#include <iostream>


void test() {
	return;
}

int read_response(rio_t* fd, Message* msg) {
	char buf[1024];
	// buf[0] = '2';
	std::cout << "reading" << std::endl;
	ssize_t bytes_read = rio_readlineb(fd, buf, 2);
	std::string encoded_msg(buf);
	std::cout << encoded_msg << std::endl;
	if(bytes_read < 0) {
		throw CommException("invalid write on read");
	}
	std::cout << "read line \n";
	MessageSerialization::decode(encoded_msg, *msg);
	std::cout << "decoded msg \n";


	if(msg->get_message_type() == MessageType::FAILED || msg->get_message_type() == MessageType::ERROR) {
		throw FailedTransaction("");
	} 
	return bytes_read;
}

int send_message(int fd, Message* msg, std::string encoded_msg) {
	MessageSerialization::encode(*msg, encoded_msg);

	std::cout << encoded_msg;
	int bytes_written = rio_writen(fd, &encoded_msg, encoded_msg.size());
	if(bytes_written != encoded_msg.size()) {
		throw CommException("invalid write on write");
	}
	std:: cout << "done sending \n";
	return bytes_written;
}