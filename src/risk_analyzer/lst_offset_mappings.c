/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/2016
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>



mem_map_t * add_mem_entry(mem_map_t * memmap_it_p, uint32 offset, uint32 count, memtype_t type) {
  memmap_it_p->offset=offset;
  memmap_it_p->count=count;
  memmap_it_p->mem_type=type;
  memmap_it_p++;
  return memmap_it_p;
}


mem_map_t * scan_mem_map(ctiff_t * ctif) {
  mem_map_t * memmap_p = NULL;
  memmap_p = malloc (sizeof(mem_map_t) * 128);
  if (NULL == memmap_p) {
	  perror ("could not allocate mem for memmap, abort");
  }
  mem_map_t * memmap_it_p = memmap_p;
  int entries = 0;
  /* header */
  memmap_it_p = add_mem_entry( memmap_it_p, 0, 4, constant);
  /* IFD0 Offset */
  memmap_it_p = add_mem_entry( memmap_it_p, 4, 4, ifd_offset);
  /* IFDO */
  uint32 ifd = get_ifd0_pos( ctif );
  //uint32 ifd0 = get_first_IFD(ctif);
  uint32 count = get_ifd0_count( ctif);

  memmap_it_p = add_mem_entry( memmap_it_p, 8, 2, ifd); /* count of tags in ifd */
  int ifdbase=8+2;
  /* iterate through IFD0 entries */
  int tagidx;
  

  for (tagidx = 0; tagidx< count; tagidx++) {
	  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12), 8, ifd); /* tagid, field type, count */
	  ifd_entry_t ifd_entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
	  uint32 tag = TIFFGetRawTagListEntry( ctif, tagidx);
	  TIFFDataType datatype = TIFFGetRawTagType( ctif, tag);
	  int datasize;
	  switch (datatype) {
		  case TIFF_ASCII: datasize = 1; break;
		  case TIFF_LONG: datasize =  4; break;
		  case TIFF_SHORT: datasize =  2; break;
		  case TIFF_BYTE: datasize =  1; break;
		  case TIFF_UNDEFINED: datasize =  1; break;
		  case TIFF_RATIONAL: datasize =  8; break;
		  case TIFF_SSHORT: datasize =  2; break;
		  case TIFF_SBYTE: datasize =  1; break;
		  case TIFF_SLONG: datasize =  4; break;
		  case TIFF_SRATIONAL: datasize =  8; break;
		  case TIFF_FLOAT: datasize =  4; break;
		  case TIFF_DOUBLE: datasize =  8; break;
	  }

	  if (ifd_entry.value_or_offset==is_offset) { /* offset */
		  uint32 offset = ifd_entry.data32offset;
		  uint16 count = ifd_entry.count;

		  memmap_it_p->count=count;

		  if (tag < 32768) { /* standard tag */
			  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12)+8, 4,ifd_offset_to_standardized_value ); 
			  memmap_it_p = add_mem_entry( memmap_it_p, offset, count*datasize, standardized_value );
		  } else if (tag < 65000) { /* registered tag */
			  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12)+8, 4,ifd_offset_to_registered_value ); 
			  memmap_it_p = add_mem_entry( memmap_it_p, offset, count*datasize, registered_value );
		  } else { /* private tag */
			  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12)+8, 4,ifd_offset_to_private_value ); 
			  memmap_it_p = add_mem_entry( memmap_it_p, offset, count*datasize, private_value );
		  }
	  } else if (ifd_entry.value_or_offset==is_value) { /* embedded value */
		  if (tag < 32768) { /* standard tag */
			  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12)+8, 4,ifd_embedded_standardized_value ); 
		  } else if (tag < 65000) { /* registered tag */
			  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12)+8, 4,ifd_embedded_registered_value ); 
		  } else { /* private tag */
			  memmap_it_p = add_mem_entry( memmap_it_p, ifdbase+(tagidx*12)+8, 4,ifd_embedded_private_value ); 
		  }

	  }
  }
  /* check next IFD mark */
  uint32 offset = get_ifd0_pos(ctif );
  uint32 IFDn = get_next_ifd_pos( ctif, offset );
  memmap_it_p->offset=offset;
  memmap_it_p->count=4;
  memmap_it_p->mem_type=ifd_offset;
  memmap_it_p++;
  /* TODO */
  for (mem_map_t * i=memmap_p; i < memmap_it_p; i++) {
	  printf ("offset=%i count=%i, (end=%i) type=%i\n", i->offset, i->count, (i->offset + i->count), i->mem_type);
  }
}
