#include "FileConfigReader.h"

FileConfigReader::FileConfigReader ( const string& file_name )
{
	fileName = file_name;
	isfileList = false;
	ExtractKeys();
}

FileConfigReader::~FileConfigReader()
{
	file.close();
}

UINTN FileConfigReader::getOutputTraceLength()
{
	return OutputTraceLength;
}

UINTN FileConfigReader::getOutputTraceOffset()
{
	return OutputTraceOffset;
}

UINTN FileConfigReader::getOutputTracePerFile()
{
	return OutputTracePerFile;
}

string FileConfigReader::getOutputPath()
{
	return OutputPath;
}

string FileConfigReader::getOutputName()
{
	return OutputName;
}

const vector<string>& FileConfigReader::getFileList()
{
	return FileList;
}

int FileConfigReader::getInputFormat()
{
	return InputFormat;
}

template <typename ValueType>
ValueType FileConfigReader::getValueOfKey ( const string& key, ValueType const& defaultValue ) const
{
	if ( !keyExists ( key ) ) {
		cerr << "The key '" << key.c_str() << "' doesn't exist in file: " << fileName.c_str() << endl;
		exit ( 2 );
	}
	return string_to_T<ValueType> ( contents.find ( key )->second );
}

void FileConfigReader::ExtractKeys()
{
	file.open ( fileName.c_str() );
	if ( !file.is_open() ) {
		cerr << "Error opening attacker configuration file: " << fileName.c_str() << endl;
		exit ( 2 );
	}
	string line;
	size_t lineNo = 0;
	while ( getline ( file, line ) ) {
		lineNo++;
		string temp = line;
		if ( temp.empty() )
		{ continue; }
		removeComment ( temp );
		if ( onlyWhitespace ( temp ) )
		{ continue; }
		parseLine ( temp, lineNo );
	}
	OutputTraceLength = getValueOfKey<UINTN> ( "output_trace_length" );
	OutputTraceOffset = getValueOfKey<UINTN> ( "output_trace_offset" );
	OutputTracePerFile = getValueOfKey<UINTN> ( "output_traces_per_file" );
	OutputPath = getValueOfKey<string> ( "output_path" );
	OutputName = getValueOfKey<string> ( "output_name" );
	InputFormat = StringToFileType ( getValueOfKey<string> ( "input_format" ) );
}

void FileConfigReader::parseLine ( const string& line, size_t const lineNo )
{
	if ( line.find ( '=' ) == line.npos ) {
		string temp = line;
		temp.erase ( 0, temp.find_first_not_of ( "\t " ) );
		if ( isfileList == true ) {
			if ( temp[0] == '/' )
			{ extractFileList ( line ); }
			else {
				cerr << "Error during parsing of: " << fileName.c_str() << endl;
				cerr << "Wrong path after 'file_list:' " << temp << endl;
				exit ( 2 );
			}
		} else if ( line.find ( "file_list" ) == 0 ) {
			isfileList = true;
		} else {
			cerr << "Error during parsing of: " << fileName.c_str() << endl;
			cerr << "Couldn't find separator on line: " << lineNo << endl;
			exit ( 2 );
		}
	}
	if ( !validLine ( line ) ) {
		cerr << "Error during parsing of: " << fileName.c_str() << endl;
		cerr << "Bad format for line: " << lineNo << endl;
		exit ( 2 );
	}
	extractContents ( line );
}

void FileConfigReader::extractContents ( const string& line )
{
	string temp = line;
	temp.erase ( 0, temp.find_first_not_of ( "\t " ) );
	size_t sepPos = temp.find ( '=' );
	string key, value;
	extractKey ( key, sepPos, temp );
	extractValue ( value, sepPos, temp );
	if ( !keyExists ( key ) )
	{ contents.insert ( pair<string, string> ( key, value ) ); }
	else {
		cerr << "Error during parsing of: " << fileName.c_str() << endl;
		cerr << "Please remove duplicates. Key: '" << key.c_str() << "' already parsed" << endl;
		exit ( 2 );
	}
}

void FileConfigReader::extractFileList ( const string& line )
{
	string temp = line;
	temp.erase ( 0, temp.find_first_not_of ( "\t " ) );
	FileList.push_back ( temp );
}

void FileConfigReader::extractKey ( string& key, size_t const& sepPos, const string& line ) const
{
	key = line.substr ( 0, sepPos );
	if ( key.find ( '\t' ) != line.npos || key.find ( ' ' ) != line.npos )
	{ key.erase ( key.find_first_of ( "\t " ) ); }
}
void FileConfigReader::extractValue ( string& value, size_t const& sepPos, const string& line ) const
{
	value = line.substr ( sepPos + 1 );
	value.erase ( 0, value.find_first_not_of ( "\t " ) );
	value.erase ( value.find_last_not_of ( "\t " ) + 1 );
}

void FileConfigReader::removeComment ( string& line ) const
{
	if ( line.find ( '#' ) != line.npos )
	{ line.erase ( line.find ( '#' ) ); }
}

bool FileConfigReader::onlyWhitespace ( const string& line ) const
{
	return ( line.find_first_not_of ( ' ' ) == line.npos );
}

bool FileConfigReader::validLine ( const string& line ) const
{
	string temp = line;
	temp.erase ( 0, temp.find_first_not_of ( "\t " ) );
	if ( temp[0] == '=' )
	{ return false; }
	for ( size_t i = temp.find ( '=' ) + 1; i < temp.length(); i++ )
		if ( temp[i] != ' ' )
		{ return true; }
	return false;
}

bool FileConfigReader::keyExists ( const string& key ) const
{
	return contents.find ( key ) != contents.end();
}

int FileConfigReader::StringToFileType ( const string& str ) const
{
	if ( ( str.compare ( "lecroy" ) == 0 ) || ( str.compare ( "Lecroy" ) == 0 ) ||
		 ( str.compare ( "LECROY" ) == 0 ) || ( str.compare ( "LeCroy" ) == 0 ) ) {
		return FILE_TYPE_LECROY;
	} else if ( ( str.compare ( "agilent" ) == 0 ) || ( str.compare ( "Agilent" ) == 0 ) ||
				( str.compare ( "AGILENT" ) == 0 ) ) {
		return FILE_TYPE_AGILENT;
	} else if ( ( str.compare ( "txt" ) == 0 ) || ( str.compare ( "TXT" ) == 0 ) ||
				( str.compare ( ".txt" ) == 0 ) || ( str.compare ( ".TXT" ) == 0 ) ) {
		return FILE_TYPE_TXT;
	} else {
		cerr << "Error during parsing of: " << fileName.c_str() << endl;
		cerr << "Wrong file type: '" << str.c_str() << endl;
		exit ( 2 );
	}
}