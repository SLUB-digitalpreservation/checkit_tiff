#include "check.h"
#include "check_helper.h"
#include <pcre.h>
#include <string.h>

/*
#define DEBUG
*/


ret_t check_tag_has_value_matching_regex(ctiff_t * ctif, tag_t tag, const char * regex_string) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);
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
                           int rc = pcre_exec( re, NULL, val, count-1, 0,PCRE_NOTEMPTY, ovector, OVECCOUNT);
#ifdef DEBUG
                           printf("tag %s with count=%d and value='%s' -> rc=%d\n", TIFFTagName(tag), count, val, rc);
#endif
                           pcre_free( re );
                           if (rc >= 0 ) {
                             ret.returncode=is_valid;
                             return ret;
                           } else {
                             switch(rc) {
                               case PCRE_ERROR_NOMATCH:
                                 ret.value_found = strncpy(ret.value_found, val, VALUESTRLEN);
                                 ret.returncode = tagerror_pcre_nomatch;
                                 return ret;
                                 break;
                                 /*
                                    Handle other special cases if you like
                                    */
                             }
                         }
                         } else {
                           char array[VALUESTRLEN];
                           snprintf(array, sizeof(array), "regex '%s' compile error: %s at offset: %i\n",regex_string, errorcode, erroffset);
                           ret.value_found = strncpy(ret.value_found, array, VALUESTRLEN);
                           ret.returncode = pcre_compile_error;
                           return ret;
                         }
                       } else {
                         ret.returncode = tagerror_expected_count_iszero;
                         return ret;
                       }
                     }
    default:  /*  none */
                     {
                       ret.value_found = strncpy(ret.value_found, TIFFTypeName(datatype), VALUESTRLEN);
                       ret.returncode = tagerror_unexpected_type_found;
                       return ret;
                     }
  }
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
