/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see file LICENSE)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <string.h>
/*
#define DEBUG
*/

ret_t check_tag_has_some_of_these_values(ctiff_t * ctif, tag_t tag, int count, const unsigned int * values) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  if (ret.returncode != is_valid) return ret;
  const unsigned int * p = values;

  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
  switch (datatype) {
    case TIFF_LONG: { 
                      p = values;
                      ret_t tmp_res;
                      for (int i=0; i< count; i++) {
#ifdef DEBUG
                        printf("### value = %u", *p);
#endif
                        tmp_res = check_tag_has_u32value(ctif, tag, *p);
                        if (tmp_res.returncode == 0) return tmp_res;
                        p++;
                      }
                      uint32 * valp = NULL;
                      uint32 val=0;
                      uint32 vcount=0;
                      ret = TIFFGetFieldLONG(ctif, tag, &valp, &vcount);
                      if (vcount >0) {
                        val = *valp;
                        char value[VALUESTRLEN];
                        snprintf(value, sizeof(value), "%u", val);
                        ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                        ret.returncode = tagerror_value_differs;
                      };
                      return ret;
                      break;
                    }
    case TIFF_SHORT: {
                       p = values;
                       ret_t tmp_res;
                       for (int i=0; i< count; i++) {
#ifdef DEBUG
                         printf("### value = %u", *p);
#endif
                         tmp_res = check_tag_has_u16value(ctif, tag, *p);
                         if (tmp_res.returncode == 0) return tmp_res;
                         p++;
                       }
                       uint16 * valp = NULL;
                       uint16 val=0;
                       uint32 vcount=0;
                       ret = TIFFGetFieldSHORT(ctif, tag, &valp, &vcount);
                       if (vcount >0) {
                         val = *valp;
                         char value[VALUESTRLEN];
                         snprintf(value, sizeof(value), "%u", val);
                         ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                         ret.returncode = tagerror_value_differs;
                       }
                       return ret;
                       break;
                     }
    case TIFF_RATIONAL: {
                          p = values;
                          ret_t tmp_res;
                          for (int i=0; i< count; i++) {
#ifdef DEBUG
                            printf("### value = %u", *p);
#endif
                            tmp_res = check_tag_has_fvalue(ctif, tag, *p);
                            if (tmp_res.returncode == 0) return tmp_res;
                            p++;
                          }
                          float * valp = NULL;
                          float val=0.0f;
                          uint32 vcount=0;
                          ret = TIFFGetFieldRATIONAL(ctif, tag, &valp, &vcount);
                          if (count >0) {
                            val = * valp;
                            char value[VALUESTRLEN];
                            snprintf(value, sizeof(value), "%f", val);
                            ret.value_found = strncpy(ret.value_found, value, VALUESTRLEN);
                            ret.returncode = tagerror_value_differs;
                          }
                          return ret;
                          break;
                        }
    default: /*  none */
                        {
                          ret.value_found = strncpy(ret.value_found, TIFFTypeName(datatype), VALUESTRLEN);
                          ret.returncode = tagerror_unexpected_type_found;
                          return ret;
                        }
  }
  ret.returncode=should_not_occur;
  assert( ret.returncode != should_not_occur);
  return ret;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
