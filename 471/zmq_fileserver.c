#include <czmq.h>
#define CHUNK_SIZE 1024
#define PIPELINE 10

static void
free_chunk (void *data, void *arg)
{
  free (data);
}

int main (void)
{
    zctx_t *ctx = zctx_new ();

    FILE *file = fopen ("testdata", "r");
    assert (file);

    void *router = zsocket_new (ctx, ZMQ_ROUTER);
    zsocket_set_hwm (router, PIPELINE * 2);
    zsocket_bind (router, "tcp://*:6000");
    while (true) {
        // First frame in each message is the sender identity
        zframe_t *identity = zframe_recv (router);
        if (!identity)
            break; // Shutting down, quit

        // Second frame is "fetch" command
        char *command = zstr_recv (router);
        assert (streq (command, "fetch"));
        free (command);

        // Third frame is chunk offset in file
        char *offset_str = zstr_recv (router);
        size_t offset = atoi (offset_str);
        free (offset_str);

        // Fourth frame is maximum chunk size
        char *chunksz_str = zstr_recv (router);
        size_t chunksz = atoi (chunksz_str);
        free (chunksz_str);

        // Read chunk of data from file
        fseek (file, offset, SEEK_SET);
        byte *data = malloc (chunksz);
        assert (data);

        // Send resulting chunk to client
        size_t size = fread (data, 1, chunksz, file);
        zframe_t *chunk = zframe_new_zero_copy (data, size, free_chunk, NULL);
        zframe_send (&identity, router, ZFRAME_MORE);
        zframe_send (&chunk, router, 0);
    }
    fclose (file);

    zctx_destroy (&ctx);
    return 0;
}