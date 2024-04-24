#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"
#include "value_stack.h"
#include <vector>


class Server {
private:
  // TODO: add member variables
	
	std::vector<Table>* tables;
	int server_fd;

  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );
  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );

  // TODO: add member functions

  void create_table( const std::string &name );
  Table *find_table( const std::string &name );

};


#endif // SERVER_H
