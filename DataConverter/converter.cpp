#include "common.h"
#include "FileReader.h"
#include "FileConfigReader.h"
#include "Utils.h"
#include <memory>

/**
 * @file converter.cpp
 * @brief This is the file containing the main of the program.
 */

/**
 * @brief Prints help informations.
 */
void help();

/**
 * @brief Main function of the program.
 * @param argc
 * @param argv
 * @return 0 if everything went fine
 */
int main ( int argc, char* argv[] )
{
	unsigned int j;
	string pathDataset;
	string Text;
	string inPath;
	string outPath;
	string textPath;
	string configPath;
	ofstream outFile;
	ifstream textFile;
	uint8_t BinText[1024];
	char tempc;
	uint32_t temp32;
	string tempString;
	uint8_t headerSpace[10];
	unsigned int BinTextLen;
	uint32_t writtenTraces = 0;
	unsigned int NSample;
	char Format;
	unsigned int FormatSize;
	if ( argc == 2 && ( !string ( argv[1] ).compare ( "-h" ) || !string ( argv[1] ).compare ( "--help" ) ) ) {
		help();
		exit ( 0 );
	} else if ( argc < 3 ) {
		cout << "Error in command line" << endl;
		help();
		exit ( 1 );
	} else if ( argc == 3 ) {
		textPath = string ( argv[1] );
		configPath = argv[2];
	} else {
		cout << "Error in command line" << endl;
		help();
		exit ( 1 );
	}
	FileConfigReader fcr ( configPath );
	textFile.open ( textPath.c_str() );
	if ( !textFile.is_open() ) {
		cerr << "Error opening plain/ciphertext file: " << textPath.c_str() << endl;
		exit ( 1 );
	}
	auto_ptr<FileReader> fr ( new FileReader ( fcr ) );
	double numOut = ceil ( ( float ) fr->getTraces().size() / ( float ) fcr.getOutputTracePerFile() );
	for ( int i = 0; i < numOut; i++ ) {
		int numTraces = 0;
		outPath = Utils::adjustPath ( fcr.getOutputPath() ) + Utils::createOutputName ( fcr.getOutputName(), i + 1, numOut );
		outFile.open ( outPath.c_str() );
		if ( !outFile.is_open() ) {
			cerr << "Error opening output file: " << outPath.c_str() << endl;
			exit ( 1 );
		}
		outFile.write ( ( const char* ) headerSpace, 10 );
		for ( j = 0; ( ( j < fcr.getOutputTracePerFile() ) && ( writtenTraces < fr->getTraces().size() ) ); j++ ) {
			getline ( textFile, Text );
			Text = Utils::CleanString ( Text );
			if ( !Utils::isHexText ( Text ) ) {
				cerr << "Error during parsing of plain/ciphertext file: " << textPath.c_str() << endl;
				cerr << "Line " << writtenTraces + 1 << " is not an hex string" << endl;
				exit ( 1 );
			}
			if ( writtenTraces == 0 ) {
				NSample = fr->getNSample();
				Format = fr->getFormatType();
				FormatSize = fr->getFormatSize();
				BinTextLen = Text.size() / 2;
			}
			Utils::HexToBin ( Text, BinText );
			outFile.write ( ( const char* ) fr->getTraces().at ( ( numOut - 1 ) *i + j ), NSample * FormatSize );
			outFile.write ( ( const char* ) BinText, BinTextLen );
			writtenTraces++;
			numTraces++;
		}
		outFile.seekp ( 0 );
		temp32 = numTraces;
		outFile.write ( ( const char* ) &temp32, 4 );
		temp32 = NSample;
		outFile.write ( ( const char* ) & ( temp32 ), sizeof ( uint32_t ) );
		tempc = Format;
		outFile.write ( ( const char* ) &tempc, 1 );
		outFile.write ( ( const char* ) & ( BinTextLen ), sizeof ( uint8_t ) );
		outFile.close();
	}
}

void help()
{
	cout << "##### HELP #####\n" << endl;
	cout << "Use:" << endl;
	cout << "  ./converter source1 source2" << endl;
	cout << "  source1: full path of the plain/ciphertext file." << endl;
	cout << "  source2: full path of the converter configuration file.\n" << endl;
	cout << "Options:\n  -h Prints help.\n  --help Prints help.\n" << endl;
}