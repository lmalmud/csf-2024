#include <cassert>
#include <iostream>
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
  t_proposed[key] = value;
}

std::string Table::get( const std::string &key )
{
  // TODO: implement
  return t_proposed.at(key);
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
  // TODO: implement - DONE
  // committed is set to proposed (to commit the changes)
  // previous is set to committed (in case we need to roll back in the future)
  t_committed.clear();
  t_previous.clear();
  for (auto s = t_proposed.begin(); s != t_proposed.end(); s++) {
    t_committed[s->first] = s->second;
    t_previous[s->first] = s->second;
  }

  t_proposed.clear(); // so that all tentative changes are in sync with the current ones
  for (auto s = t_committed.begin(); s != t_committed.end(); ++s) {
    t_proposed[s->first] = s->second;
  }
  
}

void Table::rollback_changes()
{
  // TODO: implement - DONE
  // committed and proposed is set to previous
  t_committed.clear();
  t_proposed.clear();
  for (auto s = t_previous.begin(); s != t_previous.end(); s++) {
    t_committed[s->first] = s->second;
    t_proposed[s->first] = s->second;
  }

}


void Table::printTables() const {
  std::cout << "COMMITTED: " << std::endl;
  for (auto s = t_committed.begin(); s != t_committed.end(); ++s) {
    std::cout << s->first << ": " << s->second << std::endl;
  }

  std::cout << "PROPOSED: " << std::endl;
  for (auto s = t_proposed.begin(); s != t_proposed.end(); ++s) {
    std::cout << s->first << ": " << s->second << std::endl;
  }

  std::cout << "PREVIOUS: " << std::endl;
  for (auto s = t_previous.begin(); s != t_previous.end(); ++s) {
    std::cout << s->first << ": " << s->second << std::endl;
  }
}