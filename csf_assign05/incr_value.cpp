#include "communications.h"
#include "exceptions.h"
#include "csapp.h"
#include "message_serialization.h"
#include "message.h"
#include <exception>
#include <iostream>

int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

std::string encoded_msg("");
std::string decoded_msg("");

int fd = open_clientfd(hostname.c_str(), port.c_str());

if(fd < 0) {
	throw CommException("Error with opening client fd");
	return 1;
}

rio_t file_desc;
rio_readinitb(&file_desc, fd);

try {

	Message msg(MessageType::LOGIN, {username});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	if (use_transaction) {
		Message msg(MessageType::BEGIN, {});
		send_message(fd, &msg, encoded_msg);
		read_response(&file_desc, &msg);
	}

	msg = Message(MessageType::GET, {table, key});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	msg = Message(MessageType::PUSH, {"1"});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	msg = Message(MessageType::ADD, {});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	msg = Message(MessageType::SET, {table, key});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	if (use_transaction) {
		msg = Message(MessageType::COMMIT, {});
		send_message(fd, &msg, encoded_msg);
		read_response(&file_desc, &msg);
	}

	msg = Message(MessageType::BYE, {});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

} catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << std::endl;
	close(fd);
	return 1;
}
close(fd);
return 0;

}
