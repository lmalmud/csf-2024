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
	std::string loginName = ""; // login name of user
	bool inTransaction; // represents whether we are currently processing a transaction
  bool transactionFailed;
	ValueStack valStack;
  std::vector<Table*> accessed; // a vector of tables accessed during the transaction
  const int MAX_LINE_SIZE = 1024; // used for processing raw text from the server

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );
	
  /* Handles all message types. */
  void processMessage(Message msg);

  /* Handles the set command. */
  void handleSet(Message msg);

  /* Handle the get command. */
  void handleGet(Message msg);

  /* Pops two integers from the value stack and stores their
  values in the given variables. */
  void popTwo(int* right, int* left);

  /* Sends the given message back to the server. */
  void sendResponse(Message msg);

  void handleLogin(Message msg);
  void handleCreate(Message msg);
  void handlePush(Message msg);
  void handlePop(Message msg);
  void handleOpp(MessageType m);
  void handleBye();
  void handleBegin();
  void handleCommit();

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  // TODO: additional member functions
};

#endif // CLIENT_CONNECTION_H
