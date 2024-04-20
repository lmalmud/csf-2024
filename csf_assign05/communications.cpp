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

	ssize_t bytes_read = rio_readlineb(fd, buf, 1024);
	std::string encoded_msg(buf);

	if (bytes_read < 0) {
		throw CommException("Invalid write on read");
		return 1;
	}

	MessageSerialization::decode(encoded_msg, *msg);
	if (msg->get_message_type() == MessageType::DATA) {
		std::cout << msg->get_value() << std::endl;
	} else if (msg->get_message_type() == MessageType::FAILED || msg->get_message_type() == MessageType::ERROR) {
		// TODO: properly throw errors
		throw FailedTransaction(msg->get_quoted_text());
	} 
	// NOTE: if response is OK, we do not want to print anything

	return bytes_read;
}

int send_message(int fd, Message* msg, std::string encoded_msg) {
	MessageSerialization::encode(*msg, encoded_msg);

	// std::cout << encoded_msg;
	int bytes_written = rio_writen(fd, encoded_msg.c_str(), strlen(encoded_msg.c_str()));
	if(bytes_written != (int) encoded_msg.size()) {
		throw CommException("invalid write on write");
	}
	return bytes_written;
}