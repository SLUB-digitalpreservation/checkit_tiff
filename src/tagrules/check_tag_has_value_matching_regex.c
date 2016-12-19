#include "check.h"
#include "check_helper.h"
#include <pcre.h>
#include <string.h>

/*
#define DEBUG
*/


ret_t check_tag_has_value_matching_regex(ctiff_t * ctif, tag_t tag, const char * regex_string) {
  //printf("check if tag %u (%s) has value matching regex '%s'\n", tag, TIFFTagName(tif, tag), regex_string);
  tifp_check( ctif);
  char msg[EXPECTSTRLEN];
  snprintf(msg, sizeof(msg), "has  value matching regex '%s'", regex_string);
  tif_rules_tag(tag, strdup(msg));
  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
  switch (datatype) {
    case TIFF_ASCII: {
                       char * val=NULL;
                       int count = TIFFGetFieldASCII(ctif, tag, &val);
                       if (0 < count) {
#define OVECCOUNT 30    /* should be a multiple of 3 */
                         pcre *re;
                         int erroffset;
                         const char * errorcode;
                         re = pcre_compile(
                             regex_string,                /* the pattern */
                             0,                      /* default options */
                             &errorcode,             /* for error code */
                             &erroffset,             /* for error offset */
                             NULL);                  /* no compile context */
                         if (NULL != re) {

                           int ovector[OVECCOUNT];
                           /*  PCRE_NOTEMPTY: An empty string is not a valid match */
                           int rc = pcre_exec( re, NULL, val, count, 0,PCRE_NOTEMPTY , ovector, OVECCOUNT);
#ifdef DEBUG
                           printf("tag %s with count=%d and value='%s' -> rc=%d\n", TIFFTagName(tag), count, val, rc);
#endif
                           pcre_free( re );
                           if (rc >= 0 ) {
                             ret_t res;
                             res.returnmsg=NULL;
                             res.returncode=0;
                             return res;
                           } else {
                             switch(rc) {
                               case PCRE_ERROR_NOMATCH:
                                 return tif_fails_tag( tag, strdup(regex_string), val);
                                 break;
                                 /*
                                    Handle other special cases if you like
                                    */
                               default:
                                 return tif_fails_tag( tag, strdup(regex_string), val);
                                 // tif_fails("tag %u with value '%s' called regex '%s' with matching error %d\n", tag, val, regex_string, rc);
                                 break;
                             }
                         }
                         } else {
                            char array[VALUESTRLEN];
                            snprintf(array, sizeof(array), "regex '%s' compile error: %s at offset: %i\n",regex_string, errorcode, erroffset);
                           return tif_fails(array);
                         }
                       } else {
                         return tif_fails_tag( tag, "should exist, because defined", "");
                       }
                     }
    default:  /*  none */
                     {
                       char array[VALUESTRLEN];
                       snprintf(array, sizeof(array), "but was:%i\n", datatype);
                       return tif_fails_tag( tag, "of type ASCII", array);
                     }
  }
}
