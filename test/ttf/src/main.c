/** @addtogroup untar
 * @{
 */
/** @file
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <str.h>
#include <ttf.h>

int main(int argc, char* argv[]) {
    printf("\n\t\t PRINT GLUPH OUTLINE CORDS!\n\n");
	const char *input = NULL;
	if (argv[1] != NULL) input=str_dup(argv[1]);
		if (input==NULL){
	printf("here is 2 'ttf' files.\n");
    printf("Please enter name of desired 'ttf' file.\n");
	printf("eg. 'ttf envy.ttf' or  ' ttf envy_bold.ttf'.\n");
    printf("Please try again !.\n");
	return 0;
	}
	printf("FONT -");
    printf(input);
	printf("\n\n");

    char * font= (char *)input;
    int file_size = 0;
	char* file = read_file(font, &file_size);
	char* mem_ptr = file;

	font_directory ft = {0};
	read_font_directory(file, &mem_ptr, &ft);

	glyph_outline A = get_glyph_outline(&ft, get_glyph_index(&ft, 'A'));
	print_glyph_outline(&A);
	printf("- All coordinates is written on 'FontTest.txt' file.\n");
	printf("Just tupe 'edit FontTest.txt' in terminal.\n");

	return 0;
}


/** @}
 */
