#include "communications.h"
#include "exceptions.h"
#include "csapp.h"
#include "message_serialization.h"
#include "message.h"
#include <iostream>


int main(int argc, char **argv)
{
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

  // TODO: proper error and respose message handling
	std::string encoded_msg("");
	std::string decoded_msg("");
	
	int fd = open_clientfd(hostname.c_str(), port.c_str());
	
	if (fd < 0) {
		throw CommException("Error with opening client fd");
	}

	rio_t file_desc;
	rio_readinitb(&file_desc, fd);

	try {
		// first message is login
		Message msg(MessageType::LOGIN, {username});
		send_message(fd, &msg, encoded_msg);
		read_response(&file_desc, &msg);

		// second messages pushes the desired input onto the value stack
		msg = Message(MessageType::PUSH, {value});
		send_message(fd, &msg, encoded_msg);
		read_response(&file_desc, &msg);

		// third message calls the SET function to assign the key value pair
		msg = Message(MessageType::SET, {table, key});
		send_message(fd, &msg, encoded_msg);
		read_response(&file_desc, &msg);

		// fourth message politely terminates the connection
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
