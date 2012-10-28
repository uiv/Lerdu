// Components for manipulating sequences of characters -*- C++ -*-

// Copyright (C) 1997-1999 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

//
// ISO C++ 14882: 21  Strings library
//

// Written by Jason Merrill based upon the specification by Takanori Adachi
// in ANSI X3J16/94-0013R2.

#include <bits/std_string.h>
#include <bits/std_algorithm.h>
#include <bits/std_istream.h>
#include <bits/std_ostream.h>

// NB: unnecessary if the .h headers include these
#ifndef  _GLIBCPP_FULLY_COMPLIANT_HEADERS
#include <bits/istream.tcc>
#include <bits/ostream.tcc>
#endif

// Instantiation configuration.
#ifndef C
# define C char
# define _GLIBCPP_INSTANTIATING_CHAR 1
#endif

namespace std 
{
  typedef basic_string<C> S;

// Do instantiate.
#ifdef TRAITS
  template class char_traits<C>;
#endif

#ifdef MAIN
  template class basic_string<C>;

  // Make sure string::iterators have binary operators.
  template bool operator==(const S::iterator&, const S::iterator&);
  template bool operator!=(const S::iterator&, const S::iterator&);
  template bool operator<(const S::iterator&, const S::iterator&);
  template bool operator<=(const S::iterator&, const S::iterator&);
  template bool operator>(const S::iterator&, const S::iterator&);
  template bool operator>=(const S::iterator&, const S::iterator&);
  template 
    S::iterator
    operator+(S::iterator::difference_type, const S::iterator&);

  // Make sure string::const_iterators have binary operators.
  template bool operator==(const S::const_iterator&, const S::const_iterator&);
  template bool operator!=(const S::const_iterator&, const S::const_iterator&);
  template bool operator<(const S::const_iterator&, const S::const_iterator&);
  template bool operator<=(const S::const_iterator&, const S::const_iterator&);
  template bool operator>(const S::const_iterator&, const S::const_iterator&);
  template bool operator>=(const S::const_iterator&, const S::const_iterator&);
  template 
    S::const_iterator
    operator+(S::const_iterator::difference_type, const S::const_iterator&);

  // These member templates have to be explicitly instantiated.
  template 
    S::basic_string
    (C*, C*, const allocator<C>&);

  template 
    S::basic_string
    (S::iterator, S::iterator, const allocator<C>&);

  template 
    S& 
    S::append<S::iterator>
    (S::iterator, S::iterator);

  template 
    S& 
    S::assign<S::iterator>
    (S::iterator, S::iterator);

  template 
    void
    S::insert<S::iterator> //c*
    (S::iterator, S::iterator, S::iterator); //it, c+, c+ and temptype = char*

  template 
    S&
    S::replace<S::iterator> // c*
    (S::iterator, S::iterator, S::iterator, S::iterator); //it, it, c+, c+ 

  template 
    S& 
    S::_M_replace<S::iterator>
    (S::iterator, S::iterator, S::iterator, S::iterator, forward_iterator_tag);

  template 
    S& 
    S::_M_replace<S::const_iterator>
    (S::iterator, S::iterator, 
     S::const_iterator, S::const_iterator, forward_iterator_tag);

  template 
    S& 
    S::_M_replace<C*>
    (S::iterator, S::iterator, C*, C*, forward_iterator_tag);
  
  template 
    S& 
    S::_M_replace<const C*>
    (S::iterator, S::iterator, const C*, const C*, forward_iterator_tag);

  template 
    C* 
    S::_S_construct<S::iterator>
    (S::iterator, S::iterator, const allocator<C>&);
  
  template 
    C* 
    S::_S_construct<S::iterator>
    (S::iterator, S::iterator, const allocator<C>&, forward_iterator_tag);

  template 
    C* 
    S::_S_construct<C*>
    (C*, C*, const allocator<C>&, forward_iterator_tag);

  template 
    C* 
    S::_S_construct<const C*>
    (const C*, const C*, const allocator<C>&, forward_iterator_tag);

  // These members are explicitly specialized, and can only be in one
  // translation unit or else we get multiple copies. . . 
#if _GLIBCPP_INSTANTIATING_CHAR
  template<>
    const char* 
    string::_S_find(const char* __beg, const char* __end, char __c)
    { 
      char* __retval = strchr(__beg, __c); 
      return (__retval ? __retval : __end);
    }
#else
  template<>
    const wchar_t* 
    wstring::_S_find(const wchar_t* __beg, const wchar_t* __end, wchar_t __c)
    {
      return find_if(__beg, __end, 
		     _Char_traits_match<wchar_t, traits_type>(__c));
    }
#endif
#endif // MAIN

#ifdef ADDSS
template S operator+(const S&, const S&);
#endif
#ifdef ADDPS
template S operator+(const C*, const S&);
#endif
#ifdef ADDCS
template S operator+(C, const S&);
#endif
#ifdef ADDSP
template S operator+(const S&, const C*);
#endif
#ifdef ADDSC
template S operator+(const S&, C);
#endif
#ifdef EQSS
template bool operator==(const S&, const S&);
#endif
#ifdef EQPS
template bool operator==(const C*, const S&);
#endif
#ifdef EQSP
template bool operator==(const S&, const C*);
#endif
#ifdef NESS
template bool operator!=(const S&, const S&);
#endif
#ifdef NEPS
template bool operator!=(const C*, const S&);
#endif
#ifdef NESP
template bool operator!=(const S&, const C*);
#endif
#ifdef LTSS
template bool operator<(const S&, const S&);
#endif
#ifdef LTPS
template bool operator<(const C*, const S&);
#endif
#ifdef LTSP
template bool operator<(const S&, const C*);
#endif
#ifdef GTSS
template bool operator>(const S&, const S&);
#endif
#ifdef GTPS
template bool operator>(const C*, const S&);
#endif
#ifdef GTSP
template bool operator>(const S&, const C*);
#endif
#ifdef LESS
template bool operator<=(const S&, const S&);
#endif
#ifdef LEPS
template bool operator<=(const C*, const S&);
#endif
#ifdef LESP
template bool operator<=(const S&, const C*);
#endif
#ifdef GESS
template bool operator>=(const S&, const S&);
#endif
#ifdef GEPS
template bool operator>=(const C*, const S&);
#endif
#ifdef GESP
template bool operator>=(const S&, const C*);
#endif
#ifdef EXTRACT
template basic_istream<C>& operator>>(basic_istream<C>&, S&);
#endif
#ifdef INSERT
template basic_ostream<C>& operator<<(basic_ostream<C>&, const S&);
#endif
#ifdef GETLINE
template basic_istream<C>& getline(basic_istream<C>&, S&, C);
template basic_istream<C>& getline(basic_istream<C>&, S&);
#endif
#ifdef SCOPY
template void _S_string_copy(const S&, C*, allocator<C>::size_type);
#endif

} // std















