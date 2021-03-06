#include "format-phc.h"
#include "format-pec.h"
#include "emb-logging.h"
#include "helpers-binary.h"
#include <stdio.h>

/*! Reads a file with the given \a fileName and loads the data into \a pattern.
 *  Returns \c true if successful, otherwise returns \c false. */
int readPhc(EmbPattern* pattern, const char* fileName)
{
    int colorChanges, version, bytesInSection2;
    unsigned short pecOffset, bytesInSection, bytesInSection3;
    char pecAdd;
    FILE* file = 0;
    int i;

    if(!pattern) { embLog_error("format-phc.c readPhc(), pattern argument is null\n"); return 0; }
    if(!fileName) { embLog_error("format-phc.c readPhc(), fileName argument is null\n"); return 0; }

    file = fopen(fileName, "rb");
    if(!file)
    {
        embLog_error("format-phc.c readPhc(), cannot open %s for reading\n", fileName);
        return 0;
    }

    fseek(file, 0x07, SEEK_SET);
    version = binaryReadByte(file) - 0x30; /* converting from ansi number */
    fseek(file, 0x4D, SEEK_SET);
    colorChanges = binaryReadUInt16(file);

    for(i = 0; i < colorChanges; i++)
    {
        EmbThread t = pecThreads[(int)binaryReadByte(file)];
        embPattern_addThread(pattern, t);
    }
    fseek(file, 0x2B, SEEK_SET);
    pecAdd = binaryReadByte(file);
    binaryReadUInt32(file); /* file length */
    pecOffset = binaryReadUInt16(file);
    fseek(file, pecOffset + pecAdd, SEEK_SET);
    bytesInSection = binaryReadUInt16(file);
    fseek(file, bytesInSection, SEEK_CUR);
    bytesInSection2 = binaryReadUInt32(file);
    fseek(file, bytesInSection2, SEEK_CUR);
    bytesInSection3 = binaryReadUInt16(file);
    fseek(file, bytesInSection3 + 0x12, SEEK_CUR);

    readPecStitches(pattern, file);

    embPattern_addStitchRel(pattern, 0.0, 0.0, END, 1);
    fclose(file);
    embPattern_flipVertical(pattern);
    return 1; /*TODO: finish ReadPhc */
}

/*! Writes the data from \a pattern to a file with the given \a fileName.
 *  Returns \c true if successful, otherwise returns \c false. */
int writePhc(EmbPattern* pattern, const char* fileName)
{
    if(!pattern) { embLog_error("format-phc.c writePhc(), pattern argument is null\n"); return 0; }
    if(!fileName) { embLog_error("format-phc.c writePhc(), fileName argument is null\n"); return 0; }
    return 0; /*TODO: finish writePhc */
}

/* kate: bom off; indent-mode cstyle; indent-width 4; replace-trailing-space-save on; */
