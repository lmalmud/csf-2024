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
	
	std::vector<Table*> tables; // a vector of pointers to tables
  // NOTE: we want to store the address of the tables here so that
  // other functions are able to directly modify them
	int server_fd;

  // copy constructor and assignment operator are prohibited
  Server(const Server &);
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  /* First function called to set up server_fd. */
  void listen( const std::string &port );

  /* Implementation of the main server loop. */
  void server_loop();

  /* Function called at the beginning of each thread. */
  static void *client_worker( void *arg );

  /* Prints an string to std::cerr. */
  void log_error( const std::string &what );

  /* */
  void create_table( const std::string &name );

  /* */
  Table *find_table( const std::string &name );

};


#endif // SERVER_H
