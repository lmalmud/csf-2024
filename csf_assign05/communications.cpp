#include "communications.h"
#include "exceptions.h"
#include "message_serialization.h"
#include <iostream>


void test() {
	return;
}

int read_response(rio_t* fd, Message* msg) {
	char buf[1024];
	std::string buff_string(buf);

	// std::cout << buff_string.size() << std::endl;

	ssize_t bytes_read = rio_readlineb(fd, buf, 1024);
	std::string encoded_msg(buf);
	std::cout << encoded_msg;
	if(bytes_read < 0) {
		throw CommException("invalid write on read");
	}
	MessageSerialization::decode(encoded_msg, *msg);

	//TODO: properly throw exceptions for error conditions
	if(msg->get_message_type() == MessageType::FAILED || msg->get_message_type() == MessageType::ERROR) {
		throw FailedTransaction("");
	} 
	return bytes_read;
}

int send_message(int fd, Message* msg, std::string encoded_msg) {
	MessageSerialization::encode(*msg, encoded_msg);

	// std::cout << encoded_msg;
	int bytes_written = rio_writen(fd, encoded_msg.c_str(), strlen(encoded_msg.c_str()));
	if(bytes_written != encoded_msg.size()) {
		throw CommException("invalid write on write");
	}
	return bytes_written;
}