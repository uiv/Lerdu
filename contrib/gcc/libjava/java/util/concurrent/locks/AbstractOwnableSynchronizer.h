
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __java_util_concurrent_locks_AbstractOwnableSynchronizer__
#define __java_util_concurrent_locks_AbstractOwnableSynchronizer__

#pragma interface

#include <java/lang/Object.h>

class java::util::concurrent::locks::AbstractOwnableSynchronizer : public ::java::lang::Object
{

public: // actually protected
  AbstractOwnableSynchronizer();
  virtual void setExclusiveOwnerThread(::java::lang::Thread *);
  virtual ::java::lang::Thread * getExclusiveOwnerThread();
private:
  static const jlong serialVersionUID = 3737899427754241961LL;
  ::java::lang::Thread * __attribute__((aligned(__alignof__( ::java::lang::Object)))) exclusiveOwnerThread;
public:
  static ::java::lang::Class class$;
};

#endif // __java_util_concurrent_locks_AbstractOwnableSynchronizer__
