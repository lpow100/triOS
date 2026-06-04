#ifndef USTAR_H
#define USTAR_H

#include <lib.h>
#include <text.h>

int tar_lookup(unsigned char *archive, char *filename, char **out);

#endif