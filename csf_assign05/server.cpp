#include <exception>
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"
#include "table.h"

Server::Server()
  // TODO: initialize member variables
{
  // TODO: implement
	//tables = new std::vector<Table*>();
}

Server::~Server()
{
  // TODO: implement
}

void Server::listen( const std::string &port )
{
  // TODO: implement - DONE
	server_fd = open_listenfd(port.c_str());
	if (server_fd < 0) {
    // FIXME: add proper exception later 
    // is this not the proper exception?
		throw CommException("Error opening server port");
	}
}

void Server::server_loop()
{
  // TODO: implement

  // this function is called after listen
  while (1) {
    int client_fd;

    do {
      client_fd = Accept(server_fd, NULL, NULL);
    } while (client_fd < 0 && errno == EINTR);
    if (client_fd < 0) {
		  throw CommException("Error accepting client connection");
	  }

    // note that we dynamically allocate new memory for the ClientConnection object
    ClientConnection *client = new ClientConnection(this, client_fd);
    pthread_t thr_id;
    if (pthread_create(&thr_id, NULL, client_worker, client) != 0 ) {
      // sets up the thread to start in the client_worker function
		  // log_error( "Could not create client thread" );
      throw CommException("Could not create client thread"); // NOTE: switched to fatal exception
	  }
  }
}

void Server::create_table( const std::string &name ) {
	// NOTE: we do not need to lock the table since it has just been created
  // and is now ready to be modified
	
  Table* new_table = new Table(name);
	tables.push_back(new_table);

}

Table *Server::find_table( const std::string &name ){
  // NOTE: this is used to access tables from individual ClientConnections
  for (auto t = tables.begin(); t != tables.end(); ++t) {
    if ((*t)->get_name() == name) { // NOTE: *t is where the iterator points to
      return *t; // *t is the address of the table
    }
  }
  return NULL; // table could not be found
}


void *Server::client_worker( void *arg )
{
  // TODO: implement

  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:
	pthread_detach(pthread_self());
  std::unique_ptr<ClientConnection> client(static_cast<ClientConnection *>(arg));
	try{
  client->chat_with_client();
	} catch (std::exception ex) {
	}
  return nullptr;

}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}