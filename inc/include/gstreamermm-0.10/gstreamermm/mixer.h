// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_MIXER_H
#define _GSTREAMERMM_MIXER_H


#include <glibmm.h>

/* gstreamermm - a C++ wrapper for gstreamer
 *
 * Copyright 2008 The gstreamermm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gst/interfaces/mixer.h>
#include <gst/interfaces/mixeroptions.h>
#include <gstreamermm/mixertrack.h>
#include <gstreamermm/message.h>
#include <glibmm/interface.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstMixer GstMixer;
typedef struct _GstMixerClass GstMixerClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class Mixer_Class; } // namespace Gst
namespace Gst
{

/** @addtogroup gstreamermmEnums gstreamermm Enums and Flags */

/**
 * @ingroup gstreamermmEnums
 */
enum MixerType
{
  MIXER_HARDWARE,
  MIXER_SOFTWARE
};

} // namespace Gst


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gst::MixerType> : public Glib::Value_Enum<Gst::MixerType>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{

/**
 * @ingroup gstreamermmEnums
 * @par Bitwise operators:
 * <tt>%MixerFlags operator|(MixerFlags, MixerFlags)</tt><br>
 * <tt>%MixerFlags operator&(MixerFlags, MixerFlags)</tt><br>
 * <tt>%MixerFlags operator^(MixerFlags, MixerFlags)</tt><br>
 * <tt>%MixerFlags operator~(MixerFlags)</tt><br>
 * <tt>%MixerFlags& operator|=(MixerFlags&, MixerFlags)</tt><br>
 * <tt>%MixerFlags& operator&=(MixerFlags&, MixerFlags)</tt><br>
 * <tt>%MixerFlags& operator^=(MixerFlags&, MixerFlags)</tt><br>
 */
enum MixerFlags
{
  MIXER_FLAG_NONE = 0x0,
  MIXER_FLAG_AUTO_NOTIFICATIONS = (1<<0),
  MIXER_FLAG_HAS_WHITELIST = (1<<1),
  MIXER_FLAG_GROUPING = (1<<2)
};

/** @ingroup gstreamermmEnums */
inline MixerFlags operator|(MixerFlags lhs, MixerFlags rhs)
  { return static_cast<MixerFlags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs)); }

/** @ingroup gstreamermmEnums */
inline MixerFlags operator&(MixerFlags lhs, MixerFlags rhs)
  { return static_cast<MixerFlags>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)); }

/** @ingroup gstreamermmEnums */
inline MixerFlags operator^(MixerFlags lhs, MixerFlags rhs)
  { return static_cast<MixerFlags>(static_cast<unsigned>(lhs) ^ static_cast<unsigned>(rhs)); }

/** @ingroup gstreamermmEnums */
inline MixerFlags operator~(MixerFlags flags)
  { return static_cast<MixerFlags>(~static_cast<unsigned>(flags)); }

/** @ingroup gstreamermmEnums */
inline MixerFlags& operator|=(MixerFlags& lhs, MixerFlags rhs)
  { return (lhs = static_cast<MixerFlags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs))); }

/** @ingroup gstreamermmEnums */
inline MixerFlags& operator&=(MixerFlags& lhs, MixerFlags rhs)
  { return (lhs = static_cast<MixerFlags>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs))); }

/** @ingroup gstreamermmEnums */
inline MixerFlags& operator^=(MixerFlags& lhs, MixerFlags rhs)
  { return (lhs = static_cast<MixerFlags>(static_cast<unsigned>(lhs) ^ static_cast<unsigned>(rhs))); }

} // namespace Gst


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gst::MixerFlags> : public Glib::Value_Flags<Gst::MixerFlags>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{

/**
 * @ingroup gstreamermmEnums
 */
enum MixerMessageType
{
  MIXER_MESSAGE_INVALID,
  MIXER_MESSAGE_MUTE_TOGGLED,
  MIXER_MESSAGE_RECORD_TOGGLED,
  MIXER_MESSAGE_VOLUME_CHANGED,
  MIXER_MESSAGE_OPTION_CHANGED,
  MIXER_MESSAGE_OPTIONS_LIST_CHANGED,
  MIXER_MESSAGE_MIXER_CHANGED
};

} // namespace Gst


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gst::MixerMessageType> : public Glib::Value_Enum<Gst::MixerMessageType>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{


class MixerOptions;
class MixerTrack;

/** An interface for elements that provide mixer operations.
 * @ingroup GstInterfaces
 */

class Mixer : public Glib::Interface
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef Mixer CppObjectType;
  typedef Mixer_Class CppClassType;
  typedef GstMixer BaseObjectType;
  typedef GstMixerClass BaseClassType;

private:
  friend class Mixer_Class;
  static CppClassType mixer_class_;

  // noncopyable
  Mixer(const Mixer&);
  Mixer& operator=(const Mixer&);

protected:
  Mixer(); // you must derive from this class

  /** Called by constructors of derived classes. Provide the result of 
   * the Class init() function to ensure that it is properly 
   * initialized.
   * 
   * @param interface_class The Class object for the derived type.
   */
  explicit Mixer(const Glib::Interface_Class& interface_class);

public:
  // This is public so that C++ wrapper instances can be
  // created for C instances of unwrapped types.
  // For instance, if an unexpected C type implements the C interface. 
  explicit Mixer(GstMixer* castitem);

protected:
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~Mixer();

  static void add_interface(GType gtype_implementer);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;
  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstMixer*       gobj()       { return reinterpret_cast<GstMixer*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstMixer* gobj() const { return reinterpret_cast<GstMixer*>(gobject_); }

private:


public:
 

  /** Returns a list of available tracks for this mixer/element. Note that it
   * is allowed for sink (output) elements to only provide the output tracks
   * in this list. Likewise, for sources (inputs), it is allowed to only
   * provide input elements in this list.
   *
   * @return A std::vector<> consisting of zero or more Gst::MixerTrack.
   */
  std::vector< Glib::RefPtr<Gst::MixerTrack> > list_tracks();

 
  /** Returns a list of available tracks for this mixer/element. Note that it
   * is allowed for sink (output) elements to only provide the output tracks
   * in this list. Likewise, for sources (inputs), it is allowed to only
   * provide input elements in this list.
   *
   * @return A std::vector<> consisting of zero or more Gst::MixerTrack.
   */
  std::vector< Glib::RefPtr<const Gst::MixerTrack> > list_tracks() const;

  /** Get the current volume(s) on the given track.
   *
   * @param track the Gst::MixerTrack to get the volume from.
   * @return an array of integers (of size track->num_channels) to with the
   * current volume of each channel in the given track. 
   */
  std::vector<int> get_volume(const Glib::RefPtr<const Gst::MixerTrack>& track) const;
  

  /** Sets the volume on each channel in a track. Short note about
   * naming: a track is defined as one separate stream owned by
   * the mixer/element, such as 'Line-in' or 'Microphone'. A
   * channel is said to be a mono-stream inside this track. A
   * stereo track thus contains two channels.
   * @param track The Gst::MixerTrack to set the volume on.
   * @param volumes An array of integers (of size track->num_channels)
   * that gives the wanted volume for each channel in
   * this track.
   */
  void set_volume(const Glib::RefPtr<Gst::MixerTrack>& track, const std::vector<int>& volumes);

  
  /** Mutes or unmutes the given channel. To find out whether a
   * track is currently muted, use GST_MIXER_TRACK_HAS_FLAG ().
   * @param track The Gst::MixerTrack to operate on.
   * @param mute A boolean value indicating whether to turn on or off
   * muting.
   */
  void set_mute(const Glib::RefPtr<Gst::MixerTrack>& track, bool mute);
  
  /** Enables or disables recording on the given track. Note that
   * this is only possible on input tracks, not on output tracks
   * (see GST_MIXER_TRACK_HAS_FLAG () and the GST_MIXER_TRACK_INPUT
   * flag).
   * @param track The Gst::MixerTrack to operate on.
   * @param record A boolean value that indicates whether to turn on
   * or off recording.
   */
  void set_record(const Glib::RefPtr<Gst::MixerTrack>& track, bool record);
  
  /** Sets a name/value option in the mixer to the requested value.
   * @param opts The Gst::MixerOptions that we operate on.
   * @param value The requested new option value.
   */
  void set_option(const Glib::RefPtr<Gst::MixerOptions>& opts, const Glib::ustring& value);
  
  /** This function is called by the mixer implementation to produce
   * a notification message on the bus indicating that the given track
   * has changed mute state.
   * 
   * This function only works for GstElements that are implementing the
   * GstMixer interface, and the element needs to have been provided a bus.
   * @param track The GstMixerTrack that has change mute state.
   * @param mute The new state of the mute flag on the track.
   */
  void mute_toggled(const Glib::RefPtr<Gst::MixerTrack>& track, bool mute);
  
  /** This function is called by the mixer implementation to produce
   * a notification message on the bus indicating that the given track
   * has changed recording state.
   * 
   * This function only works for GstElements that are implementing the
   * GstMixer interface, and the element needs to have been provided a bus.
   * @param track The GstMixerTrack that has changed recording state.
   * @param record The new state of the record flag on the track.
   */
  void record_toggled(const Glib::RefPtr<Gst::MixerTrack>& track, bool record);
  
  /** This function is called by the mixer implementation to produce
   * a notification message on the bus indicating that the volume(s) for the
   * given track have changed.
   * 
   * This function only works for GstElements that are implementing the
   * GstMixer interface, and the element needs to have been provided a bus.
   * @param track The GstMixerTrack that has changed.
   * @param volumes Array of volume values, one per channel on the mixer track.
   */
  void volume_changed(const Glib::RefPtr<Gst::MixerTrack>& track, const std::vector<int>& volumes);
  
  /** This function is called by the mixer implementation to produce
   * a notification message on the bus indicating that the given options
   * object has changed state. 
   * 
   * This function only works for GstElements that are implementing the
   * GstMixer interface, and the element needs to have been provided a bus.
   * @param opts The GstMixerOptions that has changed value.
   * @param value The new value of the GstMixerOptions.
   */
  void option_changed(const Glib::RefPtr<Gst::MixerOptions>& opts, const Glib::ustring& value);
  
  /** This function is called by the mixer implementation to produce
   * a notification message on the bus indicating that the list of possible
   * options of a given options object has changed.
   * 
   * The new options are not contained in the message on purpose. Applications
   * should call gst_mixer_option_get_values() on @a opts to make @a opts update
   * its internal state and obtain the new list of values.
   * 
   * This function only works for GstElements that are implementing the
   * GstMixer interface, and the element needs to have been provided a bus
   * for this to work.
   * @param opts The GstMixerOptions whose list of values has changed.
   */
  void options_list_changed(const Glib::RefPtr<Gst::MixerOptions>& opts);
  
  /** Get the current value of a name/value option in the mixer.
   * @param opts The Gst::MixerOptions that we operate on.
   * @return Current value of the name/value option.
   */
  Glib::ustring get_option(const Glib::RefPtr<const Gst::MixerOptions>& opts) const;
  
  /** This function is called by the mixer implementation to produce
   * a notification message on the bus indicating that the list of available
   * mixer tracks for a given mixer object has changed. Applications should
   * rebuild their interface when they receive this message.
   * 
   * This function only works for GstElements that are implementing the
   * GstMixer interface, and the element needs to have been provided a bus.
   */
  void mixer_changed();
  
  /** Get the set of supported flags for this mixer implementation.
   * @return A set of or-ed GstMixerFlags for supported features.
   */
  Gst::MixerFlags get_mixer_flags() const;
  
  /** Get the Gst::MixerType of this mixer implementation.
   * @return A the Gst::MixerType.
   */
  Gst::MixerType get_mixer_type() const;
  
  /** Check a bus message to see if it is a GstMixer notification
   * message and return the GstMixerMessageType identifying which
   * type of notification it is.
   * @param message A GstMessage to inspect.
   * @return The type of the GstMixerMessage, or GST_MIXER_MESSAGE_INVALID
   * if the message is not a GstMixer notification.
   */
  static Gst::MixerMessageType get_message_type(const Glib::RefPtr<Gst::Message>& message);

  // Signals were deprecated before we could wrap them (see header file).  It
  // looks like messages to the bus have replaced the signals (see the
  // Gst::MessageMixer* classes):
  

  virtual std::vector<int> get_volume_vfunc(const Glib::RefPtr<const Gst::MixerTrack>& track) const;

  virtual std::vector< Glib::RefPtr<Gst::MixerTrack> > list_tracks_vfunc();

 
    virtual void set_volume_vfunc(const Glib::RefPtr<Gst::MixerTrack>& track, const std::vector<int>& volumes);


    virtual void set_mute_vfunc(const Glib::RefPtr<Gst::MixerTrack>& track, bool mute);

    virtual void set_record_vfunc(const Glib::RefPtr<Gst::MixerTrack>& track, bool record);


    virtual void set_option_vfunc(const Glib::RefPtr<Gst::MixerOptions>& opts, const Glib::ustring& value);


    virtual Glib::ustring get_option_vfunc(const Glib::RefPtr<const Gst::MixerOptions>& opts) const;


    virtual Gst::MixerFlags get_mixer_flags_vfunc() const;


protected:

  
public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


};

/** A mute-toggled Gst::Mixer message.
 */
class MessageMixerMuteToggled : public Message
{
public:
  explicit MessageMixerMuteToggled(GstMessage* castitem);

  /** Extracts the contents of a mute-toggled bus message. Reads the
   * Gst::MixerTrack that has changed, and the new value of the mute flag.
   * 
   * The Gst::MixerTrack remains valid until the message is freed.
   *
   * @param track Reference to hold a Gst::MixerTrack object.
   * @param mute A reference to hold the bool new value.
   *
   * Since 0.10.14.
   */
  void parse(Glib::RefPtr<Gst::MixerTrack>& track, bool& mute) const;
  

  /** Extracts the contents of a mute-toggled bus message. Reads the
   * Gst::MixerTrack that has changed, and the new value of the mute flag.
   * 
   * The Gst::MixerTrack remains valid until the message is freed.
   *
   * @return the Gst::MixerTrack object.
   */
  Glib::RefPtr<Gst::MixerTrack> parse();

  /** Extracts the contents of a mute-toggled bus message. Reads the
   * Gst::MixerTrack that has changed, and the new value of the mute flag.
   * 
   * The Gst::MixerTrack remains valid until the message is freed.
   *
   * @return the Gst::MixerTrack object.
   */
  Glib::RefPtr<const Gst::MixerTrack> parse() const;

  /** Extracts the contents of a mute-toggled bus message. Reads the new value
   * of the mute flag.
   * 
   * @return The bool new value.
   */

  bool parse_mute() const;
};

/** A record-toggled Gst::Mixer message.
 */
class MessageMixerRecordToggled : public Message
{
public:
  explicit MessageMixerRecordToggled(GstMessage* castitem);

  /** Extracts the contents of a record-toggled bus message. Reads the
   * Gst::MixerTrack that has changed, and the new value of the recording flag.
   *
   * The Gst::MixerTrack remains valid until the message is freed.
   *
   * @param track A reference to hold the Gst::MixerTrack object.
   * @param record A reference to hold the bool value.
   *
   * Since 0.10.14.
   */
  void parse(Glib::RefPtr<Gst::MixerTrack>& track, bool& record) const;
  

  /** Extracts the contents of a record-toggled bus message. Reads the
   * Gst::MixerTrack that has changed.
   *
   * The Gst::MixerTrack remains valid until the message is freed.
   *
   * @return A reference to the Gst::MixerTrack object.
   */
  Glib::RefPtr<Gst::MixerTrack> parse();

  /** Extracts the contents of a record-toggled bus message. Reads the
   * Gst::MixerTrack that has changed.
   *
   * The Gst::MixerTrack remains valid until the message is freed.
   *
   * @return A reference to the Gst::MixerTrack object.
   */
  Glib::RefPtr<const Gst::MixerTrack> parse() const;

  /** Extracts the contents of a record-toggled bus message. Reads the new
   * value of the recording flag.
   *
   * @return The bool value.
   */
  bool parse_record() const;
};

/** A volume-changed Gst::Mixer message.
 */
class MessageMixerVolumeChanged : public Message
{
public:
  explicit MessageMixerVolumeChanged(GstMessage* castitem);

  /** Parses a volume-changed notification message and extracts the track
   * object it refers to, as well as an array of volumes and the size of the
   * volumes array.
   *
   * The track object remains valid until the message is freed.
   *
   * @param track A reference to hold a Gst::MixerTrack object.
   * @param volumes A reference to receive an array of int values.
   *
   * Since 0.10.14.
   */
  void parse(Glib::RefPtr<Gst::MixerTrack>& track, std::vector<int>& volumes) const;
  

  /** Parses a volume-changed notification message and extracts the track
   * object it refers to.
   *
   * The track object remains valid until the message is freed.
   *
   * @return a Gst::MixerTrack object.
   */
  Glib::RefPtr<Gst::MixerTrack> parse();

  /** Parses a volume-changed notification message and extracts the track
   * object it refers to.
   *
   * The track object remains valid until the message is freed.
   *
   * @return a Gst::MixerTrack object.
   */
  Glib::RefPtr<const Gst::MixerTrack> parse() const;

  /** Parses a volume-changed notification message and extracts the array of
   * volumes.
   *
   * @return The array of volume int values.
   */
  std::vector<int> parse_volumes() const;
};

/** An option-changed Gst::Mixer message.
 */
class MessageMixerOptionChanged : public Message
{
public:
  explicit MessageMixerOptionChanged(GstMessage* castitem);

  /** Extracts the Gst::MixerOptions and new value from a option-changed bus
   * notification message.
   *
   * The Gst::MixerOptions remain valid until the message is freed.
   *
   * @param options A reference to hold the Gst::MixerOptions object.
   * @param value Location to store the new options value.
   *
   * Since 0.10.14.
   */
  void parse(Glib::RefPtr<Gst::MixerOptions>& options,
    Glib::ustring& value) const;
  

  /** Extracts the Gst::MixerOptions from a option-changed bus notification
   * message.
   *
   * The Gst::MixerOptions remain valid until the message is freed.
   *
   * @return The Gst::MixerOptions object.
   */
  Glib::RefPtr<Gst::MixerOptions> parse();

  /** Extracts the Gst::MixerOptions from a option-changed bus notification
   * message.
   *
   * The Gst::MixerOptions remain valid until the message is freed.
   *
   * @return The Gst::MixerOptions object.
   */
  Glib::RefPtr<const Gst::MixerOptions> parse() const;

  /** Extracts the new value of the option from a option-changed bus
   * notification message.
   *
   * @return The new options value.
   */
  Glib::ustring parse_value() const;
};

/** An option-list-changed Gst::Mixer message, osted when the list of available
 * options for a GstMixerOptions object has changed (Since: 0.10.18).
 */
class MessageMixerOptionsListChanged : public Message
{
public:
  explicit MessageMixerOptionsListChanged(GstMessage* castitem);

  /** Extracts the Gst::MixerOptions whose value list has changed from an
   * options-list-changed bus notification message.
   *
   * The options object returned remains valid until the message is freed.
   *
   * Since 0.10.18.
   */
  Glib::RefPtr<Gst::MixerOptions> parse();
  

  /** Extracts the Gst::MixerOptions whose value list has changed from an
   * options-list-changed bus notification message.
   *
   * The options object returned remains valid until the message is freed.
   *
   * Since 0.10.18.
   */
  Glib::RefPtr<const Gst::MixerOptions> parse() const;
};

/** A mixer-changed Gst::Mixer message, posted when the list of available mixer
 * tracks has changed. The application should re-build its interface in this
 * case (Since: 0.10.18).
 */
class MessageMixerChanged : public Message
{
public:
  explicit MessageMixerChanged(GstMessage* castitem);
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
   * @relates Gst::Mixer
   */
  Glib::RefPtr<Gst::Mixer> wrap(GstMixer* object, bool take_copy = false);

} // namespace Glib


#endif /* _GSTREAMERMM_MIXER_H */

