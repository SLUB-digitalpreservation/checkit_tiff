/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE

#include "check.h"
#include "check_helper.h"
#include <assert.h>
/* 
#define DEBUG
*/

ret_t check_tag_has_value_in_range(ctiff_t * ctif, tag_t tag, unsigned int a, unsigned int b) {
  ret_t ret = get_empty_ret();

  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  if (ret.returncode != is_valid) return ret;
  if (a > b) { unsigned int c=a; a=b; b=c; }
    TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
    switch (datatype) {
      case TIFF_LONG: {
                        uint32 * valp = NULL;
                        uint32 val;
                        int found;
                        ret=TIFFGetFieldLONG(ctif, tag, &valp, &found);
                        if (1 == found) {
                          val = *valp;
                          if ((val >= a && val <= b )) {
                            ret.returncode=is_valid;
                            free( valp);
                            return ret;
                          } else {
                            free(valp);
                            char value[VALUESTRLEN];
                            snprintf(value, sizeof(value), "%u", val);
                            ret = set_value_found_ret (&ret, value);
                            ret.returncode = tagerror_value_differs;
                            return ret;
                          }
                        } else {
                          if (NULL != valp) {
                            free(valp);
                            valp=NULL;
                          }
                          ret.returncode = tagerror_value_not_found;
                          return ret;
                        }

                        break;
                      }
      case TIFF_SHORT: {
                         uint16 * valp = NULL;
                         uint16 val;
                         int found;
                         ret =TIFFGetFieldSHORT(ctif, tag, &valp, &found);
                         if (1 == found) {
                           val = *valp;
                           if ((val >= a && val <= b )) {
                             ret.returncode=is_valid;
                             free( valp);
                             return ret;
                           } else {
                             free( valp);
                             char value[VALUESTRLEN];
                             snprintf(value, sizeof(value), "%u", val);
                             ret = set_value_found_ret (&ret, value);
                             ret.returncode = tagerror_value_differs;
                             return ret;
                           }
                         } else {
                           if (NULL != valp) {
                             free(valp);
                             valp=NULL;
                           }
                           ret.returncode = tagerror_value_not_found;
                           return ret;
                         }

                         break;
                       }
      case TIFF_RATIONAL: {
                            float * valp = NULL;
                            float val;
                            int found;
                            ret=TIFFGetFieldRATIONAL(ctif, tag, &valp, &found);
                            if (1 == found) {
                              val = * valp;
                              if ((val >= a && val <= b )) {
                                ret.returncode=is_valid;
                                free( valp);
                                return ret;
                              } else {
                                free( valp);
                                char value[VALUESTRLEN];
                                snprintf(value, sizeof(value), "%f", val);
                                ret = set_value_found_ret (&ret, value);
                                ret.returncode = tagerror_value_differs;
                                return ret;
                              }
                            } else {
                              if (NULL != valp) {
                                free(valp);
                                valp=NULL;
                              }
                              ret.returncode = tagerror_value_not_found;
                              return ret;
                            }

                            break;
                          }
      default: /*  none */
                          {
                            ret = set_value_found_ret(&ret, TIFFTypeName(datatype));
                            ret.returncode = tagerror_unexpected_type_found;
                            return ret;
                          }
    }
    ret.returncode=should_not_occure;
    return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
