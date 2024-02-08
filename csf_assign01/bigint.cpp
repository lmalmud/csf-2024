#include <cassert>
#include "bigint.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

BigInt::BigInt() : neg(false) {
  // TODO: initialize member variables - DONE
  this->val.push_back(0UL);
}

BigInt::BigInt(uint64_t val, bool negative) : neg(negative) {
  // TODO: initialize member variables - DONE
  this->val.push_back(val);
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative) : neg(negative)
  // TODO: initialize member variables - DONE
{
  this->val.clear(); // must start value vector as empty
  for (auto itr = vals.begin(); itr != vals.end(); ++itr) {
    this->val.push_back(*itr); // individually append each new value
  }
}

BigInt::BigInt(const BigInt &other)
  // TODO: initialize member variables - DONE
{
  this->val.clear(); // if not cleared, then there will be extra values
  this->neg = other.neg; // might need to create a getter
  for (size_t i = 0; i < other.val.size(); ++i) {
    this->val.push_back(other.val.at(i));
  }
}

BigInt::~BigInt()
{
  // no dynamically allocated memory
  this->val.clear();
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  // TODO: implement - DONE
  this->neg = rhs.neg; // might need to create a getter
  this->val.clear(); // want to overwritte any previous value
  for (size_t i = 0; i < rhs.val.size(); ++i) {
    this->val.push_back(rhs.val.at(i));
  }
  return *this;
}

bool BigInt::is_negative() const
{
  // TODO: implement - DONE
  return neg;
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  // TODO: implement - DONE
  return this->val;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  // TODO: implement - DONE
  if (index >= this->val.size()) {
    return 0; // returns 0 if outside of bounds
  } else {
    return this->val.at(index);
  }
}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement - DONE

  // something plus 0
  if (rhs.is_zero()) {
    return *this;
  } else if (this->is_zero()) {
    return rhs;
  
  // handle nonzero addition
  } else {
    BigInt result;
    result.val.clear(); // do not want any initial values
    
    // result nonnegative if both operands are also nonnegative
    if (!this->is_negative() && !rhs.is_negative()) {
      result = add_magnitudes(*this, rhs);
  
    // adding two negative values
    } else if (this->is_negative() && rhs.is_negative()) {
    
      // add_magnitudes automatically assumes both are positive
      result = add_magnitudes(*this, rhs);
      result.neg = true;

    // mixed sign addition  
    } else {

      // we only want to compare magnitudes, not sign
      BigInt abs_this = *this;
      BigInt abs_rhs = rhs;
      abs_this.neg = false;
      abs_rhs.neg = false;

      if (abs_this > abs_rhs) {
        result = subtract_magnitudes(abs_this, abs_rhs);
        result.neg = this->neg; // sign matches that of the larger operand in magnitude
      } else {
        result = subtract_magnitudes(abs_rhs, abs_this);
        result.neg = rhs.neg;
      }
    }

    std::vector<uint64_t>::iterator end = result.val.end() - 1;
    while (result.val.size() > 1 && *end == 0UL) { // if there is a zero
      result.val.erase(end); // get rid of the value at the current position
      end--; // vector is now one item shorter
    }
    if (result.is_zero()) { result.neg = false; }
    return result;
  }
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement - DONE
  // Hint: a - b could be computed as a + -b
  BigInt result = *this + (-rhs);
  return result;
}

BigInt BigInt::operator-() const
{
  // TODO: implement - DONE
  BigInt result = BigInt(*this);
  result.neg = !this->neg; 

  // handle special zero case
  if (this->is_zero()) { result.neg = !result.neg; }
  return result;
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement - DONE
  // note that we are inquiring about bits not bytes
  if (n >= 64*this->val.size()) { // if we are outside the scope
    return 0;
  } else {
    int index = n / 64; // get the index of the appropriate element in the vector
    int shift = n % 64; // the bit in the appropriate index we want to access
    return (this->val.at(index) >> shift & 1UL);
  }
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement - DONE
  if (this->is_negative()) { // throws an error if negative
    throw std::invalid_argument("cannot left shift negative values");
  }

  BigInt result = *this; // start with the same value using copy constructor

  int full_shifts = n / 64; // floor(n / 64)

  // prepend extra zeros
  for (int i = 0; i < full_shifts; ++i) {
    result.val.insert(result.val.begin(), 0UL);
  }

  // we will shift by 1 as many times as needed
  bool must_prepend = false;
  for (int i = 0; i < (int) n % 64; ++i) { // for each one-bit shift
    for (int j = (int) result.val.size() * 64 - 1; j >= 0; --j) { // for each bit in all of the uint_64s
      if (j == 0) {
        result.set_nth_bit(j, 0); // the LSB is always a zero
      } else {
        if (j == (int) result.val.size() * 64 - 1 && result.is_bit_set(j)) {
        // if the most significant bit is a one,
        must_prepend = true; // then the string becomes longer
        }
        result.set_nth_bit(j, result.is_bit_set(j - 1));
      }
    }

    // could occur after shifting just one bit if MSB is a one and
    // padded with no zeros
    if (must_prepend) { // like multiplying 2^63 by 2, requires a new uint64_t
      result.val.push_back(1UL);
      must_prepend = false;
    }
  }

  return result;
}

// sets the nth bit to the value specefied
void BigInt::set_nth_bit(unsigned n, bool value) {

  int index = n / 64; // get the index of the appropriate element in the vector
  int shift = n % 64; // the element of the vector that we need

  if (value) { // setting to a 1
    this->val.at(index) |= (1UL << shift);
  } else { // setting to a 0
    this->val.at(index) &= ~(1UL << shift);
  }

}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement - DONE

  BigInt result; // result must start as empty
  result.val.clear();

  // take the absolute values of each operand
  // because left shift will not work on negative values
  BigInt abs_rhs = rhs;
  abs_rhs.neg = false;

  // for each of the bits in this number
  for (int i = 0; i < (int) this->val.size() * 64; ++i) {
    if (this->is_bit_set(i)) {
      BigInt temp = (abs_rhs << i); // same as multiplying by 2^i
      result = add_magnitudes(result, temp);
    }
    
  }

  // fix the sign
  if ((this->is_negative() && !rhs.is_negative()) || (!this->is_negative() && rhs.is_negative())) {
    result.neg = true;
  }
  if (this->is_zero() || rhs.is_zero()) { // zero case handled separately
    result.neg = false;
  }

  return result;
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
  BigInt result;
  return result;
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement - DONE
  if (this->is_zero() && rhs.is_zero()) {return 0;} // both zero
  else if (this->is_zero()) { // 0 compare something
    if (rhs.is_negative()) {return 1;}
    else {return -1;}
  }
  else if (rhs.is_zero()) { // something compare 0
    if (this->is_negative()) {return -1; }
    else {return 1;}
  }
  else if (this->is_negative() && !rhs.is_negative()) {return -1;} // lhs negative, rhs nonnegative
  else if (!this->is_negative() && rhs.is_negative()) {return 1;} // lhs nonnegative, rhs negative
  else { // signs are the same
    if (this->is_negative() && rhs.is_negative()) { // both negative
      return -1 * compare_magnitudes(*this, rhs); // reverse the result
    } else { // both positive
      return compare_magnitudes(*this, rhs); 
    }
  }
  return 0;
}

std::string BigInt::to_hex() const
{
  // TODO: implement - DONE

  // convert each number to decimal
  std::stringstream result("");
  for (std::vector<uint64_t>::const_reverse_iterator itr = this->val.crbegin(); itr != this->val.crend(); ++itr) {
    result << std::hex << std::setfill('0') << std::setw(16);
    result << *itr;
  }

  std::string s = "";
  // remove leading zeros
  bool found_nonzero = false;
  size_t i = 0;
  while (i < result.str().length()) {
    if (result.str().at(i) != '0') { found_nonzero = true; } // mark first nonzero character
    if (found_nonzero) { s += result.str().at(i); } // if there were no leading zeros, then append
    ++i;
  }

  if (s == "") {s = "0";} // if the value is zero

  // prepend negative sign if necessary
  if (this->is_negative()) {
    return "-" + s;
  } else {
    return s;
  }
}

std::string BigInt::to_dec() const
{
  // TODO: implement
  return "";
}

bool BigInt::is_zero() const {
  return (this->val.size() <= 1 && this->val.at(0) == 0);
}

// assumes both inputs are positive
BigInt BigInt::add_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  // the get_bit_at function returns 0 if a bit does not exist
  // so we will loop the numer of times as the integer with a larger magnitude
  int maxBit = std::max(lhs.val.size(), rhs.val.size());
  
  BigInt result;
  result.val.clear(); // want to start from scratch - no 0th element
  
  bool carry = false; // the value previously being carried over
  
  for (int i = 0; i < maxBit; ++i) { // for each of the elements in the larger BigInt
    uint64_t sum = lhs.get_bits(i) + rhs.get_bits(i);
    sum += carry; // account for carry
    
    if (sum < lhs.get_bits(i)) { // overflow occured
      carry = true;
    } else {
      carry = false; // need to ensure that no carry occurs unless specefied
    }
    result.val.push_back(sum); // adds the current part
    
    if (i == maxBit - 1 && carry) { // the very last digit results in a carry
      result.val.push_back(1UL);
    }

  }

  return result;
}

// assuming that lhs > rhs
BigInt BigInt::subtract_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  // remove the 0th element
  BigInt result;
  result.val.clear(); 
  
  bool borrowed = false; // stores whether or not the current value was taken from
  for (size_t i = 0; i < lhs.get_bit_vector().size(); ++i) { // note that we know that lhs has a greater length

    uint64_t difference;
    // two cases that we need to borrow from the next column: lhs < rhs

    // if we need to borrow since the current value is smaller than the value being subtracted
    // there are two cases because lhs - borrowed will cause an overflow if lhs = 0
    if (lhs.get_bits(i) - borrowed < rhs.get_bits(i) || lhs.get_bits(i) < rhs.get_bits(i)) { // worried that this could cause an overflow

      difference = 0xFFFFFFFFFFFFFFFFUL - rhs.get_bits(i) + lhs.get_bits(i) + 1 - borrowed;
      borrowed = true;
    
    // a zero that was borrowed from so we need to borrow again
    } else if (lhs.get_bits(i) == 0 && borrowed) {
      borrowed = true; 
    
      // need to handle the case that it is a zero that was borrowed from and more is
      // being subtracted from it
      if (rhs.get_bits(i) != 0) {
        difference = 0xFFFFFFFFFFFFFFFFUL - rhs.get_bits(i) + 1;
      }
      
    // there is a carry-over-borrow from the previous subtraction
    } else if (borrowed) {
      difference = lhs.get_bits(i) - rhs.get_bits(i) - borrowed;
      borrowed = false;

    } else { // we do not need to borrow
      borrowed = false;
      difference = lhs.get_bits(i) - rhs.get_bits(i); // regular
    }

    result.val.push_back(difference);

  }

  return result;
}

std::ostream& operator<<(std::ostream& os, BigInt& rhs) {
  // for each of the uint64
  for (int i = rhs.get_bit_vector().size() - 1; i >= 0 ; --i) {
    for (int j = 63; j >= 0; --j) { // for each of the bits in one uint64
      os << rhs.is_bit_set(64*i + j); // output whether that bit is set
    }
    os << " "; // print spaces between every 64 bits
  }
  return os;
}

//!   - negative if lhs < rhs
//!   - 0 if lhs < rhs
//!   - positive if lhs > rhs
int BigInt::compare_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  // lhs has more components
  if (lhs.get_bit_vector().size() > rhs.get_bit_vector().size()) {
    return 1;
  
  // rhs has more components
  } else if (lhs.get_bit_vector().size() < rhs.get_bit_vector().size()) {
    return -1;
  
  // there are the same number of elements so we must compare
  } else {
    int bigger = 0; // store the result of which is larger
    int index = lhs.get_bit_vector().size() - 1; // start at most significant bit
    while (index >= 0 && bigger == 0) { // will exit the loop as soon as there is a mismatch
      if (lhs.get_bits(index) > rhs.get_bits(index)) {
        bigger = 1;
      } else if (lhs.get_bits(index) < rhs.get_bits(index)) {
        bigger = -1;
      }
      index--;
    }
    return bigger;
  }
}