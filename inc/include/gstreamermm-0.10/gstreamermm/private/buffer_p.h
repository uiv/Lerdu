// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_BUFFER_P_H
#define _GSTREAMERMM_BUFFER_P_H


#include <glibmm/class.h>

namespace Gst
{

class Buffer_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Buffer CppObjectType;
  typedef GstBuffer BaseObjectType;
  typedef GstBufferClass BaseClassType;
  typedef Gst::MiniObject_Class CppClassParent;
  typedef GstMiniObjectClass BaseClassParent;

  friend class Buffer;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();

  static void class_init_function(void* g_class, void* class_data);

  static Gst::MiniObject* wrap_new(GstMiniObject*);

protected:

  //Callbacks (virtual functions):
};
;

} // namespace Gst

#include <gstreamermm/private/miniobject_p.h>


#endif /* _GSTREAMERMM_BUFFER_P_H */
