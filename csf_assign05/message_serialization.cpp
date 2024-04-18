#include <exception>
#include <string>
#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // TODO: implement



	MessageType msgType = msg.get_message_type();
	encoded_msg.clear();

	switch(msgType){
		case MessageType::LOGIN:
			encoded_msg += "LOGIN " + msg.get_username();
			break;
		case MessageType::CREATE:
			encoded_msg += "CREATE " + msg.get_table();
			break; 
		case MessageType::SET:
			encoded_msg += "SET " + msg.get_table() + " " + msg.get_key();
			break; 
		case MessageType::GET:
			encoded_msg += "GET " + msg.get_table() + " " + msg.get_key();
			break;
		case MessageType::PUSH:
			encoded_msg += "PUSH " + msg.get_value();
			break;
		case MessageType::POP:
			encoded_msg += "POP";
			break;
		case MessageType::TOP:
			encoded_msg += "TOP";
			break;
		case MessageType::ADD:
			encoded_msg += "ADD";
			break;
		case MessageType::SUB:
			encoded_msg += "SUB";
			break;
		case MessageType::MUL:
			encoded_msg += "MUL";
			break;
		case MessageType::DIV:
			encoded_msg += "DIV";
			break;
		case MessageType::BEGIN:
			encoded_msg += "BEGIN";
			break;
		case MessageType::COMMIT:
			encoded_msg += "COMMIT";
			break;
		case MessageType::BYE:
			encoded_msg += "BYE";
			break;
		case MessageType::OK:
			encoded_msg += "OK";
			break;
		case MessageType::FAILED:
			encoded_msg += "FAILED " + msg.get_quoted_text();
			break;
		case MessageType::ERROR:
			encoded_msg += "ERROR " + msg.get_quoted_text();
			break;
		case MessageType::DATA:
			encoded_msg += "DATA " + msg.get_value();
			break;
		case MessageType::NONE:
			break;
		}

		encoded_msg += "\n";

	if(encoded_msg.length() > Message::MAX_ENCODED_LEN) {
		throw InvalidMessage("too long");
	}
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  std::stringstream s(encoded_msg_);
	std::string currArg("");

	s >> currArg;

	std::map<std::string,MessageType> stringToArg = {{"LOGIN", MessageType::LOGIN}, 
	{"CREATE", MessageType::CREATE}, {"PUSH", MessageType::PUSH}, {"POP", MessageType::POP}, 
	{"TOP", MessageType::TOP}, {"SET", MessageType::SET}, {"GET", MessageType::GET}, 
	{"ADD", MessageType::ADD}, {"SUB", MessageType::SUB}, {"MUL", MessageType::MUL}, 
	{"DIV", MessageType::DIV}, {"BEGIN", MessageType::BEGIN}, {"COMMIT", MessageType::COMMIT}, 
	{"BYE", MessageType::BYE}, {"OK", MessageType::OK}, {"FAILED", MessageType::FAILED}, 
	{"ERROR", MessageType::ERROR}, {"DATA", MessageType::DATA}, {"NONE", MessageType::NONE}};

	msg.set_message_type(stringToArg[currArg]);


	switch(stringToArg[currArg]) {

		case MessageType::LOGIN:
		case MessageType::PUSH:
		case MessageType::CREATE:
			s >> currArg;
			msg.push_arg(currArg);
			break; 
		case MessageType::SET:
		case MessageType::GET:
			s >> currArg;
			msg.push_arg(currArg);
			currArg = "";
			s >> currArg;
			msg.push_arg(currArg);
			break;
		case MessageType::FAILED:
			// encoded_msg += "FAILED " + msg.get_quoted_text();
			break;
		case MessageType::ERROR:
			// encoded_msg += "ERROR " + msg.get_quoted_text();
			break;
		case MessageType::DATA:
			// encoded_msg += "DATA " + msg.get_value();
			break;
		case MessageType::POP:
		case MessageType::TOP:
		case MessageType::ADD:
		case MessageType::SUB:
		case MessageType::MUL:
		case MessageType::DIV:
		case MessageType::BEGIN:
		case MessageType::COMMIT:
		case MessageType::BYE:
		case MessageType::OK:
		case MessageType::NONE:
		break;
	}
}
