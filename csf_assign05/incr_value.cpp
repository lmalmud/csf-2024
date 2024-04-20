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

  // TODO: proper error and respose message handling

	std::string encoded_msg("");
	std::string decoded_msg("");
	
	int fd = open_clientfd(hostname.c_str(), port.c_str());
	
	if(fd < 0) {
		throw CommException("Error with opening client fd");
	}
	rio_t file_desc;
	rio_readinitb(&file_desc, fd);
	try{
	Message msg(MessageType::LOGIN, {username});
	// MessageSerialization::encode(msg, encoded_msg);
	std::cout << "login \n";
	send_message(fd, &msg, encoded_msg);
	std::cout << "sent login \n";
	read_response(&file_desc, &msg);

	msg = Message(MessageType::GET, {table, key});
	std::cout << "get \n";
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	msg = Message(MessageType::PUSH, {"1"});
	std::cout << "push \n";
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

	msg = Message(MessageType::ADD, {});
	std::cout << "add \n";
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);


	msg = Message(MessageType::SET, {table, key});
	std::cout << "set \n";
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);


	msg = Message(MessageType::BYE, {});
	std::cout << "bye \n";
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);
	} catch(std::exception()) {
		close(fd);
	}
	close(fd);

}
