#include <iostream>
#include <cassert>
#include <iostream>
#include <algorithm>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"
#include "communications.h"
#include "value_stack.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
{
	inTransaction = false;
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
}

void ClientConnection::chat_with_client()
{
  // NOTE: this function does not operate within a loop
  // because it will be repeatedly called from the main server loop

  rio_t in; // the read content
  rio_readinitb(&in, m_client_fd); // initialize the read input/output
  Message* msg = new Message(); // the message object that will ultimately contain the appropriate content
  char *linebuffer = (char*) malloc(MAX_LINE_SIZE); // allocate maximum line size for the buffer
  if (Rio_readlineb(&in, linebuffer, MAX_LINE_SIZE) < 0) {
    throw new CommException("Unable to read message from client.");
  }

  // decode the processed line and set up the msg object
  MessageSerialization::decode(linebuffer, *msg);

  if (inTransaction) {
    transaction.push_back(msg);
  } else {
    processMessage(*msg);
  }

}

void ClientConnection::processTransaction() {
	for (auto s = transaction.begin(); s != transaction.end(); s++) {
		// lock tables and carry out transactions in message
	}
}

void ClientConnection::handleSet(Message msg) {
  Table* table = m_server->find_table(msg.get_table());
  std::string value = valStack.get_top();
  valStack.pop(); // must also remove the top value
  if (!inTransaction) { // in autolock mode

  } else { // in transaction mode, so we use trylock
	table->lock();
	locked.push_back(table); // keep track of currently locked tables in a vector
	table->set(msg.get_key(), value);
	table->unlock();
	locked.erase(std::find(locked.begin(), locked.end(), table));
  }

}

void ClientConnection::popTwo(int* right, int*left) {
	*left = stoi(valStack.get_top());
	valStack.pop();
	*right = stoi(valStack.get_top());
	valStack.pop();
}

void ClientConnection::sendResponse(Message msg) {
	switch (msg.get_message_type()) { // FIXME: DO THE REST.
		case MessageType::OK:
		{
			std::string res = "OK\r\n";
			rio_writen(m_client_fd, res.c_str(), strlen(res.c_str()));
			break;
		}
		case MessageType::FAILED:
		case MessageType::ERROR:
			break;	
		case MessageType::DATA:
			break;
		default:
			break;
	}
}
	

void ClientConnection::processMessage(Message msg) {
  switch (msg.get_message_type()) {
		case MessageType::LOGIN:
			loginName = msg.get_username();
			break;
		case MessageType::CREATE:
      		m_server->create_table(msg.get_table());
			break;
		case MessageType::PUSH:
			valStack.push(msg.get_arg(0));
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::POP:
      		valStack.pop();
			break;
		case MessageType::TOP:
      		valStack.get_top();
			break;
		case MessageType::SET:
      		handleSet(msg);
			break;
		case MessageType::GET:
		{
			Table* table = m_server->find_table(msg.get_table());
			std::string res = table->get(msg.get_key());
			valStack.push(res);
			break;
		}
		case MessageType::ADD:
		{
			int left, right;
			popTwo(&left, &right);
			int res = left + right;
			valStack.push(std::to_string(res));
			sendResponse(Message(MessageType::OK));
			break;
		}
		case MessageType::SUB:
		{
			int left, right;
			popTwo(&left, &right);
			int res = left - right;
			valStack.push(std::to_string(res));
			break;
		}
		case MessageType::MUL:
		{
			int left, right;
			popTwo(&left, &right);
			int res = left * right;
			valStack.push(std::to_string(res));
			break;
		}
		case MessageType::DIV:
		{
			int left, right;
			popTwo(&left, &right);
			int res = left / right;
			valStack.push(std::to_string(res));
			break;
		}
		case MessageType::BEGIN:
			if (inTransaction) {
				throw OperationException("Attempt to begin a transaction from within a transaction");
			} else{
				inTransaction = true;
			}
			break;
		case MessageType::COMMIT:
			if(!inTransaction) {
				throw OperationException("Attempt to commit from outside a transaction");
			} else {
				processTransaction();
			}
		case MessageType::BYE:
			// do stuff to close connection
			break;
		case MessageType::OK:
		case MessageType::FAILED:
		case MessageType::ERROR:
		case MessageType::DATA:
			throw CommException("Illegal message type recieved by server");
		default:
			throw InvalidMessage("Invalid message type");
			break;
	}
}