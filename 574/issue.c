#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
int main( int nargs, char** args )
{
    int i, N = 1024;
    void *ctx = zmq_ctx_new();
    void **socks;
    if ( nargs > 1 ) N = atoi( args[1] );
    socks = calloc( N, sizeof( void* ));
    printf( "Creating %i sockets\n", N );
    for ( i = 0; i < N; ++i )
    {
        socks[i] = zmq_socket( ctx, ZMQ_PAIR );
        if ( !socks[i] ) {
            printf( "%i: %s\n", i, zmq_strerror( zmq_errno()));
            break;
        }
    }
    N = i;
    printf( "  %i sockets created\n",i );
    for ( i = 0; i < N; ++i )
        if ( socks[i] )
            zmq_close( socks[i] );
    free( socks );
    puts( "Destroy" );
    zmq_ctx_destroy( ctx );
    return 0;
}
