/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include "check_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/*
#define DEBUG
*/

char * secstrcat (char * dest, const char * src, int maxsize) {
  if (NULL == dest) {
    fprintf(stderr, "internal error, dest is NULL!\n");
    exit(EXIT_FAILURE);
  }
  if (NULL == src) {
    dest="";
  } else {
    int destsize = strlen(dest);
    int srclen = strlen(src);
    //fprintf(stderr, "1dest='%s' , src='%s', destsize=%i, srclen=%i\n", dest, src, destsize, srclen);
    if (destsize+srclen < maxsize) {
      strcat( dest, src);
    }
    destsize = strlen(dest);
    //fprintf(stderr, "2dest='%s' , src='%s', destsize=%i, srclen=%i\n", dest, src, destsize, srclen);
  }
  return dest;
}

const char * float2str(float v) {
  char array[VALUESTRLEN];
  snprintf(array, sizeof(array), "%f", v);
  return strdup(array);
}

const char* tag2str(tag_t tag) {
 char array[VALUESTRLEN];
  snprintf(array, sizeof(array), "%u (%s)", tag, TIFFTagName(tag));
  return strdup(array);
}

const char* int2str(int v) {
 char array[VALUESTRLEN];
  snprintf(array, sizeof(array), "%u", v);
  return strdup(array);
}

const char* frac2str(int d, int n) {
  char array[VALUESTRLEN];
  snprintf(array, sizeof(array), "%u/%u", d, n);
  return strdup(array);
}

const char* range2str(int d, int n) {
  char array[VALUESTRLEN];
  snprintf(array, sizeof(array), "<%u-%u>", d, n);
  return strdup(array);
}

void TIFFSwabShort(uint16 *a) {
  uint16 b = ((*a & 0xff) << 8) | ((*a >> 8) & 0xff);
  *a=b;
}

void TIFFSwabLong(uint32 *a) {
  uint8 b0 = (*a) & 0xff;
  uint8 b1 = (*a >> 8) & 0xff;
  uint8 b2 = (*a >> 16) & 0xff;
  uint8 b3 = (*a >> 24) & 0xff;
  uint32 b = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
  *a=b;
}

long long fsize(int fd) {
  struct stat st;
  fstat(fd, &st);
  return st.st_size;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
