// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_VOLUME_H
#define _GSTREAMERMM_VOLUME_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/audiofilter.h>
#include <gstreamermm/interface.h>
#include <gstreamermm/mixer.h>
#include <gstreamermm/streamvolume.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstVolume GstVolume;
typedef struct _GstVolumeClass GstVolumeClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class Volume_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the volume plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class Volume
: public Gst::AudioFilter, public Gst::Interface, public Gst::Mixer, public Gst::StreamVolume
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef Volume CppObjectType;
  typedef Volume_Class CppClassType;
  typedef GstVolume BaseObjectType;
  typedef GstVolumeClass BaseClassType;

private:  friend class Volume_Class;
  static CppClassType volume_class_;

private:
  // noncopyable
  Volume(const Volume&);
  Volume& operator=(const Volume&);

protected:
  explicit Volume(const Glib::ConstructParams& construct_params);
  explicit Volume(GstVolume* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~Volume();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstVolume*       gobj()       { return reinterpret_cast<GstVolume*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstVolume* gobj() const { return reinterpret_cast<GstVolume*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstVolume* gobj_copy();

private:

  
protected:
  Volume();
  explicit Volume(const Glib::ustring& name);

public:
  /** Creates a new volume plugin with a unique name.
   */
  
  static Glib::RefPtr<Volume> create();


  /** Creates a new volume plugin with the given name.
   */
  
  static Glib::RefPtr<Volume> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** mute channel.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_mute() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** mute channel.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_mute() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** volume factor, 1.0=100%.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<double> property_volume() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** volume factor, 1.0=100%.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<double> property_volume() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


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
   * @relates Gst::Volume
   */
  Glib::RefPtr<Gst::Volume> wrap(GstVolume* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_VOLUME_H */
