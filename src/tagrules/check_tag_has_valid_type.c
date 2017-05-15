/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <assert.h>
/* #define DEBUG */

/* checks if TIF has a specified tag */
ret_t check_tag_has_valid_type(ctiff_t * ctif, tag_t tag) {
  GET_EMPTY_RET(ret)

  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  assert(ret.returncode != should_not_occur);
  if (ret.returncode != is_valid) return ret;

  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
#ifdef DEBUG
  printf("### datatype=%i \n", datatype);
#endif
  int res = 0;
  switch (tag) {
    case TIFFTAG_ARTIST:            res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_BADFAXLINES:       res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_BITSPERSAMPLE:     res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_CELLLENGTH:        res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_CELLWIDTH:         res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_COLORMAP:          res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_COMPRESSION:       res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_COPYRIGHT:         res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_DATETIME:          res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_DOCUMENTNAME:      res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_DOTRANGE:          res=(datatype  ==  TIFF_BYTE) || (datatype ==  TIFF_SHORT);; break;
    case TIFFTAG_EXIFIFD:           res=(datatype  ==  TIFF_LONG); break; /* Exif Spec 2.3 */
    case TIFFTAG_EXTRASAMPLES:      res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_FILLORDER:         res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_FREEBYTECOUNTS:    res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_FREEOFFSETS:       res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_GPSIFD:            res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_GRAYRESPONSECURVE: res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_GRAYRESPONSEUNIT:  res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_HALFTONEHINTS:     res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_HOSTCOMPUTER:      res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_ICCPROFILE:        res=(datatype  ==  TIFF_UNDEFINED); break;
    case TIFFTAG_IMAGEDEPTH:        res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_IMAGEDESCRIPTION:  res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_IMAGELENGTH:       res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT);; break;
    case TIFFTAG_IMAGEWIDTH:        res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT);; break;
    case TIFFTAG_INKNAMES:          res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_INKSET:            res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_INTEROPERABILITYIFD:res=(datatype ==  TIFF_LONG); break; /* Exif Spec 2.3 */
    case TIFFTAG_JPEGPROC:          res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_MAKE:              res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_MAXSAMPLEVALUE:    res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_MINSAMPLEVALUE:    res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_MODEL:             res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_NUMBEROFINKS:      res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_ORIENTATION:       res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_OSUBFILETYPE:      res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_PAGENUMBER:        res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_PHOTOMETRIC:       res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_PLANARCONFIG:      res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_PREDICTOR:         res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_PRIMARYCHROMATICITIES:  res=(datatype  ==  TIFF_RATIONAL); break;
    case TIFFTAG_REFERENCEBLACKWHITE:    res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_RESOLUTIONUNIT:    res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_RICHTIFFIPTC:      res=(datatype == TIFF_UNDEFINED) || (datatype == TIFF_BYTE); break;
    case TIFFTAG_ROWSPERSTRIP:      res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT); break;
    case TIFFTAG_SAMPLEFORMAT:      res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_SAMPLESPERPIXEL:   res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_SMAXSAMPLEVALUE:   res=(datatype  ==  TIFF_ANY); break;
    case TIFFTAG_SMINSAMPLEVALUE:   res=(datatype  ==  TIFF_ANY); break;
    case TIFFTAG_SOFTWARE:          res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_STRIPBYTECOUNTS:   res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT); break;
    case TIFFTAG_STRIPOFFSETS:      res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT); break;
    case TIFFTAG_SUBFILETYPE:       res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_SUBIFD:            res=(datatype  ==  TIFF_LONG); break; /* TIFF technical note 1 */
    case TIFFTAG_T4OPTIONS:         res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_T6OPTIONS:         res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_TARGETPRINTER:     res=(datatype  ==  TIFF_ASCII); break;
    case TIFFTAG_THRESHHOLDING:     res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_TILEBYTECOUNTS:    res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT); break;
    case TIFFTAG_TILELENGTH:        res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT); break;
    case TIFFTAG_TILEOFFSETS:       res=(datatype  ==  TIFF_LONG); break;
    case TIFFTAG_TILEWIDTH:         res=(datatype  ==  TIFF_LONG) || (datatype ==  TIFF_SHORT); break;
    case TIFFTAG_TRANSFERFUNCTION:  res=(datatype  ==  TIFF_SHORT); break;
    case TIFFTAG_WHITEPOINT:        res=(datatype  ==  TIFF_RATIONAL); break;
    case TIFFTAG_XRESOLUTION:       res=(datatype  ==  TIFF_RATIONAL); break;
    case TIFFTAG_YRESOLUTION:       res=(datatype  ==  TIFF_RATIONAL); break;
    default: {
#ifdef DEBUG
               printf("for tag %i no explicite type check implemented\n");
#endif
               ret = set_value_found_ret(&ret, TIFFTypeName(datatype));
               ret.returncode = tagwarn_type_of_unknown_tag_could_not_be_checked;
               return ret;
             };
  }
  if (!res) {
               ret = set_value_found_ret(&ret, TIFFTypeName(datatype));
               ret.returncode = tagerror_unexpected_type_found;
               return ret;
  } else {
               ret.returncode = is_valid;
               return ret;
  }
  /* we check only count, because we evaluate only int-values */
  ret.returncode = should_not_occur;
  assert(ret.returncode != should_not_occur);
  return ret;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
