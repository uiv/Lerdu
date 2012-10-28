// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_FUNNEL_H
#define _GSTREAMERMM_FUNNEL_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/element.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstFunnel GstFunnel;
typedef struct _GstFunnelClass GstFunnelClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class Funnel_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the funnel plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class Funnel
: public Gst::Element
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef Funnel CppObjectType;
  typedef Funnel_Class CppClassType;
  typedef GstFunnel BaseObjectType;
  typedef GstFunnelClass BaseClassType;

private:  friend class Funnel_Class;
  static CppClassType funnel_class_;

private:
  // noncopyable
  Funnel(const Funnel&);
  Funnel& operator=(const Funnel&);

protected:
  explicit Funnel(const Glib::ConstructParams& construct_params);
  explicit Funnel(GstFunnel* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~Funnel();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstFunnel*       gobj()       { return reinterpret_cast<GstFunnel*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstFunnel* gobj() const { return reinterpret_cast<GstFunnel*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstFunnel* gobj_copy();

private:

  
protected:
  Funnel();
  explicit Funnel(const Glib::ustring& name);

public:
  /** Creates a new funnel plugin with a unique name.
   */
  
  static Glib::RefPtr<Funnel> create();


  /** Creates a new funnel plugin with the given name.
   */
  
  static Glib::RefPtr<Funnel> create(const Glib::ustring& name);


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


};

} // namespace Gst


namespace Glib
{
  /** A Glib::wrap() method for this object.
   * 
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Gst::Funnel
   */
  Glib::RefPtr<Gst::Funnel> wrap(GstFunnel* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_FUNNEL_H */
