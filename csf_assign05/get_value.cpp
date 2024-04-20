#include <iostream>
#include <system_error>
#include "exceptions.h"
#include "message.h"
#include "message_serialization.h"
#include "csapp.h"
#include "communications.h"

int main(int argc, char **argv)
{

  if ( argc != 6 ) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

	Message msg(MessageType::LOGIN, {username});
	std::string encoded_msg;
	std::string decoded_msg;
	
	MessageSerialization::encode(msg, encoded_msg);
	int fd = open_clientfd(hostname.c_str(), port.c_str());
	if(fd < 0) {
		throw CommException("Error with opening client fd");
	}
	rio_t file_desc;
	rio_readinitb(&file_desc, fd);

	std::cout << "connected \n";

	msg = Message(MessageType::LOGIN, {username});
	
	send_message(fd, &msg, encoded_msg);

	std::cout << "sent login \n";

	// read_response(file_desc, &msg);

	
	msg = Message(MessageType::PUSH, {"2"});
	send_message(fd, &msg, encoded_msg);


	// msg = Message(MessageType::GET, {table, key});

	// send_message(fd, &msg, encoded_msg);

	// std::cout << "sent first \n";

	// // read_response(file_desc, &msg);

	// // std::cout << "recieved first \n";



	// msg = Message(MessageType::TOP, {table, key});

	// send_message(fd, &msg, encoded_msg);

	// read_response(&file_desc, &msg);

	msg = Message(MessageType::BYE, {});

	send_message(fd, &msg, encoded_msg);

}
