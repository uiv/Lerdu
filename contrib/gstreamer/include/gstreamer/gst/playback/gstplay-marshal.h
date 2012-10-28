
#ifndef __gst_play_marshal_MARSHAL_H__
#define __gst_play_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOLEAN:OBJECT,OBJECT (gstplay-marshal.list:1) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_BOOLEAN__OBJECT_OBJECT (GClosure     *closure,
                                                     GValue       *return_value,
                                                     guint         n_param_values,
                                                     const GValue *param_values,
                                                     gpointer      invocation_hint,
                                                     gpointer      marshal_data);

/* BOOLEAN:OBJECT,OBJECT,OBJECT (gstplay-marshal.list:2) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_BOOLEAN__OBJECT_OBJECT_OBJECT (GClosure     *closure,
                                                            GValue       *return_value,
                                                            guint         n_param_values,
                                                            const GValue *param_values,
                                                            gpointer      invocation_hint,
                                                            gpointer      marshal_data);

/* BOXED:OBJECT,OBJECT (gstplay-marshal.list:3) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_BOXED__OBJECT_OBJECT (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);

/* VOID:OBJECT,BOOLEAN (gstplay-marshal.list:4) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_VOID__OBJECT_BOOLEAN (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);

/* ENUM:OBJECT,OBJECT,BOXED (gstplay-marshal.list:5) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_ENUM__OBJECT_OBJECT_BOXED (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);

/* ENUM:OBJECT,OBJECT,OBJECT (gstplay-marshal.list:6) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_ENUM__OBJECT_OBJECT_OBJECT (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* BOXED:OBJECT,OBJECT,BOXED (gstplay-marshal.list:7) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_BOXED__OBJECT_OBJECT_BOXED (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);

/* BOXED:INT (gstplay-marshal.list:8) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_BOXED__INT (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

/* OBJECT:BOXED (gstplay-marshal.list:9) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_play_marshal_OBJECT__BOXED (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

G_END_DECLS

#endif /* __gst_play_marshal_MARSHAL_H__ */

