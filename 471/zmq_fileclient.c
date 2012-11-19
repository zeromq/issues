#include <czmq.h>
#define CHUNK_SIZE 1024
#define PIPELINE 10

int main (void)
{
  zctx_t *ctx = zctx_new ();

  void *dealer = zsocket_new (ctx, ZMQ_DEALER);
  zsocket_connect (dealer, "tcp://127.0.0.1:6000");

  // We'll allow up to N chunks in transit at once
  size_t credit = PIPELINE;

  size_t total = 0;     // Total bytes received
  size_t chunks = 0;    // Total chunks received
  size_t offset = 0;    // Offset of next chunk request
  size_t offset_expc = 0;
  
  while (true) {
    while (credit) {
      // Ask for next chunk
      zstr_sendfm (dealer, "fetch");
      zstr_sendfm (dealer, "%ld", offset);
      zstr_sendf (dealer, "%ld", CHUNK_SIZE);
      offset += CHUNK_SIZE;
      credit--;
    }
    zframe_t *chunk = zframe_recv (dealer);
    if (!chunk)
      break; // Shutting down, quit
    
    chunks++;
    credit++;
    
    size_t size = zframe_size (chunk);
    zframe_destroy (&chunk);
    total += size;
    if (size < CHUNK_SIZE)
      break; // Last chunk received; exit
  }
  printf ("%zd chunks received, %zd bytes\n", chunks, total);
  
  zctx_destroy (&ctx);
  return total == 102400? 0: -1;
}