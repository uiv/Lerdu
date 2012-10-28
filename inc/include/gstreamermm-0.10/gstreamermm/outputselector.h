// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_OUTPUTSELECTOR_H
#define _GSTREAMERMM_OUTPUTSELECTOR_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/element.h>
#include <gstreamermm/pad.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstOutputSelector GstOutputSelector;
typedef struct _GstOutputSelectorClass GstOutputSelectorClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class OutputSelector_Class; } // namespace Gst
namespace Gst
{

/** @addtogroup gstreamermmEnums gstreamermm Enums and Flags */

/**
 * @ingroup gstreamermmEnums
 */
enum OutputSelectorPadNegotiationMode
{
  OUTPUT_SELECTOR_PAD_NEGOTIATION_MODE_NONE,
  OUTPUT_SELECTOR_PAD_NEGOTIATION_MODE_ALL,
  OUTPUT_SELECTOR_PAD_NEGOTIATION_MODE_ACTIVE
};

} // namespace Gst


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gst::OutputSelectorPadNegotiationMode> : public Glib::Value_Enum<Gst::OutputSelectorPadNegotiationMode>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{


/** A Wrapper for the output-selector plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class OutputSelector
: public Gst::Element
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef OutputSelector CppObjectType;
  typedef OutputSelector_Class CppClassType;
  typedef GstOutputSelector BaseObjectType;
  typedef GstOutputSelectorClass BaseClassType;

private:  friend class OutputSelector_Class;
  static CppClassType outputselector_class_;

private:
  // noncopyable
  OutputSelector(const OutputSelector&);
  OutputSelector& operator=(const OutputSelector&);

protected:
  explicit OutputSelector(const Glib::ConstructParams& construct_params);
  explicit OutputSelector(GstOutputSelector* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~OutputSelector();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstOutputSelector*       gobj()       { return reinterpret_cast<GstOutputSelector*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstOutputSelector* gobj() const { return reinterpret_cast<GstOutputSelector*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstOutputSelector* gobj_copy();

private:

  
protected:
  OutputSelector();
  explicit OutputSelector(const Glib::ustring& name);

public:
  /** Creates a new output-selector plugin with a unique name.
   */
  
  static Glib::RefPtr<OutputSelector> create();


  /** Creates a new output-selector plugin with the given name.
   */
  
  static Glib::RefPtr<OutputSelector> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Currently active src pad.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Gst::Pad> > property_active_pad() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Currently active src pad.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Gst::Pad> > property_active_pad() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Resend latest buffer after a switch to a new pad.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_resend_latest() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Resend latest buffer after a switch to a new pad.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_resend_latest() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The mode to be used for pad negotiation.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Gst::OutputSelectorPadNegotiationMode> property_pad_negotiation_mode() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The mode to be used for pad negotiation.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Gst::OutputSelectorPadNegotiationMode> property_pad_negotiation_mode() const;
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
   * @relates Gst::OutputSelector
   */
  Glib::RefPtr<Gst::OutputSelector> wrap(GstOutputSelector* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_OUTPUTSELECTOR_H */
