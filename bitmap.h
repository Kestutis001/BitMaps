#include <iostream>
#include <string>
#include <stdint.h>

#define PIXELATE_SIZE 16				//block size for pixelate function.
#define BYTE_SIZE 8						//Size of a byte.
#define FILTER_COUNT 5					//Number of times to blur.

class Bitmap
{
private:
    /**
     * Read in an image.
     * reads a bitmap in from the stream
     *
     * @param in the stream to read from.
     * @param b the bitmap that we are creating.
     *
     * @return the stream after we've read in the image.
     *
     * @throws BitmapException if it's an invalid bitmap.
     * @throws bad_alloc exception if we failed to allocate memory.
     */
    friend std::istream& operator>>(std::istream& in, Bitmap& b);

    /**
     * Write the binary representation of the image to the stream.
     *
     * @param out the stream to write to.
     * @param b the bitmap that we are writing.
     *
     * @return the stream after we've finished writting.
     *
     * @throws failure if we failed to write.
     */
    friend std::ostream& operator<<(std::ostream& out, const Bitmap& b);
	
public:
    Bitmap();
	~Bitmap();

//Variables to hold Bitmap Info:
	char tag[3] = "";			//Tag should be BM for bit map.
	uint32_t size =0;
	uint32_t ignored =0;
	uint32_t offset =0;
	uint32_t size_second =0;
	int32_t width =0;				
	int32_t height =0;
	uint32_t num_color_planes =0;
	uint32_t color_depth =0;
	uint32_t compression_method =0;	//if 0 then bitdepth is 24. if 1 then bitdepth is 32.
	uint32_t data_size =0;
	uint32_t horizontal_rez =0;
	uint32_t vertical_rez =0;
	uint32_t color_palate =0;
	uint32_t num_important_color =0;
	uint32_t mask_A =0;
	uint32_t mask_R =0;
	uint32_t mask_G =0;
	uint32_t mask_B =0;
	char color_space_info[69] = "";
	uint32_t ** image_data;
	int padding = 0;
};

void image_data_init(Bitmap& b);

void image_data_deconstruct(Bitmap& b);

void bitmap_print_header(Bitmap& b);

/**
 * cell shade an image.
 * for each component of each pixel we round to 
 * the nearest number of 0, 180, 255
 *
 * This has the effect of making the image look like.
 * it was colored.
 */
void cellShade(Bitmap& b);

/**
 * Grayscales an image by averaging all of the components.
 */
void grayscale(Bitmap& b);

/**
 * Pixelats an image by creating groups of 16*16 pixel blocks.
 */
void pixelate(Bitmap& b);

/**
 * Use gaussian bluring to blur an image.
 */
void blur(Bitmap& b);

/**
 * rotates image 90 degrees, swapping the height and width.
 */
void rot90(Bitmap& b);

/**
 * rotates an image by 180 degrees.
 */
void rot180(Bitmap& b);

/**
 * rotates image 270 degrees, swapping the height and width.
 */
void rot270(Bitmap& b);

/**
 * flips and image over the vertical axis.
 */
void flipv(Bitmap& b);

/**
 * flips and image over the horizontal axis.
 */
void fliph(Bitmap& b);

/**
 * flips and image over the line y = -x, swapping the height and width.
 */
void flipd1(Bitmap& b);

/**
 * flips and image over the line y = xr, swapping the height and width.
 */
void flipd2(Bitmap& b);

/**
 * scales the image by a factor of 2.
 */
void scaleUp(Bitmap& b);

/**
 * scales the image by a factor of 1/2.
 */
void scaleDown(Bitmap& b);



/**
 * BitmapException denotes an exception from reading in a bitmap.
 */
class BitmapException : public std::exception
{
    // the message to print out
    std::string _message;

    // position in the bitmap file (in bytes) where the error occured.
    uint32_t _position;

public:
    BitmapException() = delete;

    BitmapException(const std::string& message, uint32_t position);
    BitmapException(std::string&& message, uint32_t position);

    /**
     * prints out the exception in the form:
     *
     * "Error in bitmap at position 0xposition :
     * message"
     */
    void print_exception()
    {
		std::cout << "Error in bitmap at position 0x" << _position;
    };

};
