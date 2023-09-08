/*
	Author: bitluni 2019
	License: 
	Creative Commons Attribution ShareAlike 4.0
	https://creativecommons.org/licenses/by-sa/4.0/
	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net
*/
#pragma once

class FontVar
{
  public:
	const int firstChar;
	const int charCount;
	const unsigned char *pixels;
	const int *data;
	
	const int charWidth;
	const int charHeight;
	FontVar(int charWidth, int charHeight, const unsigned char *pixels, const int *data, int firstChar = 32, int charCount = 96)
		:firstChar(firstChar),
		charCount(charCount),
		pixels(pixels),
		charWidth(charWidth),
		charHeight(charHeight),
		data(data)
	{
	}

	bool valid(char ch) const
	{
		return ch >= firstChar && ch < firstChar + charCount;
	}
};
