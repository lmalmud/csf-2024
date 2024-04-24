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
	tables = new std::vector<Table>();
}

Server::~Server()
{
  // TODO: implement
}

void Server::listen( const std::string &port )
{
  // TODO: implement - done??
	server_fd = open_listenfd(port.c_str());
	if (server_fd < 0) {
		//add proper exception later 
		throw CommException("Error opening server port");
	}
}

void Server::server_loop()
{
  // TODO: implement - done?

	int client_fd = Accept(server_fd, NULL, NULL);
	if (client_fd < 0) {
		throw CommException("Error accepting client connection");
	}

  ClientConnection *client = new ClientConnection( this, client_fd );
  pthread_t thr_id;
  if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ){
		log_error( "Could not create client thread" );
	}
    
}

void Server::create_table( const std::string &name ) {
	//do we need to lock tables?
	tables->push_back(Table(name));

}
Table *Server::find_table( const std::string &name ){
	//search through tables vector and find find table with this name
	//mutex?
	//i think we dont need one bc its just looking and not changing anything but idk
}


void *Server::client_worker( void *arg )
{
  // TODO: implement

  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:

  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;

}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions
