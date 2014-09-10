#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

int main(int argc, char** argv)
{
	int error;
	FT_Library ft;

	error = FT_Init_FreeType(&ft);
	if (error) {
		std::cerr << "Freetype: Unable to init library." << std::endl;
		exit(1);
	}

	std::cout << "Loaded lib." << std::endl;

	FT_Done_FreeType(ft);
	exit(0);
}

