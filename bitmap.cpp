/* ****************************************************
Written By: Kestutis Saltonas

TODO:
	- Fix 24 bit image rescaling?

******************************************************* */

#include "bitmap.h"
#include <math.h>

/* ********************
Overloading Opperators: 
*********************** */

std::ostream & operator << (std::ostream &out, const Bitmap &b)
{
    out << b.tag[0] << b.tag[1];
    out.write((char*)&b.size, 4);
    out.write((char*)&b.ignored, 4);
    out.write((char*)&b.offset, 4);
    out.write((char*)&b.size_second, 4);
    out.write((char*)&b.width, 4);
    out.write((char*)&b.height, 4);
    out.write((char*)&b.num_color_planes, 2);
    out.write((char*)&b.color_depth, 2);
    out.write((char*)&b.compression_method, 4);
    out.write((char*)&b.data_size, 4);
    out.write((char*)&b.horizontal_rez, 4);
    out.write((char*)&b.vertical_rez, 4);
    out.write((char*)&b.color_palate, 4);
    out.write((char*)&b.num_important_color, 4);

if(b.compression_method == 3){					
    out.write((char*)&b.mask_R, 4);				//0xFF00 0000
    out.write((char*)&b.mask_G, 4);				//0x00FF 0000
    out.write((char*)&b.mask_B, 4);				//0x0000 FF00
    out.write((char*)&b.mask_A, 4);				//0x0000 0000
    out.write((char*)&b.color_space_info, 68);

    for(size_t i=0; i<b.height; ++i){
		for(size_t j=0; j<b.width; ++j){
			out.write((char*)&b.image_data[i][j], 4);
		}
    } 
}

if(b.compression_method == 0){
    for(size_t i=0; i<b.height; ++i){
		for(size_t j=0; j<b.width+1; ++j){
			if(j<b.width)
				out.write((char*)&b.image_data[i][j], 3);
			else
				out.write((char*)&b.image_data[i][j-1], b.padding);
		}
    } 
}

return out;
}
 
//overloading the >> operator:
std::istream & operator >> (std::istream &in,  Bitmap &b)
{
    in >> b.tag[0] >> b.tag[1];
    in.read((char*)&b.size, 4);
    //in.ignore(4);						//ignore garbage bytes.
    in.read((char*)&b.ignored, 4);
    in.read((char*)&b.offset, 4);
    in.read((char*)&b.size_second, 4);
    in.read((char*)&b.width, 4);
    in.read((char*)&b.height, 4);
    in.read((char*)&b.num_color_planes, 2);
    in.read((char*)&b.color_depth, 2);
    in.read((char*)&b.compression_method, 4);
    in.read((char*)&b.data_size, 4);
    in.read((char*)&b.horizontal_rez, 4);
    in.read((char*)&b.vertical_rez, 4);
    in.read((char*)&b.color_palate, 4);
    in.read((char*)&b.num_important_color, 4);

//Check compression and manualy set mask if not 32-bit color. 
//24-bit color mask assumed evenly distributed amoung RGB.
if(b.compression_method == 3){
    in.read((char*)&b.mask_R, 4);				//0xFF00 0000
    in.read((char*)&b.mask_G, 4);				//0x00FF 0000
    in.read((char*)&b.mask_B, 4);				//0x0000 FF00
    in.read((char*)&b.mask_A, 4);				//0x0000 0000
    in.read((char*)&b.color_space_info, 68);
}
else{								
    b.mask_R = 0xFF0000;			
    b.mask_G = 0x00FF00;				
    b.mask_B = 0x0000FF;				
}

//    bitmap_print_header(b);					//Print Header Info.

if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
else
	b.padding=0;

    image_data_init(b);						//create matrix for data

if(b.compression_method == 3){
    for(size_t i=0; i<b.height; ++i){
    	for(size_t j=0; j<b.width; ++j){
    		in.read((char*)&b.image_data[i][j], 4);	
      	}
		in.ignore(b.padding);
    }  
}

if(b.compression_method == 0){
    for(size_t i=0; i<b.height; ++i){
    	for(size_t j=0; j<b.width; ++j){
    			in.read((char*)&b.image_data[i][j], 3);
      	}
		in.ignore(b.padding);
    }  
}
    return in; 
}

/* ******************************************
Constructor and deconstructor for Image array: 
********************************************* */

//Create the 2d Matrix for Image Bitmap (used for changing matrix size):
void image_data_init(Bitmap& b)
{
	b.image_data = new uint32_t*[b.height];
	for(size_t i=0; i<b.height; ++i){
		b.image_data[i] = new uint32_t[b.width];	
	}
}

//Free the 2d Matrix used by Image Bitmap (used for changing matrix size):
void image_data_deconstruct(Bitmap& b)
{
	for(size_t i=0; i<b.height; ++i){
		delete b.image_data[i];
	}
	delete b.image_data;
}

//Constructor:
Bitmap::Bitmap()
{
    
}

//Deconstructor:
Bitmap::~Bitmap()
{
if(this->image_data){
    for(size_t i=0; i<this->height; ++i){
		delete this->image_data[i];
	}
	delete this->image_data;
}
}

/* *********************************************************
Image Filtering Functions (filter described in header file):
************************************************************ */

void flipd1(Bitmap & b)
{

	uint32_t ** temp_image;
	int temp_H;
	int temp_W;

//Allocate temp Image Bitmap:
	temp_image = new uint32_t*[b.width];
	for(size_t i=0; i<b.width; ++i)
		temp_image[i] = new uint32_t[b.height];

//Copy Data:
    for(size_t i=0; i<b.width; ++i)
    	for(size_t j=0; j<b.height; ++j)
			temp_image[b.width-1-i][b.height-1-j] = b.image_data[j][i];

//Replace old Image:
	temp_H = b.height;
	temp_W = b.width;
	image_data_deconstruct(b);
	b.height = temp_W;
	b.width = temp_H;
	image_data_init(b);

    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			b.image_data[i][j] = (uint32_t)temp_image[i][j]; 

	for(size_t i=0; i<temp_W; ++i)
		delete temp_image[i];
	delete temp_image;

//Fix Padding:
if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
 

//Easy Way, but slower:
/*	flipd2(b);
	fliph(b);
	flipv(b); */
}

void flipd2(Bitmap & b)
{
	uint32_t ** temp_image;
	int temp_H;
	int temp_W;

//Allocate temp Image Bitmap:
	temp_image = new uint32_t*[b.width];
	for(size_t i=0; i<b.width; ++i)
		temp_image[i] = new uint32_t[b.height];

//Copy Data:
    for(size_t i=0; i<b.width; ++i)
    	for(size_t j=0; j<b.height; ++j)
			temp_image[i][j] = b.image_data[j][i];

//Replace old Image:
	temp_H = b.height;
	temp_W = b.width;
	image_data_deconstruct(b);
	b.height = temp_W;
	b.width = temp_H;
	image_data_init(b);

    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			b.image_data[i][j] = (uint32_t)temp_image[i][j]; 

	for(size_t i=0; i<temp_W; ++i)
		delete temp_image[i];
	delete temp_image; 

//Fix Padding:
if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
}

void scaleUp(Bitmap& b)
{
	uint32_t ** temp_image;
	int temp_H;
	int temp_W;

//Allocate temp Image Bitmap:
	temp_image = new uint32_t*[b.height*2];
	for(size_t i=0; i<b.height*2; ++i)
		temp_image[i] = new uint32_t[b.width*2];		

//Copy every other Pix:
    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
		{
			temp_image[2*i][2*j] = b.image_data[i][j];
			temp_image[2*i+1][2*j+1] = b.image_data[i][j];
			temp_image[2*i+1][2*j] = b.image_data[i][j];
			temp_image[2*i][2*j+1] = b.image_data[i][j]; 
		}
		
//Replace old Image:
	temp_H = b.height*2;
	temp_W = b.width*2;
	image_data_deconstruct(b);
	b.height = temp_H;
	b.width = temp_W;
	image_data_init(b);

    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			b.image_data[i][j] = (uint32_t)temp_image[i][j]; 

	for(size_t i=0; i<temp_H; ++i)
		delete temp_image[i];
	delete temp_image; 

//Fix Padding:
if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
}

void scaleDown(Bitmap& b)
{
	uint32_t ** temp_image;
	int temp_H;
	int temp_W;

//Allocate temp Image Bitmap:
	temp_image = new uint32_t*[b.height/2];
	for(size_t i=0; i<b.height/2; ++i)
		temp_image[i] = new uint32_t[b.width/2];		

//Copy every other Pix:
    for(size_t i=0; i<b.height/2; ++i)
    	for(size_t j=0; j<b.width/2; ++j)
				temp_image[i][j] = b.image_data[2*i][2*j]; 
		
//Replace old Image:
	temp_H = b.height/2;
	temp_W = b.width/2;
	image_data_deconstruct(b);
	b.height = temp_H;
	b.width = temp_W;
	image_data_init(b);

    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			b.image_data[i][j] = (uint32_t)temp_image[i][j]; 

	for(size_t i=0; i<temp_H; ++i)
		delete temp_image[i];
	delete temp_image; 

//Fix Padding:
if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
}

void fliph(Bitmap& b)
{
	uint32_t temp;
	for(size_t i=0; i<b.height; ++i){
		for(size_t j=0; j<(b.width/2); ++j){		
			temp = b.image_data[i][j];
			b.image_data[i][j] = b.image_data[i][b.width-j-1];
			b.image_data[i][b.width-j-1] = temp;
		}
	}
}

void flipv(Bitmap& b)
{
	uint32_t temp;
	for(size_t i=0; i<(b.height/2); ++i){
		for(size_t j=0; j<b.width; ++j){		
			temp = b.image_data[i][j];
			b.image_data[i][j] = b.image_data[b.height-i-1][j];
			b.image_data[b.height-i-1][j] = temp;
		}
	}
}

void rot270(Bitmap& b){

	uint32_t ** temp_image;
	int temp_H;
	int temp_W;

//Allocate temp Image Bitmap:
	temp_image = new uint32_t*[b.width];
	for(size_t i=0; i<b.width; ++i)
		temp_image[i] = new uint32_t[b.height];		

//Rotate Image:
    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			temp_image[b.width-1-j][i] = b.image_data[i][j]; 

//Replace old Image:
	temp_H = b.height;
	temp_W = b.width;
	image_data_deconstruct(b);
	b.height = temp_W;
	b.width = temp_H;
	image_data_init(b);

    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			b.image_data[i][j] = (uint32_t)temp_image[i][j]; 

	for(size_t i=0; i<temp_W; ++i)
		delete temp_image[i];
	delete temp_image;
 
//Fix Padding:
if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
}

void rot180(Bitmap& b){
	rot90(b);
	rot90(b);
}

void rot90(Bitmap& b)
{

	uint32_t ** temp_image;
	int temp_H;
	int temp_W;

//Allocate temp Image Bitmap:
	temp_image = new uint32_t*[b.width];
	for(size_t i=0; i<b.width; ++i)
		temp_image[i] = new uint32_t[b.height];		

//Rotate Image:
    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			temp_image[j][b.height-1-i] = b.image_data[i][j]; 

//Replace old Image:
	temp_H = b.height;
	temp_W = b.width;
	image_data_deconstruct(b);
	b.height = temp_W;
	b.width = temp_H;
	image_data_init(b);

    for(size_t i=0; i<b.height; ++i)
    	for(size_t j=0; j<b.width; ++j)
			b.image_data[i][j] = (uint32_t)temp_image[i][j]; 

	for(size_t i=0; i<temp_W; ++i)
		delete temp_image[i];
	delete temp_image; 

//Fix Padding:
if(b.compression_method == 0){
	for(int i=0; i<4; ++i)						//calculate padding.
        if( ((b.width*3) + i) % 4 == 0)
            b.padding = i;
}
}

void blur(Bitmap& b)
{
	unsigned int Gaussian_matrix[5][5] = {	{1,  4,  6,  4, 1}, 
											{4, 16, 24, 16, 4},	
											{6, 24, 36, 24, 6},
											{4, 16, 24, 16, 4},
											{1,  4,  6,  4, 1}};
	double sum_R;
	double sum_G;
	double sum_B;
	uint32_t R_val;
	uint32_t G_val;
	uint32_t B_val;

	uint32_t temp;
	uint32_t pix_val;

for(size_t z=0; z<FILTER_COUNT; z++){
    for(size_t i=0; i<b.height; ++i){
    	for(size_t j=0; j<b.width; ++j){
			sum_R = 0;
			sum_G = 0;
			sum_B = 0;

    	    for(size_t y=i-2; y<=i+2; ++y){
     	        for(size_t x=j-2; x<=j+2; ++x){
				
					if (y >= 0 && x >= 0 && y+2 <= b.height && x+2 <= b.width){
					pix_val = b.image_data[y][x]; 
//Red pixel:
					temp = pix_val & b.mask_R;
					temp = temp>>((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE);
					sum_R += ((double)Gaussian_matrix[y-(i-2)][x-(j-2)] / 256) * temp;
//Green pixel:
					temp = pix_val & b.mask_G;
					temp = temp>>((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE);
					sum_G += ((double)Gaussian_matrix[y-(i-2)][x-(j-2)] / 256) * temp;
//Blue pixel:
					temp = pix_val & b.mask_B;
					temp = temp>>((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
					sum_B += ((double)Gaussian_matrix[y-(i-2)][x-(j-2)] / 256) * temp; 
					}
				}
			}
			R_val = (uint32_t)sum_R;
			G_val = (uint32_t)sum_G;
			B_val = (uint32_t)sum_B;

//Write the data:
			b.image_data[i][j] = R_val<<((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE) |
								 G_val<<((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE) |
								 B_val<<((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
		}
	}
}
	
}

void pixelate(Bitmap& b)
{
	uint32_t sum_R;
	uint32_t sum_G;
	uint32_t sum_B;

	uint32_t temp;
	uint32_t pix_val;

if(b.height >= b.width){
    for(size_t i=0; i<b.height; i+=PIXELATE_SIZE){
    	for(size_t j=0; j<b.width; j+=PIXELATE_SIZE){		

			sum_R = 0;
			sum_G = 0;
			sum_B = 0;

    	    for(size_t y=i; y<i+PIXELATE_SIZE && y<b.height; ++y){
     	        for(size_t x=j; x<j+PIXELATE_SIZE && x<b.width; ++x){
				pix_val = b.image_data[y][x]; 
//Red pixel:
	    		temp = pix_val & b.mask_R;
				temp = temp>>((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE);
				sum_R += temp;
//Green pixel:
				temp = pix_val & b.mask_G;
				temp = temp>>((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE);
				sum_G += temp;
//Blue pixel:
				temp = pix_val & b.mask_B;
				temp = temp>>((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
				sum_B += temp; 
				}
			} 

//Write the vals:
			sum_R = sum_R / (PIXELATE_SIZE * PIXELATE_SIZE);
			sum_G = sum_G / (PIXELATE_SIZE * PIXELATE_SIZE);
			sum_B = sum_B / (PIXELATE_SIZE * PIXELATE_SIZE);

  	 	    for(size_t y=i;y<i+PIXELATE_SIZE && y<b.height; ++y){
     	        for(size_t x=j; x<j+PIXELATE_SIZE && x<b.width; ++x){
					b.image_data[y][x] = sum_R<<((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE) |
										 sum_G<<((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE) |
									 	 sum_B<<((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
				}
			} 
		}
    }
}
else{
   for(size_t i=0; i<b.width; i+=PIXELATE_SIZE){
    	for(size_t j=0; j<b.height; j+=PIXELATE_SIZE){

			sum_R = 0;
			sum_G = 0;
			sum_B = 0;

    	    for(size_t y=i; y<i+PIXELATE_SIZE && y<b.width; ++y){
     	        for(size_t x=j; x<j+PIXELATE_SIZE && x<b.height; ++x){
				pix_val = b.image_data[x][y]; 
//Red pixel:
	    		temp = pix_val & b.mask_R;
				temp = temp>>((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE);
				sum_R += temp;
//Green pixel:
				temp = pix_val & b.mask_G;
				temp = temp>>((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE);
				sum_G += temp;
//Blue pixel:
				temp = pix_val & b.mask_B;
				temp = temp>>((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
				sum_B += temp; 
				}
			} 

//Write the vals:
			sum_R = sum_R / (PIXELATE_SIZE * PIXELATE_SIZE);
			sum_G = sum_G / (PIXELATE_SIZE * PIXELATE_SIZE);
			sum_B = sum_B / (PIXELATE_SIZE * PIXELATE_SIZE);
  	 	    for(size_t y=i;y<i+PIXELATE_SIZE && y<b.width; ++y){
     	        for(size_t x=j; x<j+PIXELATE_SIZE && x<b.height; ++x){
					b.image_data[x][y] = sum_R<<((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE) |
										 sum_G<<((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE) |
									 	 sum_B<<((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
				}
			} 
		}
    }
}
}

void grayscale(Bitmap& b)
{
    uint32_t temp;
	uint32_t new_val;
	uint32_t pix_val;

    for(size_t i=0; i<b.height; ++i){
    	for(size_t j=0; j<b.width; ++j){
		new_val = 0;
		pix_val = b.image_data[i][j];
//Red pixel:
    		temp = pix_val & b.mask_R;
			temp = temp>>((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE);
			new_val += temp;
//Green pixel:
    		temp = pix_val & b.mask_G;
			temp = temp>>((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE);
			new_val += temp;
//Blue pixel:
    		temp = pix_val & b.mask_B;
			temp = temp>>((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
			new_val += temp;
//Write the val:
			temp = new_val / 3;
			b.image_data[i][j] =	temp<<((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE) |
									temp<<((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE) |
									temp<<((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
	}
    }
}

void cellShade(Bitmap& b)
{
    uint32_t temp;
	uint32_t new_val;
	uint32_t pix_val;

    for(size_t i=0; i<b.height; ++i){
    	for(size_t j=0; j<b.width; ++j){
			new_val = 0;
			pix_val = b.image_data[i][j];
//Red pixel:
			temp = pix_val & b.mask_R;
			temp = temp>>((unsigned int)(log2(b.mask_R)+1) - BYTE_SIZE);
			if(temp < 90)
				new_val = new_val | 0x0;
			if(temp >= 90 && temp < 127)
				new_val = new_val | (0xB4<<(unsigned int)((log2(b.mask_R)+1) - BYTE_SIZE));
			if(temp >= 127)
				new_val = new_val | (0xFF<<(unsigned int)((log2(b.mask_R)+1) - BYTE_SIZE));
//Green pixel:
	   		temp = pix_val & b.mask_G;
			temp = temp>>((unsigned int)(log2(b.mask_G)+1) - BYTE_SIZE);
			if(temp < 90)
				new_val = new_val | 0x0;
			if(temp >= 90 && temp < 127)
				new_val = new_val | (0xB4<<(unsigned int)((log2(b.mask_G)+1) - BYTE_SIZE));
			if(temp >= 127)
				new_val = new_val | (0xFF<<(unsigned int)((log2(b.mask_G)+1) - BYTE_SIZE));
//Blue pixel:
	   		temp = pix_val & b.mask_B;
			temp = temp>>((unsigned int)(log2(b.mask_B)+1) - BYTE_SIZE);
			if(temp < 90)
				new_val = new_val | 0x0;
			if(temp >= 90 && temp < 127)
				new_val = new_val | (0xB4<<(unsigned int)((log2(b.mask_B)+1) - BYTE_SIZE));
			if(temp >= 127)
				new_val = new_val | (0xFF<<(unsigned int)((log2(b.mask_B)+1) - BYTE_SIZE));
//Write the val:
			b.image_data[i][j] = new_val;
      	}
    }
}

/* *********************************************************
Helper and Debug Functions:
************************************************************ */
void bitmap_print_header(Bitmap& b)
{
    std::cout << "tag: " <<b.tag << std::endl;							//Check Tag.
    std::cout << "Size: " <<b.size << std::endl; 						//Check Image Size.
    std::cout << "Offset: " <<b.offset << std::endl; 					//Check data offset.
    std::cout << "Second Header: " <<b.size_second << std::endl;		//Check Second Header Size, eq 40.
    std::cout << "color planes: " <<b.num_color_planes << std::endl;	//Check color planes, eq 1
    std::cout << "Color Depth: " <<b.color_depth << std::endl; 			//Check color_depth, eq 24 or 32.
    std::cout << "Compression: " <<b.compression_method << std::endl; 	//Check compression. eq 0 or 3.
    std::cout << "Horizontral rez: " <<b.horizontal_rez << std::endl; 	//Check rez. eq 2835.
    std::cout << "Vertical rez: " <<b.vertical_rez << std::endl; 		//Check rez. eq 2835.
    std::cout << "color palate: " <<b.color_palate << std::endl; 		//Check colors. eq 0.
    std::cout << "important: " <<b.num_important_color << std::endl; 	//Check imp colors. eq 0.
if(b.compression_method == 3){
    std::cout << "Mask_R is: " <<b.mask_R << std::endl; 				//Check Red Mask.
    std::cout << "Mask_G is: " <<b.mask_G << std::endl; 				//Check Green Mask.
    std::cout << "Mask_B is: " <<b.mask_B << std::endl; 				//Check Blue Mask.
    std::cout << "Mask_A is: " <<b.mask_A << std::endl; 				//Check Alpha Mask.
    std::cout << "Width is: " <<b.width << std::endl; 					//Check width.
    std::cout << "Height is: " <<b.height << std::endl; 				//Check height. 
}
}

/* *********************************************************************
Extra Image Filtering Functions (Created through misunderstanding. Oops):
************************************************************************ */
/*
This function flips an image aross its center diaginal.

Sample Input:		Sample Output:
| 1  2  3  4| 		|16 12  8  4|
| 5  6  7  8|		|15 11  7  3|
| 9 10 11 12|		|14 10  6  2|
|13 14 15 16|		|13  9  5  1|
*/
void fake_flipd2(Bitmap & b)
{
	int y1;
	int y2;
	int x1;
	int x2;
	int j=1;
	uint32_t temp;

    for(int i=b.height-1; i>=0; i--){
    	x2=j;
        y2=b.height-1; 
	    for(int y1=i, x1=0; y1>=0 && x1<b.width; --y1, ++x1){	
			temp = b.image_data[y1][x1];
            b.image_data[y1][x1]=b.image_data[y2][x2];
            b.image_data[y2][x2]=temp;
            y2--;
            x2++;
	    }
	    j++;
	}
}
