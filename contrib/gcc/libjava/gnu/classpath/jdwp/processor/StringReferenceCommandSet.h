
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __gnu_classpath_jdwp_processor_StringReferenceCommandSet__
#define __gnu_classpath_jdwp_processor_StringReferenceCommandSet__

#pragma interface

#include <gnu/classpath/jdwp/processor/CommandSet.h>
extern "Java"
{
  namespace gnu
  {
    namespace classpath
    {
      namespace jdwp
      {
        namespace processor
        {
            class StringReferenceCommandSet;
        }
      }
    }
  }
  namespace java
  {
    namespace nio
    {
        class ByteBuffer;
    }
  }
}

class gnu::classpath::jdwp::processor::StringReferenceCommandSet : public ::gnu::classpath::jdwp::processor::CommandSet
{

public:
  StringReferenceCommandSet();
  virtual jboolean runCommand(::java::nio::ByteBuffer *, ::java::io::DataOutputStream *, jbyte);
private:
  void executeValue(::java::nio::ByteBuffer *, ::java::io::DataOutputStream *);
public:
  static ::java::lang::Class class$;
};

#endif // __gnu_classpath_jdwp_processor_StringReferenceCommandSet__