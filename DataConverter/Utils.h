#ifndef _Utils_included_
#define _Utils_included_

#include "common.h"

/**
 * @file "Utils.h"
 */

/**
 * @brief This class provides some helpful static functions.
 */
class Utils
{
	public:
		/**
		 * @brief Returns true if the string is in hexadecimal format, false otherwise.
		 * @param str string to check
		 * @return true if the string is in hexadecimal format, false otherwise
		 */
		bool static isHexText ( string str );
		/**
		 * @brief Transforms a string in hexadecimal format into a binary format.
		 * @param str string to convert
		 * @param bin result of the conversion
		 */
		void static HexToBin ( string str, uint8_t* bin );
		/**
		 * @brief Cleans a string
		 * @param str string to clean
		 * @return a string without space, '\n', '\t', '\r'
		 */
		string static CleanString ( string str );
		/**
		 * @brief Creates a name with a pad of digit and a number at the end.
		 * @param name name to modify
		 * @param num number to add at the end
		 * @param pad the number of digit to add (filled with zeros)
		 * @return the name modified (name + zeros padding + number)
		 * this function is useful to create names in sequence.\n
		 * for example: out0001, out0002, etc...
		 */
		string static createOutputName ( string name, unsigned int num, unsigned int pad );
		/**
		 * @brief Check if a string terminates with '/', otherwise it adds this character
		 * @param path string to check
		 * @return the same string terminated with '/'
		 */
		string static adjustPath ( string path );
};

#endif
