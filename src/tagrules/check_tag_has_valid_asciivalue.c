/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
/* 
#define DEBUG 
*/

/* checks if TIF with tag and type ASCII */
ret_t check_tag_has_valid_asciivalue(ctiff_t * ctif, tag_t tag) {
  tifp_check( ctif);
    TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
#ifdef DEBUG
  printf("### datatype=%i \n", datatype);
#endif
  char *string=NULL;
  uint32 count=0;
  int r = 0;
  if (datatype == TIFF_ASCII) {
    //printf("check if tag %u (%s) has valid asciivalue\n", tag, TIFFTagName(tif, tag));
    tif_rules_tag(tag, "has valid asciivalue");
    char * val=NULL;
    int count = TIFFGetFieldASCII(ctif, tag, &val);
    if (0 < count) { /* there exists a tag */
      for (int i=0; i<count; i++) {
        if (string[i] == '\0') {
          r = i+1;
          break;
        }
      }
    }
  }
  if (0 != r) {
    char array[VALUESTRLEN];
    snprintf(array, sizeof(array), "incorrect asciivalue (\\0 at position %i in %i-len String)", r, count);
    return tif_fails_tag( tag, "", array);
  } else {
    ret_t res;
    res.returnmsg=NULL;
    res.returncode=0;
    return res;
  }
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
