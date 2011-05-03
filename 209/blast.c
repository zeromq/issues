/*  =========================================================================
    blast - futzing tool for 0MQ

    Start a 0MQ service on TCP port 5560, then start blast.

    -------------------------------------------------------------------------
    Copyright (c) 2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

    Blast is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License along
    with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#define BLOCKSIZE   1024
#define BLOCKS      1000000
#define PORT        5560

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <netdb.h>
#include <unistd.h>
#define randof(num) (int) ((float) (num) * random () / (RAND_MAX + 1.0))
typedef unsigned char byte;

int main (void)
{
    struct sockaddr_in sin = { 0 };
    struct servent *sp;
    int handle;

    sin.sin_family = AF_INET;
    handle = socket (AF_INET, SOCK_STREAM, 0);
    assert (handle);

    struct hostent *host = gethostbyname ("127.0.0.1");
    assert (host);
    sin.sin_family = host->h_addrtype;
    memcpy (&sin.sin_addr, host->h_addr, host->h_length);

    sin.sin_port = htons (PORT);

    int rc = connect (handle, (struct sockaddr *) &sin, sizeof (sin));
    assert (rc >= 0);

    int block_nbr;
    byte block [BLOCKSIZE];
    srandom ((unsigned) time (NULL));

    for (block_nbr = 0; block_nbr < BLOCKS; block_nbr++) {
        int byte_nbr;
        for (byte_nbr = 0; byte_nbr < BLOCKSIZE; byte_nbr++)
            block [byte_nbr] = randof (256);

        rc = write (handle, block, BLOCKSIZE);
        if (rc != BLOCKSIZE) {
            printf ("Test failed at count %d\n", block_nbr);
            break;
        }
    }
    if (block_nbr == BLOCKS)
        printf ("Successful!\n");
    return 0;
}