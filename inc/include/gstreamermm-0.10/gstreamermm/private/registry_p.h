// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_REGISTRY_P_H
#define _GSTREAMERMM_REGISTRY_P_H


#include <glibmm/class.h>

namespace Gst
{

class Registry_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Registry CppObjectType;
  typedef GstRegistry BaseObjectType;
  typedef GstRegistryClass BaseClassType;
  typedef Gst::Object_Class CppClassParent;
  typedef GstObjectClass BaseClassParent;

  friend class Registry;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();


  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.
  static void feature_added_callback(GstRegistry* self, GstPluginFeature* p0);
  static void plugin_added_callback(GstRegistry* self, GstPlugin* p0);

  //Callbacks (virtual functions):
};


} // namespace Gst

#include <gstreamermm/private/object_p.h>


#endif /* _GSTREAMERMM_REGISTRY_P_H */
