#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
  // TODO: initialize member variable(s) (if necessary)
{
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  // TODO: implement - DONE
  return v_stack.empty();
}

void ValueStack::push( const std::string &value )
{
  // TODO: implement - DONE
  v_stack.push_back(value);
}

std::string ValueStack::get_top() const
{
  // TODO: implement - DONE
  if (this->is_empty()) {
    throw OperationException("cannot get_top() since stack is empty.");
  } else {
    return v_stack.at(v_stack.size() - 1);
  }
}

void ValueStack::pop()
{
  // TODO: implement - DONE
  if (this->is_empty()) {
    throw OperationException("cannot pop() since stack is empty.");
  } else {
    v_stack.erase(v_stack.end() - 1);
  }
}
