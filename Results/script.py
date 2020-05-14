#!/usr/bin/python

###########################
#Written By: Kestutis Saltonas
#
#Notes:
#
#	Example to Run:  ./script.py Inputs/ Outputs/
#
#
#
###########################

import sys
import os

###########################
#Global Variable:
###########################
Program_name = "../bitmap"			#Name and Location of the program.

###########################
#Functions:
###########################
def usage(script_name):
	print("Usage: " + script_name + "Bitmap_IN_Directory Bitmap_OUT_Directory")
	print
	print("This Script will run through all the following Bitmaps:")
	print("bear1_24  and bear1_32")
	print("bear2_24  and bear2_32")
	print("bear3_24  and bear3_32")
	print("pikachu24 and pikachu32")
	print("sonic24   and sonic32")
	print("--------------------------------------------------------")
	print("This Script will run through all the following filters:")
	print("cell shade			gray scale")
	print("pixelate			blur")
	print("rotate 90			rotate 180")
	print("rotate 270			flip vertically")
	print("flip horizontally		flip diagonally 1")
	print("flip diagonally 2		scale image by 2")
	print("scale image by 0.5		Unaltered")
	print
	print

###########################
#Main Function Call:
###########################
def main(argv):
	if len(argv) != 3:
		usage(argv[0])
		return

	Bitmap_List = [	"bear1_24",  "bear1_32", 
					"bear2_24",  "bear2_32",
					"bear3_24",  "bear3_32",
					"pikachu24", "pikachu32",
					"sonic24", 	  "sonic32"]

	Filter_List = [	"-c",		"-g",		"-p", 		"-b", 		"-r90",
					"-r180", 	"-r270", 	"-v", 		"-h", 		"-d1", 
					"-d2",		"-grow",	"-shrink",	"-x"]

	Filter_Name = [	"cell",		"gray",		"pixal",	"blur",		"R90",
					"R180",		"R270",		"flipV",	"flipH",	"flipD1",
					"flipD2",	"grow",		"shrink",	""]
	
	for i in range (0, len(Filter_List)):
		for j in range (0, len(Bitmap_List)):
			cmd = Program_name + " " + Filter_List[i] + " " + argv[1] + Bitmap_List[j] + ".bmp" + " " + argv[2] + Bitmap_List[j] + "_" + Filter_Name[i] + ".bmp"
			os.system(cmd)

	return

###########################
#Top Level Execution:
###########################
if __name__ == "__main__":
	main(sys.argv)
