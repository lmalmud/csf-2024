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
	
	if(fd < 0) {
		throw CommException("Error with opening client fd");
	}
	rio_t file_desc;
	rio_readinitb(&file_desc, fd);


	Message msg(MessageType::LOGIN, {username});
	MessageSerialization::encode(msg, encoded_msg);

	send_message(fd, &msg, encoded_msg);


	read_response(&file_desc, &msg);


	msg = Message(MessageType::GET, {table, key});

	send_message(fd, &msg, encoded_msg);

	read_response(&file_desc, &msg);



	msg = Message(MessageType::TOP, {table, key});

	send_message(fd, &msg, encoded_msg);

	read_response(&file_desc, &msg);

	msg = Message(MessageType::BYE, {});

	send_message(fd, &msg, encoded_msg);










	// Message loginMsg(MessageType::LOGIN, {username}); // create the login message
	// std::string encodedLoginMsg;
	// MessageSerialization::encode(loginMsg, encodedLoginMsg);

  // Message getMsg(MessageType::GET, {table, key}); // create the GET message from the given table
  // std::string encodedGetMsg;
  // MessageSerialization::encode(getMsg, encodedGetMsg);

  // Message topMsg(MessageType::TOP); // create the TOP message
  // std::string topMsgEncoded;
  // MessageSerialization::encode(topMsg, topMsgEncoded);

	

  // Message byeMsg(MessageType::BYE); // create the BYE message
  // std::string byeMsgEncoded;
  // MessageSerialization::encode(byeMsg, byeMsgEncoded);

	// int fd = open_clientfd(hostname.c_str(), port.c_str());
  // if (fd < 0) {
  //   std::cerr << "Error: Could not connect to server.\n";
  //   return 1;
  // }

  // rio_writen(fd, encodedLoginMsg.c_str(), strlen(encodedLoginMsg.c_str())); // send the login request

  // rio_t rio; // object for reading response from server
  // rio_readinitb(&rio, fd); // initialize reading back
  // char buf[1024]; // considers the maximum possible message size
  // ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));

  // if (n > 0) { // if we got a response
  //   std::cout << buf << std::endl;
  //   // FIXME: check if response is an error and if so return with nonzero exit after closing
  // }

  // // continue sendi g requests

  close(fd);
  return 0;
}
