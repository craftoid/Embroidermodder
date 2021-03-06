#include "format-ksm.h"
#include "helpers-binary.h"
#include "emb-logging.h"
#include <stdio.h>

static void ksmEncode(unsigned char* b, char dx, char dy, int flags)
{
    if(!b)
    {
        embLog_error("format-ksm.c ksmEncode(), b argument is null\n");
        return;
    }
    if(flags == TRIM)
    {
        b[0] = 128;
        b[1] = 2;
        b[2] = dx;
        b[3] = dy;
    }
    else if(flags == STOP)
    {
        b[0] = 128;
        b[1] = 1;
        b[2] = dx;
        b[3] = dy;
    }
    else
    {
        b[0] = dx;
        b[1] = dy;
    }
}

/*! Reads a file with the given \a fileName and loads the data into \a pattern.
 *  Returns \c true if successful, otherwise returns \c false. */
int readKsm(EmbPattern* pattern, const char* fileName)
{
    int prevStitchType = NORMAL;
    char b[3];
    FILE* file = 0;

    if(!pattern) { embLog_error("format-ksm.c readKsm(), pattern argument is null\n"); return 0; }
    if(!fileName) { embLog_error("format-ksm.c readKsm(), fileName argument is null\n"); return 0; }

    file = fopen(fileName, "rb");
    if(!file)
    {
        embLog_error("format-ksm.c readKsm(), cannot open %s for reading\n", fileName);
        return 0;
    }

    fseek(file, 0x200, SEEK_SET);

    while(fread(b, 1, 3, file) == 3)
    {
        int flags = NORMAL;

        if(((prevStitchType & 0x08) == 0x08) && (b[2] & 0x08) == 0x08)
        {
            flags = STOP;
        }
        else if((b[2] & 0x1F) != 0)
        {
            flags = TRIM;
        }
        prevStitchType = b[2];
        if(b[2] & 0x40)
            b[1] = -b[1];
        if(b[2] & 0x20)
            b[0] = -b[0];
        embPattern_addStitchRel(pattern, b[1] / 10.0, b[0] / 10.0, flags, 1);
    }
    embPattern_addStitchRel(pattern, 0, 0, END, 1);
    fclose(file);
    return 1;
}

/*! Writes the data from \a pattern to a file with the given \a fileName.
 *  Returns \c true if successful, otherwise returns \c false. */
int writeKsm(EmbPattern* pattern, const char* fileName)
{
    FILE* file = 0;
    EmbStitchList* pointer = 0;
    double xx = 0, yy = 0, dx = 0, dy = 0;
    int flags = 0;
    int i;
    unsigned char b[4];

    if(!pattern) { embLog_error("format-ksm.c writeKsm(), pattern argument is null\n"); return 0; }
    if(!fileName) { embLog_error("format-ksm.c writeKsm(), fileName argument is null\n"); return 0; }

    file = fopen(fileName, "wb");
    if(!file)
    {
        embLog_error("format-ksm.c writeKsm(), cannot open %s for writing\n", fileName);
        return 0;
    }
    for(i = 0; i < 0x80; i++)
    {
        binaryWriteInt(file, 0);
    }
    /* write stitches */
    xx = yy = 0;
    pointer = pattern->stitchList;
    while(pointer)
    {
        dx = pointer->stitch.xx - xx;
        dy = pointer->stitch.yy - yy;
        xx = pointer->stitch.xx;
        yy = pointer->stitch.yy;
        flags = pointer->stitch.flags;
        ksmEncode(b, (char)(dx * 10.0), (char)(dy * 10.0), flags);
        fprintf(file, "%c%c", b[0], b[1]);
        pointer = pointer->next;
    }
    fprintf(file, "\x1a");
    fclose(file);
    return 1;
}

/* kate: bom off; indent-mode cstyle; indent-width 4; replace-trailing-space-save on; */
