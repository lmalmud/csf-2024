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
  while(1){
		Message* msg = new Message(); // the message object that will ultimately contain the appropriate content
		char *linebuffer = (char*) malloc(MAX_LINE_SIZE); // allocate maximum line size for the buffer
		if (rio_readlineb(&in, linebuffer, MAX_LINE_SIZE) < 0) {
			throw new CommException("Unable to read message from client.");
		}

		// decode the processed line and set up the msg object
		MessageSerialization::decode(linebuffer, *msg);
		std::cout << linebuffer;
		if (inTransaction) {
			transaction.push_back(msg);
		} else {
			processMessage(*msg);
		}
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
		{
			std::string res = msg.get_quoted_text() + "\r\n";
			rio_writen(m_client_fd, res.c_str(), strlen(res.c_str()));
			break;	
		}
		case MessageType::DATA:
		{
			std::string res = "DATA " + msg.get_value() + "\r\n";
			rio_writen(m_client_fd, res.c_str(), strlen(res.c_str()));
			break;
		}
		default:
			break;
	}
}
	
/*
NOTES ON THIS FUNCTION [IN PROGRESS]
- all operations (except BYE) are written
- the only operation that properly sends a response back is LOGIN
*/
void ClientConnection::processMessage(Message msg) {
  switch (msg.get_message_type()) {
		case MessageType::LOGIN: // DONE & WORKS
			loginName = msg.get_username();
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::CREATE:
      		m_server->create_table(msg.get_table());
			break;
		case MessageType::PUSH: 
			std::cout << "PUSH\n"; // NOTE: currently, when PUSH is typed, it does not register here.
			valStack.push(msg.get_arg(0));
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::POP:
			std::cout << "POP\n";
			try {
				valStack.pop();
			} catch (OperationException &ex) {
				sendResponse(Message(MessageType::FAILED, {ex.what()}));
			}
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
		case MessageType::ADD: // FIXME: need to handle possible OperationExceptions
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
			if (!inTransaction) {
				throw OperationException("Attempt to commit from outside a transaction");
			} else {
				processTransaction();
			}
		case MessageType::BYE:
			// do stuff to close connection
			// FIXME: unlock all tables?
			// QUESTION: is there anything else that has to be done
			break;
		case MessageType::OK:
		case MessageType::FAILED:
		case MessageType::ERROR:
		case MessageType::DATA:
			throw CommException("Illegal message type recieved by server");
		default:
			std::cout << "DEFAULT CASE\n";
			throw InvalidMessage("Invalid message type");
			break;
	}
}