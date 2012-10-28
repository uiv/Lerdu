
#ifndef __AUDIORESAMPLE_BUFFER_H__
#define __AUDIORESAMPLE_BUFFER_H__

#include <glib.h>

typedef struct _AudioresampleBuffer AudioresampleBuffer;
typedef struct _AudioresampleBufferQueue AudioresampleBufferQueue;

struct _AudioresampleBuffer
{
  unsigned char *data;
  int length;

  int ref_count;

  AudioresampleBuffer *parent;

  void (*free) (AudioresampleBuffer *, void *);
  void *priv;
  void *priv2;
};

struct _AudioresampleBufferQueue
{
  GList *buffers;
  int depth;
  int offset;
};
#ifdef __SYMBIAN32__
IMPORT_C
#endif


AudioresampleBuffer *   audioresample_buffer_new                (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

AudioresampleBuffer *   audioresample_buffer_new_and_alloc      (int size);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

AudioresampleBuffer *   audioresample_buffer_new_with_data      (void *data, int size);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

AudioresampleBuffer *   audioresample_buffer_new_subbuffer      (AudioresampleBuffer * buffer, 
                                                                 int offset,
                                                                 int length);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void                    audioresample_buffer_ref                (AudioresampleBuffer * buffer);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void                    audioresample_buffer_unref              (AudioresampleBuffer * buffer);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


AudioresampleBufferQueue *      
                        audioresample_buffer_queue_new          (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void                    audioresample_buffer_queue_free         (AudioresampleBufferQueue * queue);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

int                     audioresample_buffer_queue_get_depth    (AudioresampleBufferQueue * queue);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

int                     audioresample_buffer_queue_get_offset   (AudioresampleBufferQueue * queue);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void                    audioresample_buffer_queue_push         (AudioresampleBufferQueue * queue,
                                                                 AudioresampleBuffer * buffer);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

AudioresampleBuffer *   audioresample_buffer_queue_pull         (AudioresampleBufferQueue * queue, int len);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

AudioresampleBuffer *   audioresample_buffer_queue_peek         (AudioresampleBufferQueue * queue, int len);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void                    audioresample_buffer_queue_flush        (AudioresampleBufferQueue * queue);

#endif
