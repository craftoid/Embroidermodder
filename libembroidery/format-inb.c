#include "format-inb.h"
#include "emb-logging.h"
#include "helpers-binary.h"
#include <stdio.h>

/*! Reads a file with the given \a fileName and loads the data into \a pattern.
 *  Returns \c true if successful, otherwise returns \c false. */
int readInb(EmbPattern* pattern, const char* fileName)
{
    FILE* file = 0;
    unsigned char fileDescription[8];
    unsigned char nullVal;
    int stitchCount;
    short width;
    short height;
    short colorCount;
    short unknown3;
    short unknown2;
    short imageWidth;
    short imageHeight;
    unsigned char bytesUnknown[300];
    short nullbyte;
    short left;
    short right;
    short top;
    short bottom;
    int x = 0;
    int y = 0;
    int i;
    int fileLength;

    if(!pattern) { embLog_error("format-inb.c readInb(), pattern argument is null\n"); return 0; }
    if(!fileName) { embLog_error("format-inb.c readInb(), fileName argument is null\n"); return 0; }

    file = fopen(fileName, "rb");
    if(!file)
    {
        embLog_error("format-inb.c readInb(), cannot open %s for reading\n", fileName);
        return 0;
    }

    embPattern_loadExternalColorFile(pattern, fileName);
    fseek(file, 0, SEEK_END);
    fileLength = ftell(file);
    binaryReadBytes(file, fileDescription, 8);
    nullVal = binaryReadByte(file);
    binaryReadInt16(file);
    stitchCount = binaryReadInt32(file);
    width = binaryReadInt16(file);
    height = binaryReadInt16(file);
    colorCount = binaryReadInt16(file);
    unknown3 = binaryReadInt16(file);
    unknown2 = binaryReadInt16(file);
    imageWidth = binaryReadInt16(file);
    imageHeight = binaryReadInt16(file);
    binaryReadBytes(file, bytesUnknown, 300);
    nullbyte = binaryReadInt16(file);
    left = binaryReadInt16(file);
    right = binaryReadInt16(file);
    top = binaryReadInt16(file);
    bottom = binaryReadInt16(file);
    fseek(file, 0x2000, SEEK_SET);
    /* Calculate stitch count since header has been seen to be blank */
    stitchCount = (int)((fileLength - 0x2000) / 3);
    for(i = 0; i < stitchCount; i++)
    {
        unsigned char type;
        int stitch = NORMAL;
        x = binaryReadByte(file);
        y = binaryReadByte(file);
        type = binaryReadByte(file);
        if((type & 0x40) > 0)
            x = -x;
        if((type & 0x10) > 0)
            y = -y;
        if((type & 1) > 0)
            stitch = STOP;
        if((type & 2) > 0)
            stitch = TRIM;
        embPattern_addStitchRel(pattern, x / 10.0, y / 10.0, stitch, 1);
    }
    fclose(file);
    embPattern_addStitchRel(pattern, 0, 0, END, 1);
    embPattern_flipVertical(pattern);
    return 1;
}

/*! Writes the data from \a pattern to a file with the given \a fileName.
 *  Returns \c true if successful, otherwise returns \c false. */
int writeInb(EmbPattern* pattern, const char* fileName)
{
    if(!pattern) { embLog_error("format-inb.c writeInb(), pattern argument is null\n"); return 0; }
    if(!fileName) { embLog_error("format-inb.c writeInb(), fileName argument is null\n"); return 0; }
    return 0; /*TODO: finish writeInb */
}

/* kate: bom off; indent-mode cstyle; indent-width 4; replace-trailing-space-save on; */
