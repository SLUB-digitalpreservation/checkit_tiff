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
#include "check_renderer.h"
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/*
#define DEBUG
*/

char * secstrcat (char * dest, const char * src, size_t * maxsize_p) {
  assert(NULL != maxsize_p);
  size_t maxsize = *maxsize_p;
  if (NULL == dest) {
    fprintf(stderr, "internal error, dest is NULL!\n");
    exit(EXIT_FAILURE);
  }
  if (NULL == src) {
  } else {
    int destsize = strnlen(dest, maxsize);
    int srclen = strnlen(src, maxsize);
    if (destsize+srclen < maxsize-1) {
    	strncat( dest, src, srclen);
    } else {
      int new_maxsize=(((destsize+srclen) / RENDERSIZE)+1) * RENDERSIZE-1; 
      if (new_maxsize < 4096*RENDERSIZE && new_maxsize > 0) {
        char * dest_new = NULL;
	dest_new = realloc(dest, (new_maxsize+1) * sizeof(char));
	if (NULL == dest_new) {
	  perror( "Could not realloc memory");
	  exit( could_not_allocate_memory ); 
	} else {
	  *maxsize_p = new_maxsize;
	  dest = dest_new;
	  strncat( dest, src, destsize +srclen);
	}	
      }
    }
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


ret_t set_value_found_ret (ret_t * rp, const char * msg) {
  assert( NULL != rp);
  assert( NULL != msg);
  rp->value_found=malloc(sizeof(char) * VALUESTRLEN);
  if (NULL == rp->value_found) {
    rp->returncode=could_not_allocate_memory;
  } else {
    memset(rp->value_found, '\0', VALUESTRLEN);
    // printf("DEBUG='%s'\n", msg);
    strncpy(rp->value_found, msg, VALUESTRLEN-1);
    // printf("DEBUG2='%s'\n", rp->value_found);
  }
  return *rp;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
