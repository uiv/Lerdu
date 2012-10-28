/*
 * This file was generated by orbit-idl - DO NOT EDIT!
 */

#include <glib.h>
#define ORBIT_IDL_SERIAL 9
#include <orb/orbit.h>

#ifndef gnome_unknown_H
#define gnome_unknown_H 1
#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

/** typedefs **/
#if !defined(ORBIT_DECL_GNOME_Unknown) && !defined(_GNOME_Unknown_defined)
#define ORBIT_DECL_GNOME_Unknown 1
#define _GNOME_Unknown_defined 1
#define GNOME_Unknown__free CORBA_Object__free
   typedef CORBA_Object GNOME_Unknown;
   extern CORBA_unsigned_long GNOME_Unknown__classid;
#if !defined(TC_IMPL_TC_GNOME_Unknown_0)
#define TC_IMPL_TC_GNOME_Unknown_0 'g'
#define TC_IMPL_TC_GNOME_Unknown_1 'n'
#define TC_IMPL_TC_GNOME_Unknown_2 'o'
#define TC_IMPL_TC_GNOME_Unknown_3 'm'
#define TC_IMPL_TC_GNOME_Unknown_4 'e'
#define TC_IMPL_TC_GNOME_Unknown_5 '_'
#define TC_IMPL_TC_GNOME_Unknown_6 'u'
#define TC_IMPL_TC_GNOME_Unknown_7 'n'
#define TC_IMPL_TC_GNOME_Unknown_8 'k'
#define TC_IMPL_TC_GNOME_Unknown_9 'n'
#define TC_IMPL_TC_GNOME_Unknown_10 'o'
#define TC_IMPL_TC_GNOME_Unknown_11 'w'
#define TC_IMPL_TC_GNOME_Unknown_12 'n'
   extern const struct CORBA_TypeCode_struct TC_GNOME_Unknown_struct;
#define TC_GNOME_Unknown ((CORBA_TypeCode)&TC_GNOME_Unknown_struct)
#endif
#endif

/** POA structures **/
   typedef struct
   {
      void *_private;
      void (*ref) (PortableServer_Servant _servant, CORBA_Environment * ev);
      void (*unref) (PortableServer_Servant _servant, CORBA_Environment * ev);
       CORBA_Object(*query_interface) (PortableServer_Servant _servant,
				       const CORBA_char * repoid,
				       CORBA_Environment * ev);
   }
   POA_GNOME_Unknown__epv;
   typedef struct
   {
      PortableServer_ServantBase__epv *_base_epv;
      POA_GNOME_Unknown__epv *GNOME_Unknown_epv;
   }
   POA_GNOME_Unknown__vepv;
   typedef struct
   {
      void *_private;
      POA_GNOME_Unknown__vepv *vepv;
   }
   POA_GNOME_Unknown;
   extern void POA_GNOME_Unknown__init(PortableServer_Servant servant,
				       CORBA_Environment * ev);
   extern void POA_GNOME_Unknown__fini(PortableServer_Servant servant,
				       CORBA_Environment * ev);

/** prototypes **/
   void GNOME_Unknown_ref(GNOME_Unknown _obj, CORBA_Environment * ev);
   void GNOME_Unknown_unref(GNOME_Unknown _obj, CORBA_Environment * ev);
   CORBA_Object GNOME_Unknown_query_interface(GNOME_Unknown _obj,
					      const CORBA_char * repoid,
					      CORBA_Environment * ev);

   void _ORBIT_skel_GNOME_Unknown_ref(POA_GNOME_Unknown * _ORBIT_servant,
				      GIOPRecvBuffer * _ORBIT_recv_buffer,
				      CORBA_Environment * ev,
				      void (*_impl_ref)
				      (PortableServer_Servant _servant,
				       CORBA_Environment * ev));
   void _ORBIT_skel_GNOME_Unknown_unref(POA_GNOME_Unknown * _ORBIT_servant,
					GIOPRecvBuffer * _ORBIT_recv_buffer,
					CORBA_Environment * ev,
					void (*_impl_unref)
					(PortableServer_Servant _servant,
					 CORBA_Environment * ev));
   void _ORBIT_skel_GNOME_Unknown_query_interface(POA_GNOME_Unknown *
						  _ORBIT_servant,
						  GIOPRecvBuffer *
						  _ORBIT_recv_buffer,
						  CORBA_Environment * ev,
						  CORBA_Object
						  (*_impl_query_interface)
						  (PortableServer_Servant
						   _servant,
						   const CORBA_char * repoid,
						   CORBA_Environment * ev));
#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif
#undef ORBIT_IDL_SERIAL