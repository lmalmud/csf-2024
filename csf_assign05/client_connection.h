#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"
#include <vector>
#include "message.h"
#include "value_stack.h"
class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server; // pointer to parent server
  int m_client_fd; // file descriptor of current connection
  rio_t m_fdbuf;
	std::string loginName; // login name of user
	bool inTransaction; // represents whether we are currently processing a transaction
	std::vector<Message*> transaction; // pointers to all messages in the processed transaction
	ValueStack valStack;
  std::vector<Table*> locked; // a vector of currently locked tables
  const int MAX_LINE_SIZE = 1024;

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );
	void processMessage(Message msg);
	void processTransaction();

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  // TODO: additional member functions
};

#endif // CLIENT_CONNECTION_H
