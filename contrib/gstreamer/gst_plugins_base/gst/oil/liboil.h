#ifndef __LIBOIL__
#define __LIBOIL__

#include<stdint.h>
#include <stdio.h>

IMPORT_C  void resample_linear_u8_ref (uint8_t *dest, uint8_t *src, int n, uint32_t *in);
IMPORT_C  void resample_linear_argb_ref (uint32_t *d, uint32_t *s, int n, uint32_t *in);
IMPORT_C  void merge_linear_argb_ref (uint32_t *d, uint32_t *s1, uint32_t *s2, uint32_t *src3, int n);
//IMPORT_C  void splat_u8_ref (uint8_t *dest, int dstr, uint8_t *param, int n);
IMPORT_C  void oil_scalarmultiply_f32_ns (float * d, const float * s1, const float * s2_1, int n);
IMPORT_C  void oil_merge_linear_argb (uint32_t * d_n, const uint32_t * s_n, const uint32_t * s2_n, const uint32_t * s3_1, int n);
IMPORT_C  void oil_merge_linear_u8 (uint8_t * d_n, const uint8_t * s_n, const uint8_t * s2_n, const uint32_t * s3_1, int n);
IMPORT_C  void oil_resample_linear_argb (uint32_t * d_n, const uint32_t * s_2xn, int n, uint32_t * i_2);
IMPORT_C  void oil_resample_linear_u8 (uint8_t * d_n, const uint8_t * s_2xn, int n, uint32_t * i_2);
IMPORT_C  void oil_splat_u8 (uint8_t * dest, int dstr, const uint8_t * s1_1, int n);
IMPORT_C  void oil_splat_u8_ns (uint8_t * dest, const uint8_t * s1_1, int n);

#endif
