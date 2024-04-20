#include <iostream>
#include <system_error>
#include "exceptions.h"
#include "csapp.h"
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

	std::string encoded_msg("");
	std::string decoded_msg("");
	
	int fd = open_clientfd(hostname.c_str(), port.c_str());
	
	if (fd < 0) {
		throw CommException("Error with opening client fd");
    return 1;
	}

	rio_t file_desc;
	rio_readinitb(&file_desc, fd);

  // first message is login
	Message msg(MessageType::LOGIN, {username});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

  // second message is get
	msg = Message(MessageType::GET, {table, key});
	send_message(fd, &msg, encoded_msg);
  read_response(&file_desc, &msg);

  // third message is top
	msg = Message(MessageType::TOP, {table, key});
	send_message(fd, &msg, encoded_msg);
	read_response(&file_desc, &msg);

  // fourth message is to politely close the connection
	msg = Message(MessageType::BYE, {});
	send_message(fd, &msg, encoded_msg);

  close(fd);
  return 0;

}
