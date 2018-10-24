#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#include "base64.h"

#define BUF_GROW 1024

static void exit_with_help(int status)
{
    printf("b64 – convert data TO and FROM base64 (default: TO).\n");
    printf("Usage: b64 [options]\n");
    printf("Options:\n");
    printf("  -d   base64-decode stdin to stdout.\n");
    printf("  -h   print this help screen and exit.\n");
    exit(status);
}

static char *read_input(FILE *f, size_t *psize)
{
    int c;
    size_t insize, sz = 0;
    char *bp = NULL, *cp = NULL, *ep = NULL;

    while ((c = fgetc(f)) != EOF)
    {
        if (cp >= ep)
        {
            size_t nsz = sz == 0 ? BUF_GROW : sz * 2;
            char *np = realloc(bp, nsz);
            if (np == NULL)
            {
                perror("readin realloc");
                exit(1);
            }
            cp = np + (cp - bp);
            bp = np;
            ep = np + nsz;
            sz = nsz;
        }
        *cp++ = (char) c;
    }
    *psize = cp - bp;
    return bp;
}

static int encode(FILE *f)
{
    size_t insize;
    char *outbuf, *inbuf = read_input(f, &insize);
    size_t outsize = base64_encode_alloc(inbuf, insize, &outbuf);
    if (outbuf == NULL)
    {
        if (outsize == 0 && insize != 0)
        {
            fprintf(stderr, "encode: input too long\n");
            return 1;
        }
        fprintf(stderr, "encode: allocation failure\n");
    }
    fwrite(outbuf, outsize, 1, stdout);
    free(inbuf);
    free(outbuf);
    return 0;
}

static int decode(FILE *f)
{
    size_t outsize, insize;
    char *outbuf, *inbuf = read_input(f, &insize);
    bool ok = base64_decode_alloc(inbuf, insize, &outbuf, &outsize);
    if (!ok)
    {
        fprintf(stderr, "decode: input not base64\n");
        return 1;
    }
    if (outbuf == NULL)
    {
        fprintf(stderr, "decode: allocation failure\n");
        return 1;
    }
    fwrite(outbuf, outsize, 1, stdout);
    free(inbuf);
    free(outbuf);
    return 0;
}

int main(int argc, char *argv[])
{
    int c;
    bool tob64 = true;

    while ((c = getopt(argc, argv, "dh")) != -1)
    {
        switch (c)
        {
            case 'd':
                tob64 = false;
                break;
            case 'h':
            default:
                exit_with_help(c == 'h' ? 0 : 1);
        }
    }
    return tob64 ? encode(stdin) : decode(stdin);
}

