#include <set>
#include <map>
#include <regex>
#include <cassert>
#include <iostream>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs )
{
  // TODO: implement - DONE
  this->m_message_type = rhs.get_message_type();
  this->m_args.clear(); // remove any previous values
  for(int i = 0; i < (int) rhs.get_num_args(); ++i) { // make deep copy of arguments list
    this->m_args.push_back(rhs.get_arg(i));
  }
  return *this;
}

MessageType Message::get_message_type() const
{
  return m_message_type;
}

void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}

std::string Message::get_username() const
{
  // TODO: implement - DONE
  // FIXME: is there anything we are supposed to do if someone tries
  // to get the username on a request that is not a login request?
	// I think we should throw an exception?
  if (this->m_message_type == MessageType::LOGIN) {
    return this->m_args.at(0);
  }
  return "";
}

std::string Message::get_table() const
{
  // TODO: implement - DONE
  if (this->m_message_type == MessageType::GET || this->m_message_type == MessageType::SET || this->m_message_type == MessageType::CREATE) {
    return this->m_args.at(0);
  }
  return "";
}

std::string Message::get_key() const
{
  // TODO: implement - DONE
  if (this->m_message_type == MessageType::GET || this->m_message_type == MessageType::SET) {
    return this->m_args.at(1);
  }
  return "";
}

std::string Message::get_value() const
{
  // TODO: implement - DONE
  if (this->m_message_type == MessageType::PUSH || this->m_message_type == MessageType::DATA) {
    return this->m_args.at(0);
  }
  return "";
}

std::string Message::get_quoted_text() const
{
  // TODO: implement - DONE
  if (this->m_message_type == MessageType::FAILED || this->m_message_type == MessageType::ERROR) {
    return this->m_args.at(0);
  }
  return "";
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

// Helper function to determine if an identifier string is valid - DONE
bool Message::is_valid_identifier(std::string id) const {

  if (!is_valid_length(id)) { return false; }

  if (!isalpha(id.at(0))) {
    return false; // immediately exit if does not start with a-z or A-Z
  }

  for(size_t i = 1; i < id.length(); ++i) {  // for each character
    if (!isalnum(id.at(i)) && id.at(i) != '_') { // must either be alphanumeric or underscore
      return false;
    }
  }
  return true;
}

// Helper function to determine if a value string is valid - DONE
bool Message::is_valid_value(std::string val) const {
  if (!is_valid_length(val)) { return false; }

  if (val.length() < 1) {
    return false; // must be 1+ characters
  } else {
    for (size_t i = 0; i < val.length(); ++i) {
      // immediately return false upon finding any whitespace
      if (isspace(val.at(i))) {
        return false;
      }
    }
    return true;
  }
}

// Helper function to determine if a quoted text string is valid
bool Message::is_valid_quoted_text(std::string qt) const {
  if (!is_valid_length(qt)) { return false; }

  for (size_t i = 0; i < qt.length(); ++i) {
    if (qt.at(i) == '\"') {
      return false;
    }
  }
  return true;
}

bool Message::is_valid_length(std::string s) const {
  if (s.length() >= MAX_ENCODED_LEN) { // encoded string is too big
    return false;
  } else {
    return true;
  }
}

bool Message::is_valid() const
{
  // TODO: implement - DONE
  // a collection of all the message types that will require parameter checking
  std::vector<MessageType> hasIdentifiers = {MessageType::LOGIN, MessageType::CREATE,
    MessageType::PUSH, MessageType::SET, MessageType::GET, MessageType::FAILED,
    MessageType::ERROR, MessageType::FAILED, MessageType::DATA};

  if (count(hasIdentifiers.begin(), hasIdentifiers.end(), this->m_message_type) >= 1) {

    // these types of requests only have a quoted text string that must be checked
    if (this->m_message_type == MessageType::ERROR || this->m_message_type == MessageType::FAILED) {
      return is_valid_quoted_text(this->m_args.at(0));
    
    // these types of requests only have a value that must be checked
    } else if (this->m_message_type == MessageType::PUSH || this->m_message_type == MessageType::DATA) {
      return is_valid_value(this->m_args.at(0));
    
    // LOGIN or CREATE request
    } else if (this->m_message_type == MessageType::LOGIN || this->m_message_type == MessageType::CREATE) {
      return is_valid_identifier(this->m_args.at(0));
    
    // must either be SET, GET which all are have an identifier, key
    } else {
      return is_valid_identifier(this->m_args.at(0)) && is_valid_identifier(this->m_args.at(1));
    }
  } else { // if the message does not contain any identifiers, the message is valid
    return true;
  }
  
}
