#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name )
  // TODO: initialize additional member variables
{
  // TODO: implement
  pthread_mutex_init(&my_mutex, NULL); // initialize mutex
}

Table::~Table()
{
  // TODO: implement
  pthread_mutex_destroy(&my_mutex); // destroy mutex when table is destroied
}

void Table::lock()
{
  // TODO: implement - DONE
  pthread_mutex_lock(&my_mutex);
}

void Table::unlock()
{
  // TODO: implement - DONE
  pthread_mutex_unlock(&my_mutex);
}

bool Table::trylock()
{
  // TODO: implement - DONE
  return pthread_mutex_trylock(&my_mutex);
}

void Table::set( const std::string &key, const std::string &value )
{
  // TODO: implement
  t_proposed[key] = value;
}

std::string Table::get( const std::string &key )
{
  // TODO: implement
  return t_proposed[key];
}

bool Table::has_key( const std::string &key )
{
  // TODO: implement
  if (t_proposed.find(key) == t_proposed.end()) {
    return false;
  } else {
    return true;
  }
}

void Table::commit_changes()
{
  // TODO: implement
    // committed is set to proposed (to commit the changes)
  // previous is set to committed (in case we need to roll back in the future)
}

void Table::rollback_changes()
{
  // TODO: implement
  // committed is set to previous, abandoning the proposed changes
}
