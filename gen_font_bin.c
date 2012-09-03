/* this takes the image strip saved by GIMP, convert it to memory image that is easy to use in the arm-clock code.
 * Converted image has the following format:
 *
 *   Each character is represented by black/white 40x56 pixels. Each line is store in 5 bytes (5 x 8bits = 40bits). And there are 56 lines. Then there's totl 5*56=280 bytes for each characters. All characters are stored in continus memory.
 */

#include <stdio.h>
#include <assert.h>
#include "font.c"

#define TILE_WIDTH	40
#define TILE_HEIGHT	56

int main()
{
	assert(gimp_image.width%TILE_WIDTH == 0);
	assert(gimp_image.height == TILE_HEIGHT);

	int tile_num = gimp_image.width / TILE_WIDTH;

	fprintf(stderr, "Generating %d tiles ...\n", tile_num);

	int t,r,c;
    unsigned char out = 0;
	for(t=0;t<tile_num;t++)
	{
        for(r=0;r<TILE_HEIGHT;r++)
        {
            for(c=0;c<TILE_WIDTH;c++)
            {
                //unsigned char pixel = gimp_image.pixel_data[ r * gimp_image.width + t * TILE_WIDTH + c ];
                unsigned char pixel = gimp_image.pixel_data[ ( r*gimp_image.width + c + TILE_WIDTH*t)*3 ];
                
                if(c%8== 0)
                    out = 0;

                if(pixel<127)
                    out |= 1;
                out = out << 1;

                if(c%8==7)
                    write(1, &out, 1);
            }
        }
	}

	
	return 0;
}
