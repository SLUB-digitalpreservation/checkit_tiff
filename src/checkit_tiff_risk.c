/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/16
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"

/* *********** main ************* */
int main(int argc, char * argv[]) {
	if (argc != 2) {
		perror ("checkit_tiff_risk FILE\n\twhere FILE is a baseline TIFF-file\n");
		exit (-1);
	}
	const char *tiff_file=argv[1];
	tif_files(tiff_file);
	ctiff_t * ctif = initialize_ctif( tiff_file );
	scan_mem_map(ctif);
	return 0;
}

