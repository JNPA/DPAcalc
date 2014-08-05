#ifndef _FileReader_included_
#define _FileReader_included_

#include "common.h"
#include "FileConfigReader.h"

/**
 * @file "FileReader.h"
 */

/**
 * @brief This class parses Lecroy or Agilent files.
 *
 * This class parses a list of files (Lecroy or Agilent), save the most important informations and
 * then saves the traces of each file in a single vector. This vector is used to create the output files.
 *
 */
class FileReader
{
	public:
		/**
		 * @brief Creates a new object of the class.
		 * @param settings the file with the settings used to parse Lecroy/Agilent files.
		 */
		FileReader ( FileConfigReader& settings );
		/**
		 * @brief Class destructor.
		 */
		~FileReader();
		/**
		 * @brief Returns the number of samples saved for each trace.
		 * @return the number of samples saved for each trace
		 */
		UINTN getNSample();
		/**
		 * @brief Returns the data format type.
		 * @return the data format type
		 * several format types are available:\n
		 * @li for Lecroy files: 'b' for int8 and 'c' for int16
		 * @li for Agilent files: 'f' for float and 'd' for double
		 */
		char getFormatType();
		/**
		 * @brief Returns the size in bytes of the data format type.
		 * @return the size in byte of the data format type
		 */
		UINTN getFormatSize();
		/**
		 * @brief Returns the list of all the traces contained in the parsed files.
		 * @return the list of all the traces contained in the parsed files
		 */
		const vector<uint8_t*>& getTraces();
	private:
		string FileName;
		ifstream DataFile;
		vector <uint8_t*> Traces;
		UINTN NSample;
		char FormatType;
		UINTN FormatSize;
		bool isLecroy();
		bool isAgilent();
		void readLecroy ( FileConfigReader& settings );
		void readAgilent ( FileConfigReader& settings );
		void readTxt ( FileConfigReader& settings );
};

#endif
