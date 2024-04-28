#include <cstddef>
#include <cstring>
#include <iostream>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "communications.h"
#include "client_connection.h"
#include "value_stack.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
{
	inTransaction = false;
	isLoggedIn = false;
	valStack = new ValueStack();
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
}

void ClientConnection::chat_with_client()
{

  rio_t in; // the read content
  rio_readinitb(&in, m_client_fd); // initialize the read input/output
  while (1) {
		Message* msg = new Message(); // the message object that will ultimately contain the appropriate content
		char *linebuffer = (char*) malloc(MAX_LINE_SIZE); // allocate maximum line size for the buffer
		if (rio_readlineb(&in, linebuffer, MAX_LINE_SIZE) < 0) {
			throw CommException("Unable to read message from client.");
		}
		// decode the processed line and set up the msg object
		
		try {
			MessageSerialization::decode(linebuffer, *msg);
		// Catches invalid message formats, like too few arguments
		} catch (const InvalidMessage& ex) {
			sendResponse(Message(MessageType::ERROR, {ex.what()}));
			handleBye();
		}

		try {
			processMessage(*msg);
		} 
		catch (const CommException& ex) { // irrecoverable
			sendResponse(Message(MessageType::ERROR, {ex.what()}));
			handleBye();
		}
		catch (const InvalidMessage& ex) { // irrecoverable
			sendResponse(Message(MessageType::ERROR, {ex.what()}));
			handleBye();
		} 
		catch (const FailedTransaction& ex) {
			sendResponse(Message(MessageType::FAILED, {ex.what()}));
			endTransaction(false);
		} 
		catch (const OperationException& ex) {
			sendResponse(Message(MessageType::FAILED, {ex.what()}));
		} 
			
	}

}

void ClientConnection::handleSet(Message msg) {

  Table* table = m_server->find_table(msg.get_table());
	if(table == NULL) {
		throw OperationException("no such table");
	}
  std::string value = valStack->get_top();
  valStack->pop(); // must also remove the top value
  if (!inTransaction) { // in autolock mode
		table->lock();
		table->set(msg.get_key(), value);
		table->unlock();
  } else { // in transaction mode, so we use trylock
		// Check if table can not be locked and it is not already locked by this transaction
		if (std::find(accessed.begin(), accessed.end(), table) == accessed.end() && !table->trylock()) {
			throw FailedTransaction("could not get table lock");
		}
		accessed.push_back(table); // keep track of currently locked tables in a vector
		table->set(msg.get_key(), value);
		table->unlock();
  }

}

void ClientConnection::handleGet(Message msg) {
	try {
		Table* table = m_server->find_table(msg.get_table());
		if (table == NULL) {
			throw OperationException("No such table");
		}
		std::string res("");
		try {
			res = table->get(msg.get_key());
		} catch (const std::out_of_range& ex) {
			throw OperationException("No such key");
		}
		valStack->push(res);
	} catch (const std::runtime_error& ex) {
		// got here by running: LOGIN LUCY, CREATE FRUIT, GET ABC A
		throw OperationException("Invalid table get access (SHOULD NEVER GET HERE. IS PROBLEM IF IT DOES)");
	}
	
}

void ClientConnection::popTwo(int* right, int*left) {
	try {
		*left = stoi(valStack->get_top());
		valStack->pop();
		*right = stoi(valStack->get_top());
		valStack->pop();
	} catch (const OperationException& ex) { // in case there were not two values
		throw OperationException(ex.what());
	}
	
}

void ClientConnection::sendResponse(Message msg) {
	int bytes_written = 0;
	std::string encoded_msg("");
	bytes_written = send_message(m_client_fd, &msg, encoded_msg);
	if(bytes_written < (int) std::strlen(encoded_msg.c_str())) {
		throw CommException("invalid write");
	}

}

void ClientConnection::handleLogin(Message msg) {
	loginName = msg.get_username();
	isLoggedIn = true;
}

void ClientConnection::handleCreate(Message msg) {
	m_server->create_table(msg.get_table());
}

void ClientConnection::handlePush(Message msg) {
	valStack->push(msg.get_arg(0));
}

void ClientConnection::handlePop(Message msg) {
	valStack->pop();
}

void ClientConnection::handleOpp(MessageType m) {
	try {
		int left, right;
		popTwo(&left, &right);
		int res;
		switch (m) {
			case MessageType::ADD:
				res = left + right;
				break;
			case MessageType::SUB:
				res = left - right;
				break;
			case MessageType::DIV:
				res = left / right;
				break;
			case MessageType::MUL:
				res = left * right;
				break;
			default:
				res = 0;
		}
		valStack->push(std::to_string(res));
	} catch (const std::invalid_argument& ex) {
		throw OperationException("Cannot perform arithmetic operation with non-integer value.");
	} catch (const OperationException& ex) { // FIXME: needs to be able to handle division by zero, etc
		throw OperationException("Unable to perform arithmetic operation");
	}
}

void ClientConnection::handleBye() {
	
	for (auto t = accessed.begin(); t != accessed.end(); ++t) { // unlock all tables
		(*t)->unlock();
	}
	close(m_client_fd); // close file descriptor
	// NOTE: we do not want to exit since then other clients cannot communicate- exit(0);
}

void ClientConnection::handleBegin() {
	if (inTransaction) {
		throw FailedTransaction("Attempt to begin a transaction from within a transaction");
	} else{
		inTransaction = true;
		transactionFailed = false;
	}
}

void ClientConnection::handleCommit() {
	if (!inTransaction) {
		throw OperationException("Attempt to commit from outside a transaction");
	} else {
		endTransaction(true);
	}
}

// Unlocks tables and rolls back changes if necessary
void ClientConnection::endTransaction(bool transactionSucceeded) {
	inTransaction = false;
	if (!transactionSucceeded) {
		for (auto t = accessed.begin(); t != accessed.end(); ++t) { // unlock all tables
			(*t)->rollback_changes();
			(*t)->unlock();
		}
	} else {
		for (auto t = accessed.begin(); t != accessed.end(); ++t) { // unlock all tables
			(*t)->commit_changes();
			(*t)->unlock();
		}
	}
}
	
void ClientConnection::processMessage(Message msg) {
	if (!isLoggedIn && msg.get_message_type() != MessageType::LOGIN) {
		throw InvalidMessage("First mesage must be login");
	}
	
	if (!msg.is_valid()) {
		throw InvalidMessage("Imporper message format");
	}
	switch (msg.get_message_type()) {
		case MessageType::LOGIN:
			handleLogin(msg);
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::CREATE:
			handleCreate(msg);
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::PUSH:
			std::cout << "thinks valid\n"; 
			handlePush(msg);
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::POP:
			handlePop(msg);
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::TOP:
			sendResponse(Message(MessageType::DATA, {valStack->get_top()}));
			break;
		case MessageType::SET:
			handleSet(msg);
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::GET:
			handleGet(msg);
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::ADD:
		case MessageType::SUB:
		case MessageType::MUL:
		case MessageType::DIV:
			handleOpp(msg.get_message_type());
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::BEGIN:
			handleBegin();
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::COMMIT:
			handleCommit();
			sendResponse(Message(MessageType::OK));
			break;
		case MessageType::BYE:
			sendResponse(Message(MessageType::OK));
			handleBye();
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