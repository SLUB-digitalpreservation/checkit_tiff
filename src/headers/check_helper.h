/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#ifndef _FIXIT_TIFF_CHECK_HELPER
#define _FIXIT_TIFF_CHECK_HELPER
#include <stdlib.h>
//#include <tiff.h>
//#include <tiffio.h>
#define TIFF_ANY 0
void TIFFSwabShort(uint16 *a);
void TIFFSwabLong(uint32 *a);

typedef enum{ render_default, render_debug, render_ansi, render_xml} render_type;
/* helper */
long long fsize(int fd);
ret_t check_tag_has_fvalue(ctiff_t * ctif, tag_t tag, float value);
ret_t check_tag_has_u16value(ctiff_t * ctif, tag_t tag, uint16 value);
ret_t check_tag_has_u32value(ctiff_t * ctif, tag_t tag, uint32 value);
const char * TIFFTypeName( uint16 tagtype );
const char * TIFFTagName (tag_t tag);
uint32 TIFFGetRawTagTypeListEntry( ctiff_t * ctif, int tagidx );
tag_t TIFFGetRawTagListEntry( ctiff_t * ctif, int tagidx ) ;
int TIFFGetRawTagListCount (ctiff_t * ctif, uint32 ifdpos) ;
TIFFDataType TIFFGetRawTagType(ctiff_t * ctif, tag_t tag);
ifd_entry_t TIFFGetRawIFDEntry( ctiff_t * ctif, tag_t tag);
ifd_entry_t TIFFGetRawTagIFDListEntry( ctiff_t * ctif, int tagidx );
int TIFFGetRawTagListIndex(ctiff_t * ctif, tag_t tag);
int TIFFGetFieldASCII(ctiff_t * ctif, tag_t tag, char** string_p);
int TIFFGetFieldLONG(ctiff_t * ctif, tag_t tag, uint32 ** long_p);
int TIFFGetFieldSHORT(ctiff_t * ctif, tag_t tag, uint16 ** short_p);
int TIFFGetFieldRATIONAL(ctiff_t * ctif, tag_t tag, float ** float_p);
offset_t read_offsetdata( ctiff_t * ctif, uint32 address, uint16 count, uint16 datatype);
const char * renderer ( const ret_t ret );
void set_renderer_to_ansi();
void set_renderer_to_xml();
char * secstrcat (char * dest, const char * src, int maxsize);
ctiff_t * initialize_ctif( const char * tiff_file, ct_ioflag_t );
void free_ctif( ctiff_t * ctif);
uint32 get_ifd0_pos( ctiff_t * ctif );
uint32 get_next_ifd_pos( ctiff_t * ctif, uint32 actual_pos );
uint16 get_ifd0_count( ctiff_t * ctif );
char is_byteswapped( ctiff_t * ctif );
#endif
