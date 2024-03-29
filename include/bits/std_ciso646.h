// ciso646 -*- C++ -*-

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
// ISO C++ 14882: <ciso646>
//

#ifndef _CPP_ISO646
#define _CPP_ISO646	1

namespace std {

  // 2.11 Keywords
  // alternative representations
  // and, and_eq, bitand, bitor, compl, not, not_eq, or, or_eq, xor, xor_eq

#ifndef __cplusplus
#define and	&&
#define and_eq	&=
#define bitand	&
#define bitor	|
#define compl	~
#define not	!
#define not_eq	!=
#define or	||
#define or_eq	|=
#define xor	^
#define xor_eq	^=
#endif

#if 0
  template<typename _T1, typename _T2>
    inline bool
    operator and(const _T1& __lhs, const _T2& __rhs) 
    { return (static_cast<bool>(__lhs) && static_cast<bool>(__rhs)); }

  template<typename _T1, typename _T2>
    inline bool
    operator or(const _T1& __lhs, const _T2& __rhs) 
    { return (static_cast<bool>(__lhs) || static_cast<bool>(__rhs)); }
#endif

} // namespace std

#endif // _CPP_ISO646
