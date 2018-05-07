#include "check.h"
#include "check_helper.h"
#include <pcre.h>
#include <string.h>

/*
#define DEBUG
*/


ret_t check_tag_has_value_matching_regex(ctiff_t * ctif, tag_t tag, const char * regex_string) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  if (ret.returncode != is_valid) return ret;

  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
  switch (datatype) {
    case TIFF_ASCII: {
                       char * val=NULL;
                       uint32 count=0;
                       int r = 0;
                       ret = TIFFGetFieldASCII(ctif, tag, &val, &count);
                       if (ret.returncode != is_valid) return ret;
                       if (0 < count) {
                         for (uint32 i=0; i<count-1; i++) {
                           if (val[i] == '\0' && val[i+1] =='\0') {
                             r = i+1;
                             break;
                           }
                         }
                         if (val[count-1] != '\0') {
                           char array[VALUESTRLEN];
                           snprintf(array, sizeof(array), "'%c' (at position %lu: '%s')", val[count-1], count-1, val);
                           ret = set_value_found_ret(&ret, array);
                           ret.returncode = tagerror_no_zero_as_end_of_string_in_asciivalue;
                           return ret;
                         }
                         if (r != 0) {
                           char array[VALUESTRLEN];
                           snprintf(array, sizeof(array), "'%s' (\\0 at position %i in %u-len)", val, r, count);
                           ret = set_value_found_ret(&ret, array);
                           ret.returncode = tagerror_multiple_zeros_in_asciivalue;
                           return ret;
                         }

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
                           printf("tag %s with count=%u and value='%s' -> rc=%d\n", TIFFTagName(tag), count, val, rc);
#endif
                           pcre_free( re );
                           if (rc >= 0 ) {
                             ret.returncode=is_valid;
                             return ret;
                           } else {
                             switch(rc) {
                               case PCRE_ERROR_NOMATCH:
                                 ret = set_value_found_ret(&ret, val);
                                 free(val);
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
                           ret = set_value_found_ret(&ret, array);
                           ret.returncode = pcre_compile_error;
                           free(val);
                           return ret;
                         }
                       } else {
                         ret.returncode = tagerror_expected_count_iszero;
                         free(val);
                         return ret;
                       }
                     }
    default:  /*  none */
                     {
                       ret = set_value_found_ret( &ret, TIFFTypeName(datatype));
                       ret.returncode = tagerror_unexpected_type_found;
                       return ret;
                     }
  }
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
