// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_VORBISTAG_H
#define _GSTREAMERMM_VORBISTAG_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/vorbisparse.h>
#include <gstreamermm/tagsetter.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstVorbisTag GstVorbisTag;
typedef struct _GstVorbisTagClass GstVorbisTagClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class VorbisTag_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the vorbistag plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class VorbisTag
: public Gst::VorbisParse, public Gst::TagSetter
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef VorbisTag CppObjectType;
  typedef VorbisTag_Class CppClassType;
  typedef GstVorbisTag BaseObjectType;
  typedef GstVorbisTagClass BaseClassType;

private:  friend class VorbisTag_Class;
  static CppClassType vorbistag_class_;

private:
  // noncopyable
  VorbisTag(const VorbisTag&);
  VorbisTag& operator=(const VorbisTag&);

protected:
  explicit VorbisTag(const Glib::ConstructParams& construct_params);
  explicit VorbisTag(GstVorbisTag* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~VorbisTag();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstVorbisTag*       gobj()       { return reinterpret_cast<GstVorbisTag*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstVorbisTag* gobj() const { return reinterpret_cast<GstVorbisTag*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstVorbisTag* gobj_copy();

private:

  
protected:
  VorbisTag();
  explicit VorbisTag(const Glib::ustring& name);

public:
  /** Creates a new vorbistag plugin with a unique name.
   */
  
  static Glib::RefPtr<VorbisTag> create();


  /** Creates a new vorbistag plugin with the given name.
   */
  
  static Glib::RefPtr<VorbisTag> create(const Glib::ustring& name);


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
   * @relates Gst::VorbisTag
   */
  Glib::RefPtr<Gst::VorbisTag> wrap(GstVorbisTag* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_VORBISTAG_H */

