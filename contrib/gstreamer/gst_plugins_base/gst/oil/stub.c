#include"liboil.h"

//Arun's changes
typedef struct _OilFunctionImpl OilFunctionImpl;
typedef struct _OilFunctionClass OilFunctionClass;
typedef void (*OilTestFunction) (OilFunctionClass *klass,OilFunctionImpl *impl);

#define OIL_CHECK_PROTOTYPE(a)
/**
 * OilFunctionClass:
 *
 * An opaque structure representing a function class.
 *
 */
struct _OilFunctionClass {
  /*< private >*/
	void *func;
	const char *name;
	const char *desc;
	OilTestFunction test_func;

	OilFunctionImpl *first_impl;
	OilFunctionImpl *reference_impl;

	OilFunctionImpl *chosen_impl;

	const char *prototype;
};


/**
 * OilFunctionImpl:
 *
 * An opaque structure representing a function implementation.
 *
 */
struct _OilFunctionImpl {
  /*< private >*/
    void *next;
    OilFunctionClass *klass;
    void *func;
    unsigned int flags;
    const char *name;
        double profile_ave;
        double profile_std;
};

#ifndef OIL_NO_CLASSES
/**
 * OIL_DEFINE_CLASS_FULL:
 * @klass: name of class to declare (without oil_ prefix)
 * @string: prototype of class
 * @test: test function
 *
 * Defines a #OilFunctionClass structure for @klass.  Classes
 * defined this way will be automatically at Liboil initialization
 * time.
 */

#define OIL_DEFINE_CLASS_FULL(klass, string, test) \
OilFunctionClass _oil_function_class_ ## klass = { \
	NULL, \
	#klass , \
	NULL, \
        test, \
        NULL, \
        NULL, \
        NULL, \
        string \
}; \
OilFunctionClass *oil_function_class_ptr_ ## klass = \
  &_oil_function_class_ ## klass
#else
#define OIL_DEFINE_CLASS_FULL(klass, string, test) \
  OIL_DECLARE_CLASS(klass)
#endif

/**
 * OIL_DEFINE_CLASS:
 * @klass: name of class to declare (without oil_ prefix)
 * @string: prototype of class
 *
 * Defines a #OilFunctionClass structure for @klass.  Classes
 * defined this way will be automatically at Liboil initialization
 * time.
 */
#define OIL_DEFINE_CLASS(klass, string) \
  OIL_DEFINE_CLASS_FULL (klass, string, NULL)


OIL_DEFINE_CLASS (scalarmultiply_f32_ns, "float *d, float *s1, float *s2_1, int n");

OIL_DEFINE_CLASS_FULL (resample_linear_argb,
    "uint32_t *d_n, uint32_t *s_2xn, int n, uint32_t *i_2",
    NULL);

OIL_DEFINE_CLASS_FULL (resample_linear_u8,
    "uint8_t *d_n, uint8_t *s_2xn, int n, uint32_t *i_2",
    NULL);

OIL_DEFINE_CLASS_FULL (merge_linear_argb,
    "uint32_t *d_n, uint32_t *s_n, uint32_t *s2_n, uint32_t *s3_1, int n",
    NULL);
OIL_DEFINE_CLASS_FULL (merge_linear_u8,
    "uint8_t *d_n, uint8_t *s_n, uint8_t *s2_n, uint32_t *s3_1, int n",
    NULL);

OIL_DEFINE_CLASS(splat_u8_ns,"uint8_t *dest, uint8_t *s1_1, int n");

OIL_DEFINE_CLASS(splat_u8,"uint8_t *dest, int dstr, uint8_t *s1_1, int n");

typedef enum {
  OIL_IMPL_FLAG_REF = (1<<0),
  OIL_IMPL_FLAG_OPT = (1<<1),
  OIL_IMPL_FLAG_ASM = (1<<2),
  OIL_IMPL_FLAG_DISABLED = (1<<3),
  OIL_IMPL_FLAG_CMOV = (1<<16),
  OIL_IMPL_FLAG_MMX = (1<<17),
  OIL_IMPL_FLAG_SSE = (1<<18),
  OIL_IMPL_FLAG_MMXEXT = (1<<19),
  OIL_IMPL_FLAG_SSE2 = (1<<20),
  OIL_IMPL_FLAG_3DNOW = (1<<21),
  OIL_IMPL_FLAG_3DNOWEXT = (1<<22),
  OIL_IMPL_FLAG_SSE3 = (1<<23),
  OIL_IMPL_FLAG_ALTIVEC = (1<<24),
  OIL_IMPL_FLAG_EDSP = (1<<25),
  OIL_IMPL_FLAG_ARM6 = (1<<26),
  OIL_IMPL_FLAG_VFP = (1<<27),
  OIL_IMPL_FLAG_SSSE3 = (1<<28)
} OilImplFlag;

#ifndef OIL_OPT_MANGLE
#define OIL_OPT_MANGLE(a) a
#define OIL_OPT_FLAG_MANGLE(a) a
#else
#define OIL_NO_CLASSES
#define OIL_OPT_FLAG_MANGLE(a) (((a)&(~OIL_IMPL_FLAG_REF)) | OIL_IMPL_FLAG_OPT)
#endif
#ifndef OIL_OPT_SUFFIX
#define OIL_OPT_SUFFIX
#endif

/**
 * OIL_DEFINE_IMPL_FULL:
 * @function: name of function
 * @klass: name of class to declare (without oil_ prefix)
 * @flags: implementation flags and CPU requirements
 *
 * Defines a #OilFunctionImpl structure for the function @function
 * and class @klass.  CPU-dependent flags in @flags will indicate
 * that this implementation requires the given CPU flags.
 */
#define OIL_DEFINE_IMPL_FULL(function,klass,flags) \
OilFunctionImpl OIL_OPT_MANGLE(_oil_function_impl_ ## function) = { \
    NULL, \
    &_oil_function_class_ ## klass , \
    (void *)function, \
    OIL_OPT_FLAG_MANGLE(flags), \
        #function OIL_OPT_SUFFIX \
} \
OIL_CHECK_PROTOTYPE(;_oil_type_ ## klass _ignore_me_ ## function = function)

/**
 * OIL_DEFINE_IMPL:
 * @function: name of function
 * @klass: name of class to declare (without oil_ prefix)
 *
 * Shorthand for defining a C implementation.  See OIL_DEFINE_IMPL_FULL().
 */
#define OIL_DEFINE_IMPL(function,klass) \
    OIL_DEFINE_IMPL_FULL(function,klass,0)
/**
 * OIL_DEFINE_IMPL_REF:
 * @function: name of function
 * @klass: name of class to declare (without oil_ prefix)
 *
 * Shorthand for defining a reference implementation.  See OIL_DEFINE_IMPL_FULL().
 */
#define OIL_DEFINE_IMPL_REF(function,klass) \
    OIL_DEFINE_IMPL_FULL(function,klass,OIL_IMPL_FLAG_REF)




typedef void (*_oil_type_scalarmultiply_f32_ns)(float * d, const float * s1, const float * s2_1, int n);
//#define oil_scalarmultiply_f32_ns ((_oil_type_scalarmultiply_f32_ns)(*(void **)oil_function_class_ptr_scalarmultiply_f32_ns))



#define OIL_GET(ptr, offset, type) (*(type *)((uint8_t *)(ptr) + (offset)) )

/**************'_oil_resample_linear_u8'****************************/
#ifdef __SYMBIAN32__
EXPORT_C
#endif


static void
resample_linear_u8_ref (uint8_t *dest, uint8_t *src, int n,
    uint32_t *in)
{
  int acc = in[0];
  int increment = in[1];
  int i;
  int j;
  int x;

  for(i=0;i<n;i++){
    j = acc>>16;
    x = (acc&0xffff)>>8;
    dest[i] = (src[j]*(256-x) + src[j+1]*x) >> 8;

    acc += increment;
  }

  in[0] = acc;
}

/************************'_oil_resample_linear_argb'***************************/
static  void
resample_linear_argb_ref (uint32_t *d, uint32_t *s, int n, uint32_t *in)
{
  uint8_t *src = (uint8_t *)s;
  uint8_t *dest = (uint8_t *)d;
  int acc = in[0];
  int increment = in[1];
  int i;
  int j;
  int x;

  for(i=0;i<n;i++){
    j = acc>>16;
    x = (acc&0xffff)>>8;
    dest[4*i+0] = (src[4*j+0]*(256-x) + src[4*j+4]*x) >> 8;
    dest[4*i+1] = (src[4*j+1]*(256-x) + src[4*j+5]*x) >> 8;
    dest[4*i+2] = (src[4*j+2]*(256-x) + src[4*j+6]*x) >> 8;
    dest[4*i+3] = (src[4*j+3]*(256-x) + src[4*j+7]*x) >> 8;

    acc += increment;
  }

  in[0] = acc;
}

/****************** '_oil_merge_linear_argb'**************************/
static  void
merge_linear_argb_ref (uint32_t *d, uint32_t *s1, uint32_t *s2,
    uint32_t *src3, int n)
{
  uint8_t *src1 = (uint8_t *)s1;
  uint8_t *src2 = (uint8_t *)s2;
  uint8_t *dest = (uint8_t *)d;
  int i;
  int x = src3[0];

  for(i=0;i<n;i++){
    dest[4*i+0] = (src1[4*i+0]*(256-x) + src2[4*i+0]*x) >> 8;
    dest[4*i+1] = (src1[4*i+1]*(256-x) + src2[4*i+1]*x) >> 8;
    dest[4*i+2] = (src1[4*i+2]*(256-x) + src2[4*i+2]*x) >> 8;
    dest[4*i+3] = (src1[4*i+3]*(256-x) + src2[4*i+3]*x) >> 8;
  }
}

static void
merge_linear_u8_ref (uint8_t *dest, uint8_t *src1, uint8_t *src2,
    uint32_t *src3, int n)
{
  int i;
  int x = src3[0];

  for(i=0;i<n;i++){
    dest[i] = (src1[i]*(256-x) + src2[i]*x) >> 8;
  }
}

static void splat_u8_ref (uint8_t *dest, int dstr, uint8_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    OIL_GET(dest,i*dstr, uint8_t) = *param;
  }
}

static void splat_u8_ns_ref (uint8_t *dest, uint8_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    dest[i] = *param;
  }
}

static void
scalarmultiply_f32_ns_ref (float *dest, float *src1, float *src2, int n)
{
  int i;

  for(i=0;i<n;i++){
    dest[i] = src1[i] * src2[0];
  }
}

/********oil_splat_u8*******/
/*
EXPORT_C  void splat_u8_ref (uint8_t *dest, int dstr, uint8_t *param, int n)
{
  int i;
  for(i=0;i<n;i++){
    OIL_GET(dest,i*dstr, uint8_t) = *param;
  }
}
*/

unsigned long oil_cpu_flags;

/**
 * oil_cpu_get_flags:
 *
 * Returns a bitmask containing the available CPU features.
 *
 * Returns: the CPU features.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

unsigned int
oil_cpu_get_flags (void)
{
  return oil_cpu_flags;
}

/**
 * OIL_CPU_FLAG_MASK:
 *
 * Mask describing which bits in #OilImplFlag depend on the current
 * CPU.
 */
#define OIL_CPU_FLAG_MASK 0xffff0000

/**
 * oil_impl_is_runnable:
 * @impl: an @OilFunctionImpl
 *
 * Determines whether the function implementation given by @impl
 * can be executed by the current CPU.
 *
 * Returns: 1 if the implementation can be executed, otherwise 0
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

int
oil_impl_is_runnable (OilFunctionImpl *impl)
{
  unsigned int oil_cpu_flags = oil_cpu_get_flags();

  if ((impl->flags & OIL_CPU_FLAG_MASK) & (~oil_cpu_flags))
    return 0;
  return 1;
}

/**
 * oil_class_optimize:
 * @klass: a function class
 *
 * Tests and profiles each implementation for the given function
 * class.  Testing compares the output of running each implementation
 * on random input against the reference implementation for the
 * same input.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
oil_class_optimize (OilFunctionClass * klass)
{
  OilFunctionImpl *impl;
  OilFunctionImpl *min_impl;
  int ret;


  if (klass->reference_impl == NULL) {
    return;
  }
  if (klass->first_impl == NULL) {
    return;
  }

  min_impl = NULL;
  
  for (impl = klass->first_impl; impl; impl = impl->next) {
    if (!oil_impl_is_runnable (impl))
      continue;
  }
  
  if (min_impl == NULL) {
    return;
  }

  klass->chosen_impl = min_impl;
  klass->func = min_impl->func;

}


#ifdef __SYMBIAN32__
EXPORT_C
#endif
 void oil_scalarmultiply_f32_ns (float * d, const float * s1, const float * s2_1, int n)
{
  /*  
  if (_oil_function_class_scalarmultiply_f32_ns.func == NULL) {
    oil_class_optimize (&_oil_function_class_scalarmultiply_f32_ns);
  }
  */
  scalarmultiply_f32_ns_ref(d,(float*) s1,(float*) s2_1, n);
  //((void (*)(float * d, const float * s1, const float * s2_1, int n))(_oil_function_class_scalarmultiply_f32_ns.func))(d, s1, s2_1, n);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

 void
oil_merge_linear_argb (uint32_t * d_n, const uint32_t * s_n, const uint32_t * s2_n, const uint32_t * s3_1, int n)
{
  if (_oil_function_class_merge_linear_argb.func == NULL) {
    oil_class_optimize (&_oil_function_class_merge_linear_argb);
  }
  ((void (*)(uint32_t * d_n, const uint32_t * s_n, const uint32_t * s2_n, const uint32_t * s3_1, int n))(_oil_function_class_merge_linear_argb.func))(d_n, s_n, s2_n, s3_1, n);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif
 void
oil_merge_linear_u8 (uint8_t * d_n, const uint8_t * s_n, const uint8_t * s2_n, const uint32_t * s3_1, int n)
{
  if (_oil_function_class_merge_linear_u8.func == NULL) {
    oil_class_optimize (&_oil_function_class_merge_linear_u8);
  }
  ((void (*)(uint8_t * d_n, const uint8_t * s_n, const uint8_t * s2_n, const uint32_t * s3_1, int n))(_oil_function_class_merge_linear_u8.func))(d_n, s_n, s2_n, s3_1, n);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


 void
oil_resample_linear_argb (uint32_t * d_n, const uint32_t * s_2xn, int n, uint32_t * i_2)
{
  if (_oil_function_class_resample_linear_argb.func == NULL) {
    oil_class_optimize (&_oil_function_class_resample_linear_argb);
  }
  ((void (*)(uint32_t * d_n, const uint32_t * s_2xn, int n, uint32_t * i_2))(_oil_function_class_resample_linear_argb.func))(d_n, s_2xn, n, i_2);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif
 void
oil_resample_linear_u8 (uint8_t * d_n, const uint8_t * s_2xn, int n, uint32_t * i_2)
{
  if (_oil_function_class_resample_linear_u8.func == NULL) {
    oil_class_optimize (&_oil_function_class_resample_linear_u8);
  }
  ((void (*)(uint8_t * d_n, const uint8_t * s_2xn, int n, uint32_t * i_2))(_oil_function_class_resample_linear_u8.func))(d_n, s_2xn, n, i_2);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif


 void
oil_splat_u8 (uint8_t * dest, int dstr, const uint8_t * s1_1, int n)
{
  if (_oil_function_class_splat_u8.func == NULL) {
    oil_class_optimize (&_oil_function_class_splat_u8);
  }
  ((void (*)(uint8_t * dest, int dstr, const uint8_t * s1_1, int n))(_oil_function_class_splat_u8.func))(dest, dstr, s1_1, n);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
oil_splat_u8_ns (uint8_t * dest, const uint8_t * s1_1, int n)
{
  if (_oil_function_class_splat_u8_ns.func == NULL) {
    oil_class_optimize (&_oil_function_class_splat_u8_ns);
  }
  ((void (*)(uint8_t * dest, const uint8_t * s1_1, int n))(_oil_function_class_splat_u8_ns.func))(dest, s1_1, n);
}

OIL_DEFINE_IMPL_REF (scalarmultiply_f32_ns_ref, scalarmultiply_f32_ns);
OIL_DEFINE_IMPL_REF (resample_linear_u8_ref, resample_linear_u8);
OIL_DEFINE_IMPL_REF (resample_linear_argb_ref, resample_linear_argb);
OIL_DEFINE_IMPL_REF (merge_linear_argb_ref, merge_linear_argb);
OIL_DEFINE_IMPL_REF (merge_linear_u8_ref, merge_linear_u8);
OIL_DEFINE_IMPL_REF(splat_u8_ref, splat_u8);
OIL_DEFINE_IMPL_REF(splat_u8_ns_ref, splat_u8_ns);

