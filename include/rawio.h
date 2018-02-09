#ifndef RAWIO_H
#define RAWIO_H

#include "codes.h"
#include "io.h"

int rawputc(int channel, char c);
int rawgetc(int channel, char *c);
int rawcanputc(int channel);
int rawcangetc(int channel);
char raw_get_error(int channel);

#endif /* RAWIO_H */
