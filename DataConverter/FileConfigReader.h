#ifndef _FileConfigReader_included_
#define _FileConfigReader_included_

#include "common.h"
#include "Utils.h"

/**
 * @file "FileConfigReader.h"
 */

/**
 * @brief This class parses a converter configuration file.
 *
 * The object created contains all the configuration settings used by the FileReader Class to parse Lecroy/Agilent files \n
 * and used by the Converter to create the output files.\n
 * The configuration file must contain lines in this format:\n
 * @li the couple: keyValue = value
 * @li the special key @e 'file_list:' followed by a full path list of the Lecroy/Agilent files\n
 * @li a comment line starts with '#'
 */
class FileConfigReader
{
	public:
		/**
		 * @brief Creates a new object of the class.
		 * @param file_name complete file name of the configuration file to parse
		 */
		FileConfigReader ( const string& file_name );
		/**
		 * @brief Class destructor.
		 */
		~FileConfigReader();
		/**
		 * @brief Returns the length of the trace to save.
		 * @return the length of the trace to save in number of samples
		 */
		UINTN getOutputTraceLength();
		/**
		 * @brief Returns the number of samples to ignore during the conversion.
		 * @return the number of samples to ignore during the conversion
		 */
		UINTN getOutputTraceOffset();
		/**
		 * @brief Returns the number of traces to save in any output file.
		 * @return the number of traces to save in any output file
		 */
		UINTN getOutputTracePerFile();
		/**
		 * @brief Returns the location where the output files will be saved.
		 * @return the location where the output files will be saved
		 */
		string getOutputPath();
		/**
		 * @brief Returns the name of the output file to save.
		 * @return the name of the output file to save
		 */
		string getOutputName();
		/**
		 * @brief Returns the format of the input files (Lecroy, Agilent, txt).
		 * @return the format of the input files (Lecroy, Agilent, txt)
		 */
		int getInputFormat();
		/**
		 * @brief Returns the list of the input files to convert with full path.
		 * @return the list of the input files to convert with full path
		 * Fix by TFT: returning a reference to avoid inefficiency
		 */
		const vector <string>& getFileList();
		/**
		 * @brief Checks if a key exists in the list of parsed keys
		 * @param key name of the key
		 * @return true if the key was parsed, false otherwise
		 */
		bool keyExists ( const string& key ) const;
		/**
		 * @brief Returns the value of a parsed key converted in ValueType
		 * @param key name of the key
		 * @param ValueType data type of the value of the key
		 * @return the value of a parsed key converted in ValueType
		 * this function uses a template to get the value of the key converted in the desidered type. \n
		 * used type: unsigned int, string.
		 */
		template <typename ValueType>
		ValueType getValueOfKey ( const string& key, ValueType const& defaultValue = ValueType() ) const;
	private:
		string fileName;
		ifstream file;
		map<string, string> contents;
		UINTN OutputTraceLength;
		UINTN OutputTraceOffset;
		UINTN OutputTracePerFile;
		string OutputPath;
		string OutputName;
		vector <string> FileList;
		int InputFormat;
		bool isfileList;
		template <typename T>
		inline static string T_to_string ( T const& val ) {
			ostringstream ostr;
			ostr << val;
			return ostr.str();
		}
		template <typename T>
		inline static T string_to_T ( string const& val ) {
			istringstream istr ( val );
			T returnVal;
			if ( ! ( istr >> returnVal ) ) {
				cerr << "Not a valid type received" << endl;
				exit ( 2 );
			}
			return returnVal;
		}
		void removeComment ( string& line ) const;
		bool onlyWhitespace ( const string& line ) const;
		bool validLine ( const string& line ) const;
		void extractKey ( string& key, size_t const& sepPos, const string& line ) const;
		void extractValue ( string& value, size_t const& sepPos, const string& line ) const;
		void extractContents ( const string& line );
		void extractFileList ( const string& line ) ;
		void parseLine ( const string& line, size_t const lineNo );
		void ExtractKeys();
		int StringToFileType ( const string& str ) const;
};

#endif

