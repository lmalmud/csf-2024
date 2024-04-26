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
  while (1) {
		Message* msg = new Message(); // the message object that will ultimately contain the appropriate content
		char *linebuffer = (char*) malloc(MAX_LINE_SIZE); // allocate maximum line size for the buffer
		if (rio_readlineb(&in, linebuffer, MAX_LINE_SIZE) < 0) {
			throw new CommException("Unable to read message from client.");
		}

		// decode the processed line and set up the msg object
		MessageSerialization::decode(linebuffer, *msg);
		std::cout << linebuffer;
		processMessage(*msg);
	}

}

void ClientConnection::handleSet(Message msg) {
  Table* table = m_server->find_table(msg.get_table());
  std::string value = valStack.get_top();
  valStack.pop(); // must also remove the top value
  if (!inTransaction) { // in autolock mode
	table->lock();
	table->set(msg.get_key(), value);
	table->unlock();
	sendResponse(Message(MessageType::OK));
  } else { // in transaction mode, so we use trylock
	if (!table->trylock()) {
		// failed, so when the message is comitted we need to roll back all changes
		transactionFailed = true;
	}
	accessed.push_back(table); // keep track of currently locked tables in a vector
	table->set(msg.get_key(), value);
	table->unlock();
	sendResponse(Message(MessageType::OK));
  }

}

void ClientConnection::handleGet(Message msg) {
	try {
		Table* table = m_server->find_table(msg.get_table());
		std::string res = table->get(msg.get_key());
		valStack.push(res);
		sendResponse(Message(MessageType::DATA, {res}));
	} catch (std::runtime_error& ex) {
		throw new OperationException("Invalid table get access");
	}
	
}

void ClientConnection::popTwo(int* right, int*left) {
	try {
		*left = stoi(valStack.get_top());
		valStack.pop();
		*right = stoi(valStack.get_top());
		valStack.pop();
	} catch (OperationException& ex) { // in case there were not two values
		throw new OperationException(ex.what());
	}
	
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
			std::string res = "ERROR \"" + msg.get_quoted_text() + "\"\r\n";
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

void ClientConnection::handleLogin(Message msg) {
	loginName = msg.get_username();
	sendResponse(Message(MessageType::OK));
}

void ClientConnection::handleCreate(Message msg) {
	m_server->create_table(msg.get_table());
	sendResponse(Message(MessageType::OK));
}

void ClientConnection::handlePush(Message msg) {
	valStack.push(msg.get_arg(0));
	sendResponse(Message(MessageType::OK));
}

void ClientConnection::handlePop(Message msg) {
	try {
		valStack.pop();
		sendResponse(Message(MessageType::OK));
	} catch (OperationException &ex) {
		sendResponse(Message(MessageType::FAILED, {ex.what()}));
	}
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
		valStack.push(std::to_string(res));
		sendResponse(Message(MessageType::OK));
	} catch (const OperationException& ex) { // FIXME: needs to be able to handle division by zero, etc
		throw new OperationException("Unable to perform arithmetic operation");
	}
}

void ClientConnection::handleBye() {
	// FIXME: is this something that we need here or does it only matter when we close a transaction
	for (auto t = accessed.begin(); t != accessed.end(); ++t) { // unlock all tables
		(*t)->unlock();
	}
	close(m_client_fd); // close file descriptor
	// exit(0); <- FIXME: is this something that we want?
}

void ClientConnection::handleBegin() {
	if (inTransaction) {
		throw OperationException("Attempt to begin a transaction from within a transaction");
	} else{
		inTransaction = true;
		transactionFailed = false;
	}
}

void ClientConnection::handleCommit() {
	if (!inTransaction) {
		throw OperationException("Attempt to commit from outside a transaction");
	} else {
		inTransaction = false;
		if (transactionFailed) {
			for (auto t = accessed.begin(); t != accessed.end(); ++t) { // unlock all tables
				(*t)->rollback_changes();
				(*t)->unlock();
			}
			throw FailedTransaction("Transaction failure. Changes have been rolled back.");
		} else {
			for (auto t = accessed.begin(); t != accessed.end(); ++t) { // unlock all tables
				(*t)->commit_changes();
				(*t)->unlock();
			}
		}
	}
}
	
void ClientConnection::processMessage(Message msg) {
	try {
		if (false) { // FIXME: add appropriate condition
			throw new InvalidMessage("First mesage must be login");
		}
		switch (msg.get_message_type()) {
			case MessageType::LOGIN:
				handleLogin(msg);
				break;
			case MessageType::CREATE:
				handleCreate(msg);
				break;
			case MessageType::PUSH: 
				handlePush(msg);
				break;
			case MessageType::POP:
				handlePop(msg);
				break;
			case MessageType::TOP:
				valStack.get_top();
				break;
			case MessageType::SET:
				handleSet(msg);
				break;
			case MessageType::GET:
				handleGet(msg);
				break;
			case MessageType::ADD:
			case MessageType::SUB:
			case MessageType::MUL:
			case MessageType::DIV:
				handleOpp(msg.get_message_type());
				break;
			case MessageType::BEGIN:
				handleBegin();
				break;
			case MessageType::COMMIT:
				handleCommit();
				break;
			case MessageType::BYE:
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
	} catch (const OperationException& ex) {
		sendResponse(Message(MessageType::FAILED, {ex.what()}));
	} catch (const CommException& ex) { // irrecoverable
		sendResponse(Message(MessageType::FAILED, {ex.what()}));
	} catch (const InvalidMessage& ex) { // irrecoverable
		sendResponse(Message(MessageType::FAILED, {ex.what()}));
	} catch (const FailedTransaction& ex) {
		sendResponse(Message(MessageType::FAILED, {ex.what()}));
	}
	
}