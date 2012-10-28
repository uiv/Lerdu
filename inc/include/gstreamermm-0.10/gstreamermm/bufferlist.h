// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_BUFFERLIST_H
#define _GSTREAMERMM_BUFFERLIST_H


#include <glibmm.h>

/* gstreamermm - a C++ wrapper for gstreamer
 *
 * Copyright 2008-2009 The gstreamermm Development Team
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

#include <gst/gstbufferlist.h>
#include <gstreamermm/miniobject.h>
#include <gstreamermm/wrap.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstBufferList GstBufferList;
typedef struct _GstBufferListClass GstBufferListClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class BufferList_Class; } // namespace Gst
namespace Gst
{

/** @addtogroup gstreamermmEnums gstreamermm Enums and Flags */

/**
 * @ingroup gstreamermmEnums
 */
enum BufferListItem
{
  BUFFER_LIST_CONTINUE,
  BUFFER_LIST_SKIP_GROUP,
  BUFFER_LIST_END
};


class Buffer;
class BufferListIterator;

/** A grouped scatter data buffer type for data-passing.
 * Buffer lists are units of grouped scatter/gather data transfer in GStreamer.
 *
 * Buffer lists are created with create() and filled with data using a
 * Gst::BufferListIterator. The iterator has no current buffer; its cursor
 * position lies between buffers, immediately before the buffer that would be
 * returned by Gst::BufferListIterator::next(). After iterating to the end of a group the iterator must
 * be advanced to the next group by a call to
 * Gst::BufferListIterator::next_group() before any further calls to
 * Gst::BufferListIterator::next() can return buffers again. The cursor
 * position of a newly created iterator lies before the first group; a call to
 * Gst::BufferListIterator::next_group() is necessary before calls to
 * Gst::BufferListIterator::next() can return buffers.
 *
 * For more details, see the C API docs.
 */

class BufferList : public MiniObject
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef BufferList CppObjectType;
  typedef BufferList_Class CppClassType;
  typedef GstBufferList BaseObjectType;
  typedef GstBufferListClass BaseClassType;

private:
  friend class BufferList_Class;
  static CppClassType bufferlist_class_;

private:
  // noncopyable
  BufferList(const BufferList&);
  BufferList& operator=(const BufferList&);

protected:
  explicit BufferList(GstBufferList* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~BufferList();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;
  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GstMiniObject.
  GstBufferList*       gobj()       { return reinterpret_cast<GstBufferList*>(gobject_); }

  ///Provides access to the underlying C GstMiniObject.
  const GstBufferList* gobj() const { return reinterpret_cast<GstBufferList*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstBufferList* gobj_copy();

private:


public:
  /** For example,
   * Gst::BufferListItem on_foreach(const Glib::RefPtr<Gst::Buffer>& buffer,
   * guint group, guint idx);.
   * A slot that will be called from foreach(). The buffer field will be a
   * reference to the buffer at idx in group.
   *
   * When this function returns Gst::BUFFER_LIST_CONTINUE, the next buffer will
   * be returned. When Gst::BUFFER_LIST_SKIP_GROUP is returned, all remaining
   * buffers in the current group will be skipped and the first buffer of the
   * next group is returned (if any). When Gst::BUFFER_LIST_END is returned,
   * foreach() will return.
   *
   * When the buffer is set to 0, the item will be removed from the bufferlist.
   * When the buffer has been made writable, the new buffer reference can be
   * assigned to the buffer.
   */
  typedef sigc::slot< Gst::BufferListItem, Glib::RefPtr<Gst::Buffer>&, guint,
    guint> SlotForeach;

public:
  
  /** Creates a new, empty Gst::BufferList. The caller is responsible for unreffing
   * the returned Gst::BufferList.
   * 
   * Free-function: gst_buffer_list_unref
   * @return The new Gst::BufferList. gst_buffer_list_unref()
   * after usage.
   */
  static Glib::RefPtr<BufferList> create();

  /** Tests if you can safely add buffers and groups into a buffer list.
   * @return <tt>true</tt> if the buffer list is writable, <tt>false</tt>
   * otherwise.
   *
   * Since 0.10.24
   */
  bool is_writable() const;

  /** Makes a writable buffer list from the buffer list. If the buffer list is
   * already writable, this will simply return the same buffer list. A copy
   * will otherwise be made.
   * @return The same buffer list if it is writable, otherwise a new copy.
   *
   * Since 0.10.24
   */
  Glib::RefPtr<BufferList> create_writable();

  //TODO: h2def.py does not recognize the C function declaration in the
  //header.
  /** Returns the number of groups in list.
   *
   * @return The number of groups in the buffer list.
   *
   * Since 0.10.24
   */
  guint n_groups() const;

  /** Call @a slot for each buffer in @a list.
   *
   * @a slot can modify the passed buffer pointer or its contents. The return
   * value of @a slot define if this function returns or if the remaining
   * buffers in a group should be skipped.
   *
   * @param slot A SlotForeach to call on each buffer.
   *
   * Since 0.10.24
   */
  void foreach(const SlotForeach& slot);

  
  /** Get the buffer at @a idx in @a group.
   * 
   * Note that this function is not efficient for iterating over the entire list.
   * Use an iterator or foreach() instead.
   * @param group The group.
   * @param idx The index in @a group.
   * @return The buffer at @a idx in @a group or <tt>0</tt> when there
   * is no buffer. The buffer remains valid as long as @a list is valid.
   */
  Glib::RefPtr<Gst::Buffer> get(guint group, guint idx);
  
  /** Get the buffer at @a idx in @a group.
   * 
   * Note that this function is not efficient for iterating over the entire list.
   * Use an iterator or foreach() instead.
   * @param group The group.
   * @param idx The index in @a group.
   * @return The buffer at @a idx in @a group or <tt>0</tt> when there
   * is no buffer. The buffer remains valid as long as @a list is valid.
   */
  Glib::RefPtr<const Gst::Buffer> get(guint group, guint idx) const;

  /** Iterate the buffers in the buffer list.
   * @param result A place in which to store the iterator.
   * Sine 0.10.24.
   */
  void iterate(BufferListIterator& result);


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


};

/** An opaque iterator for a Gst::BufferList.
 */
class BufferListIterator
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef BufferListIterator CppObjectType;
  typedef GstBufferListIterator BaseObjectType;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

private:


public:
  /// Default constructor.
  BufferListIterator();

  /** C object constructor.
   * @param castitem The C object.
   * @param take_ownership Whether to destroy the C object with the wrapper or
   * not.
   */
  explicit BufferListIterator(GstBufferListIterator* castitem,
    bool take_ownership = true);

  /// Destructor.
  virtual ~BufferListIterator();

  /// Provides access to the underlying C object.
  GstBufferListIterator* gobj()
    { return reinterpret_cast<GstBufferListIterator*>(gobject_); }

  /// Provides access to the underlying C object.
  const GstBufferListIterator* gobj() const
    { return reinterpret_cast<GstBufferListIterator*>(gobject_); }

private:
// noncopyable
  BufferListIterator(const BufferListIterator& other);
  BufferListIterator& operator=(const BufferListIterator& other);

  friend class BufferList;

public:
  /** For example,
   * Glib::RefPtr<Gst::Buffer> on_process(const Glib::RefPtr<Gst::Buffer>&
   * buffer);.
   * A slot for processing the last buffer returned by
   * Gst::BufferListIterator::next(). The slot can leave the buffer in the
   * list, replace the buffer in the list or remove the buffer from the list,
   * depending on the return value. If the slot returns 0, the buffer will be
   * removed from the list, otherwise the buffer will be replaced with the
   * returned buffer.
   *
   * The last buffer returned by Gst::BufferListIterator::next() will be
   * replaced with the buffer returned from the function. If 0 is returned, the
   * buffer will be removed from the list. The list must be writable.
   */
  typedef sigc::slot< Glib::RefPtr<Gst::Buffer>, const
    Glib::RefPtr<Gst::Buffer>& > SlotProcess;

  
  /** Returns the number of buffers left to iterate in the current group. I.e. the
   * number of calls that can be made to next() before
   * it returns <tt>0</tt>.
   * 
   * This function will not move the implicit cursor or in any other way affect
   * the state of the iterator @a it.
   * @return The number of buffers left to iterate in the current group.
   */
  guint n_buffers() const;
  
  /** Inserts @a buffer into the Gst::BufferList iterated with @a it. The buffer is
   * inserted into the current group, immediately before the buffer that would be
   * returned by next(). The buffer is inserted before
   * the implicit cursor, a subsequent call to next()
   * will return the buffer after the inserted buffer, if any.
   * 
   * This function takes ownership of @a buffer.
   * @param buffer A Gst::Buffer.
   */
  void add(const Glib::RefPtr<Gst::Buffer>& buffer);
  
  /** Inserts a new, empty group into the Gst::BufferList iterated with @a it. The
   * group is inserted immediately before the group that would be returned by
   * next_group(). A subsequent call to
   * next_group() will advance the iterator to the group
   * after the inserted group, if any.
   */
  void add_group();

  
  /** Returns the next buffer in the list iterated with @a it. If the iterator is at
   * the end of a group, <tt>0</tt> will be returned. This function may be called
   * repeatedly to iterate through the current group.
   * 
   * The caller will not get a new ref to the returned Gst::Buffer and must not
   * unref it.
   * @return The next buffer in the current group of the
   * buffer list, or <tt>0</tt>.
   */
  Glib::RefPtr<Gst::Buffer> next();
  
  /** Returns the next buffer in the list iterated with @a it. If the iterator is at
   * the end of a group, <tt>0</tt> will be returned. This function may be called
   * repeatedly to iterate through the current group.
   * 
   * The caller will not get a new ref to the returned Gst::Buffer and must not
   * unref it.
   * @return The next buffer in the current group of the
   * buffer list, or <tt>0</tt>.
   */
  Glib::RefPtr<const Gst::Buffer> next() const;

  
  /** Advance the iterator @a it to the first buffer in the next group. If the
   * iterator is at the last group, <tt>false</tt> will be returned. This function may be
   * called repeatedly to iterate through the groups in a buffer list.
   * @return <tt>true</tt> if the iterator could be advanced to the next group, <tt>false</tt> if
   * the iterator was already at the last group.
   */
  bool next_group();
  
  /** Removes the last buffer returned by next() from
   * the Gst::BufferList iterated with @a it. next() must
   * have been called on @a it before this function is called. This function can
   * only be called once per call to next().
   * 
   * The removed buffer is unreffed.
   */
  void remove();

  
  /** Returns the last buffer returned by next() without
   * modifying the refcount of the buffer.
   * @return The last buffer returned by
   * next().
   */
  Glib::RefPtr<Gst::Buffer> steal();
  
  /** Replaces the last buffer returned by next() with
   *  @a buffer in the Gst::BufferList iterated with @a it and takes ownership of
   *  @a buffer. next() must have been called on @a it before
   * this function is called. remove() must not have been
   * called since the last call to next().
   * 
   * This function unrefs the replaced buffer if it has not been stolen with
   * steal() and takes ownership of @a buffer (i.e. the
   * refcount of @a buffer is not increased).
   * 
   * FIXME 0.11: this conditional taking-ownership is not good for bindings
   * @param buffer A Gst::Buffer.
   */
  void take(const Glib::RefPtr<Gst::Buffer>& buffer);

  /** Calls the given function for the last buffer returned by next(). next()
   * must have been called on it before this function is called. remove() and
   * steal() must not have been called since the last call to next().
   *
   * See the SlotProcess docs for more details.
   *
   * @param slot The slot to be called.
   * @return The return value from @a slot.
   */
  Glib::RefPtr<Gst::Buffer> process(const SlotProcess& slot);

  
  /** Merge a buffer list group into a normal Gst::Buffer by copying its metadata
   * and memcpying its data into consecutive memory. All buffers in the current
   * group after the implicit cursor will be merged into one new buffer. The
   * metadata of the new buffer will be a copy of the metadata of the buffer that
   * would be returned by next(). If there is no buffer
   * in the current group after the implicit cursor, <tt>0</tt> will be returned.
   * 
   * This function will not move the implicit cursor or in any other way affect
   * the state of the iterator @a it or the list.
   * @return A new Gst::Buffer, gst_buffer_unref() after usage,
   * or <tt>0</tt>.
   */
  Glib::RefPtr<Gst::Buffer> merge_group();

protected:
  GstBufferListIterator* gobject_;      // The C object.
  bool take_ownership;                  // Bool signaling ownership.

protected:
  // So that Gst::BufferList::iterate() can set the C object.
  void set_gobject(GstBufferListIterator* castitem, bool take_ownership = true);


};

} // namespace Gst


namespace Gst
{
  /** @relates Gst::BufferList
   * @param object The C instance
   * @param take_copy false if the result should take ownership of the C instance. true if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   */
  Glib::RefPtr<Gst::BufferList> wrap(GstBufferList* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_BUFFERLIST_H */
