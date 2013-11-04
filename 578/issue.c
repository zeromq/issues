#include <zmq.h>
#include <zmq_utils.h>

int main (void)
{
    //  We don't care what's in this buffer, it has no impact on the test
    const size_t test_size = 500000;
    uint8_t test_data [test_size];
    uint8_t decoded [test_size];
    char encoded [test_size * 5 / 4];

    printf ("\nEncoding binary to string...");
    fflush (stdout);
    zmq_z85_encode (encoded, test_data, test_size);

    printf ("\nDecoding string to binary...");
    fflush (stdout);
    zmq_z85_decode (decoded, encoded);

    if (memcmp (test_data, decoded, test_size) == 0)
        puts (" OK");
    else
        puts (" Failed");

    return 0;
}
