#include <iostream>
#include <cassert>
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
	Message msg;
	std::string stringMsg("");

	do{
		msg = Message();
		stringMsg.clear();
		//reads line from socket as a message object
		int bytesRead = read_response(&m_fdbuf, &msg);
		if(bytesRead < 0) {
			throw CommException("invalid read of server");
		}

		if(inTransaction) {
			transaction.push_back(msg);
		}
		else{
			processMessage(msg);
		}

		
	} while(msg.get_message_type() != MessageType::BYE);
	

	if(msg.get_message_type() == MessageType::LOGIN) {
		
	}

}

// TODO: additional member functions

void ClientConnection::processTransaction() {
	for (auto s = transaction.begin(); s != transaction.end(); s++) {
		//lock tables and carry out transactions in message
	}
}

void ClientConnection::processMessage(Message msg) {
switch(msg.get_message_type()) {
		case MessageType::LOGIN:
			loginName = msg.get_username();
			break;
		case MessageType::CREATE:
			break;
		case MessageType::PUSH:
			valStack.push(msg.get_arg(0));
			break;
		case MessageType::POP:
			break;
		case MessageType::TOP:
			break;
		case MessageType::SET:
			break;
		case MessageType::GET:
			break;
		case MessageType::ADD:
			break;
		case MessageType::SUB:
			break;
		case MessageType::MUL:
			break;
		case MessageType::DIV:
			break;
		case MessageType::BEGIN:
			if(inTransaction) {
				throw OperationException("attemp to begin a transaction from within a transaction");
			}
			else{
				inTransaction = true;
			}
			break;
		case MessageType::COMMIT:
			if(!inTransaction) {
				throw OperationException("attemp to commit from outside a transaction");
			} else {
				processTransaction();
			}
		case MessageType::BYE:
			//do stuff to close connection
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