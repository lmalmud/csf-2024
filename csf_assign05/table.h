#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <string>
#include <pthread.h>

class Table {
private:
  std::string m_name;
  // TODO: add member variables
  pthread_mutex_t my_mutex;

  // the current value of the talbe
  std::map<std::string, std::string> t_committed;

  // the working value of the table
  // treated as the active table for get, has_key, etc.
  std::map<std::string, std::string> t_proposed;
  
  // the previous state of the table- includes ALL key, value pairs
  std::map<std::string, std::string> t_previous;

  // copy constructor and assignment operator are prohibited
  Table( const Table & );
  Table &operator=( const Table & );

  void printTables() const;

public:
  Table( const std::string &name );
  ~Table();

  std::string get_name() const { return m_name; }

  void lock();
  void unlock();
  bool trylock();

  // Note: these functions should only be called while the
  // table's lock is held!
  void set( const std::string &key, const std::string &value );
  bool has_key( const std::string &key );
  std::string get( const std::string &key );
  void commit_changes();
  void rollback_changes();
};

#endif // TABLE_H
