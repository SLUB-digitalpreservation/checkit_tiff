/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/16
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include "validate_icc.h"
#include "ctype.h"
/** checks a ICC tag, see Annex B of http://www.color.org/specification/ICC1v43_2010-12.pdf
 */
ret_t check_icc(ctiff_t * ctif ) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  ret=check_tag_quiet(ctif, TIFFTAG_ICCPROFILE);
  if (ret.returncode != is_valid) return ret;

  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, TIFFTAG_ICCPROFILE);
  uint32 icc_profile_size;
  uint32 count;
  char * icc_profile = NULL;
  switch (ifd_entry.datatype) { /* icc datatype should be undefined (val=7) */
      case TIFF_UNDEFINED: {
                       icc_profile_size = ifd_entry.count;
                       count = ifd_entry.count;
                     /*  offset */
                      if (ifd_entry.value_or_offset == is_offset) {
                        offset_t offset;
                        ret = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype, &offset);
                        if (ret.returncode != is_valid) return ret;
                        icc_profile = (char *)offset.data32p;
                      } else {
                        ret.returncode = tagerror_encoded_as_value_excited_space;
                        return ret;
                      }
                       break;
                     }
    default: { /*  none */
               ret = set_value_found_ret( &ret, TIFFTypeName(ifd_entry.datatype));
               ret.returncode = tagerror_unexpected_type_found;
               return ret;
               break;
             };
  }

  //printf("DEBUG: iccprofile_size=%i\n", icc_profile_size);
  // printf("DEBUG: iccprofile='%s'\n", icc_profile);
  /* DEBUG
  char * p = icc_profile;
  int i=0;
  for (i = 0; i< icc_profile_size; i++, p++) {
    if (0 == i % 8) printf("|");
    printf("%c(%0x) ", (isalnum(*p)?*p:' '),*p);
  }
  printf("\n");
  */
  char * errmessage = malloc(sizeof(char) * VALUESTRLEN);
  unsigned long errsize = VALUESTRLEN;
  icc_returncode_t icc_ret = parse_icc(icc_profile_size, icc_profile, errsize, errmessage);
  if (NULL != icc_profile) free(icc_profile);
  ret.returncode = should_not_occur; 
  switch (icc_ret) { /*  map between returncodes icc profile and tag check */
    case icc_is_valid: ret.returncode = is_valid; break; 
    case icc_error_profileclass: ret.returncode = iccerror_profileclass; break;
    case icc_error_colorspacedata: ret.returncode = iccerror_colorspacedata; break;
    case icc_error_connectionspacedata: ret.returncode = iccerror_connectionspacedata; break;
    case icc_error_primaryplatformsignature: ret.returncode = iccerror_primaryplatformsignature; break;
    case icc_error_header_1v43_2010: ret.returncode = iccerror_header_1v43_2010; break;
    case icc_error_header_v240_v430: ret.returncode = iccerror_header_v240_v430; break;
    case icc_error_header_generic: ret.returncode = iccerror_header_generic; break;
    case icc_error_preferredcmmtype: ret.returncode = iccerror_preferredcmmtype; break;
    case icc_error_committed_size_differs: ret.returncode = iccerror_committed_size_differs; break;
    case icc_error_header_v500_2016: ret.returncode = iccerror_header_v500_2016; break;
    case icc_error_header_version_undefined: ret.returncode = iccerror_header_version_undefined; break;
    case icc_error_header_version_outdated: ret.returncode = iccerror_header_version_outdated; break;
    case icc_should_not_occur:  ret.returncode = should_not_occur; break;
  }
  ret = set_value_found_ret(&ret, errmessage);
  free (errmessage);
  assert( ret.returncode != should_not_occur);
  return ret;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
