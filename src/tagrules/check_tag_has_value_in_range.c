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
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);
  if (a > b) { unsigned int c=a; a=b; b=c; }
    TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
    switch (datatype) {
      case TIFF_LONG: {
                        uint32 * valp = NULL;
                        uint32 val;
                        int found=TIFFGetFieldLONG(ctif, tag, &valp);
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
                            ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                            ret.returncode = tagerror_value_differs;
                            return ret;
                          }
                        } else {
                          if (NULL != valp) {
                            free(valp);
                            valp=NULL;
                          }
                          ret.value_found = strncpy(ret.value_found, "not found", VALUESTRLEN);
                          ret.returncode = tagerror_value_not_found;
                          return ret;
                        }

                        break;
                      }
      case TIFF_SHORT: {
                         uint16 * valp = NULL;
                         uint16 val;
                         int found=TIFFGetFieldSHORT(ctif, tag, &valp);
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
                             ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                             ret.returncode = tagerror_value_differs;
                             return ret;
                           }
                         } else {
                           if (NULL != valp) {
                             free(valp);
                             valp=NULL;
                           }
                           ret.value_found = strncpy(ret.value_found, "not found", VALUESTRLEN);
                           ret.returncode = tagerror_value_not_found;
                           return ret;
                         }

                         break;
                       }
      case TIFF_RATIONAL: {
                            float * valp = NULL;
                            float val;
                            int found=TIFFGetFieldRATIONAL(ctif, tag, &valp);
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
                                ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                                ret.returncode = tagerror_value_differs;
                                return ret;
                              }
                            } else {
                              if (NULL != valp) {
                                free(valp);
                                valp=NULL;
                              }
                              ret.value_found = strncpy(ret.value_found, "not found", VALUESTRLEN);
                              ret.returncode = tagerror_value_not_found;
                              return ret;
                            }

                            break;
                          }
      default: /*  none */
                          {
                            ret.value_found = strncpy(ret.value_found, TIFFTypeName(datatype), VALUESTRLEN);
                            ret.returncode = tagerror_unexpected_type_found;
                            return ret;
                          }
    }
    ret.returncode=should_not_occure;
    return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
