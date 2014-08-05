#include "FileReader.h"
#include <stdexcept>

FileReader::FileReader ( FileConfigReader& settings )
{
	for ( UINTN i = 0; i < settings.getFileList().size(); i++ ) {
		int fileType;
		FileName = settings.getFileList().at ( i );
		DataFile.open ( FileName.c_str() );
		if ( !DataFile.is_open() ) {
			cerr << "Error opening file: " << FileName.c_str() << endl;
			exit ( 2 );
		}
		DataFile.close();
		fileType = settings.getInputFormat();
		switch ( fileType ) {
			case FILE_TYPE_LECROY:
				readLecroy ( settings );
				break;
			case FILE_TYPE_AGILENT:
				readAgilent ( settings );
				break;
			case FILE_TYPE_TXT:
				readTxt ( settings );
				break;
			default: {
					cerr << "Invalid format." << endl;
					exit ( 2 );
				}
		}
	}
}

FileReader::~FileReader()
{
	for ( int i = 0; i < Traces.size(); i++ ) { delete[] Traces[i]; }
}

UINTN FileReader::getNSample()
{
	return NSample;
}

char FileReader::getFormatType()
{
	return FormatType;
}

UINTN FileReader::getFormatSize()
{
	return FormatSize;
}

const vector<uint8_t*>& FileReader::getTraces()
{
	return Traces;
}

bool FileReader::isLecroy()
{
	char buf[100];
	string buf_s;
	UINTN p1, p2;
	DataFile.read ( buf, 100 );
	buf_s.assign ( ( char* ) buf, 100 );
	p1 = buf_s.find ( "WAVEDESC" );
	if ( p1 == buf_s.npos ) {
		return false;
	}
	p2 = buf_s.find ( "LECROY" );
	if ( p2 - p1 != 16 || p1 == buf_s.npos ) {
		return false;
	}
	return true;
}

bool FileReader::isAgilent()
{
	char id[2];
	string buf_s;
	DataFile.read ( id, 2 );
	buf_s.assign ( ( char* ) id, 2 );
	if ( buf_s.compare ( "AG" ) == 0 ) {
		return true;
	}
	return false;
}

void FileReader::readLecroy ( FileConfigReader& settings )
{
	DataFile.open ( FileName.c_str() );
	if ( isLecroy() ) {
		DataFile.close();
		cout << "...analyzing Lecroy file...";
		char buf[344];
		string buf_s;
		int p1;
		uint16_t temp16_1, temp16_2, temp16_3;
		uint8_t* Trace;
		UINTN file_size, data_start, data_offset;
		DataFile.open ( FileName.c_str() );
		DataFile.seekg ( 0, ios::end );
		file_size = DataFile.tellg();
		if ( file_size < 344 ) {
			cerr << "Error during conversion of: " << FileName.c_str() << endl;
			cerr << "File too small." << endl;
			exit ( 2 );
		}
		DataFile.seekg ( 0, ios::beg );
		DataFile.read ( buf, 344 );
		buf_s.assign ( ( char* ) buf, 344 );
		p1 = buf_s.find ( "WAVEDESC" );
		if ( p1 == -1 ) {
			cerr << "Error during conversion of: " << FileName.c_str() << endl;
			cerr << "Invalid format..." << endl;
			exit ( 2 );
		}
		memcpy ( &temp16_1, &buf[p1 + 32], 2 );
		if ( temp16_1 == 0 ) {
			FormatType = 'b';
			FormatSize = sizeof ( uint8_t );
		} else if ( temp16_1 == 1 ) {
			FormatType = 'c';
			FormatSize = sizeof ( uint16_t );
		} else {
			cerr << "Error during conversion of: " << FileName.c_str() << endl;
			cerr << "Invalid Matlab format. It must be single or double." << endl;
			exit ( 2 );
		}
		NSample = settings.getOutputTraceLength();
		memcpy ( &temp16_1, &buf[p1 + 36], 2 );
		memcpy ( &temp16_2, &buf[p1 + 40], 2 );
		memcpy ( &temp16_3, &buf[p1 + 48], 2 );
		data_start = p1 + temp16_1 + temp16_2 + temp16_3;
		data_offset = settings.getOutputTraceOffset() * FormatSize;
		if ( data_start > file_size ) {
			cerr << "Error during conversion of: " << FileName.c_str() << endl;
			cerr << "Bad data start: " << data_start << endl;
			exit ( 2 );
		}
		if ( NSample * FormatSize > ( file_size - ( data_start + data_offset ) ) ) {
			cerr << "Error during conversion of: " << FileName.c_str() << endl;
			cerr << "Check configuration settings. Samples in file are less than samples requested." << endl;
			exit ( 2 );
		}
		Trace = new uint8_t[NSample * FormatSize];
		DataFile.seekg ( data_start + data_offset, ios::beg );
		DataFile.read ( ( char* ) Trace, NSample * FormatSize );
		cout << "...done!" << endl;
		Traces.push_back ( Trace );
	} else {
		DataFile.close();
		cerr << "Invalid format: expected Lecroy file" << endl;
		exit ( 2 );
	}
	DataFile.close();
}

void FileReader::readAgilent ( FileConfigReader& settings )
{
	DataFile.open ( FileName.c_str() );
	if ( isAgilent() ) {
		DataFile.close();
		cout << "...analyzing Agilent file...";
		AgilentFileHeader fileHeader;
		uint8_t* Trace;
		DataFile.open ( FileName.c_str() );
		DataFile.seekg ( 0, ios::beg );
		DataFile.read ( fileHeader.cookie, 2 );
		DataFile.read ( fileHeader.version, 2 );
		DataFile.read ( ( char* ) &fileHeader.fileSize, sizeof ( int ) );
		DataFile.read ( ( char* ) &fileHeader.numberOfWaveforms, sizeof ( int ) );
		for ( UINTN j = 0; j < fileHeader.numberOfWaveforms; j++ ) {
			AgilentWaveformHeader waveHeader;
			DataFile.read ( ( char* ) &waveHeader.HeaderSize, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveHeader.WaveformType, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveHeader.NWaveformBuffers, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveHeader.Points, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveHeader.Count, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveHeader.XDisplayRange, sizeof ( float ) );
			DataFile.read ( ( char* ) &waveHeader.XDisplayOrigin, sizeof ( double ) );
			DataFile.read ( ( char* ) &waveHeader.XIncrement, sizeof ( double ) );
			DataFile.read ( ( char* ) &waveHeader.XOrigin, sizeof ( double ) );
			DataFile.read ( ( char* ) &waveHeader.XUnits, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveHeader.YUnits, sizeof ( int ) );
			DataFile.read ( waveHeader.Date, 16 );
			DataFile.read ( waveHeader.Time, 16 );
			DataFile.read ( waveHeader.Frame, 24 );
			DataFile.read ( waveHeader.WaveformLabel, 16 );
			DataFile.read ( ( char* ) &waveHeader.TimeTag, sizeof ( double ) );
			DataFile.read ( ( char* ) &waveHeader.SegmentIndex, sizeof ( int ) );
			AgilentWaveformDataHeader waveDataHeader;
			DataFile.read ( ( char* ) &waveDataHeader.HeaderSize, sizeof ( int ) );
			DataFile.read ( ( char* ) &waveDataHeader.BufferType, sizeof ( short ) );
			DataFile.read ( ( char* ) &waveDataHeader.BytesPerPoint, sizeof ( short ) );
			DataFile.read ( ( char* ) &waveDataHeader.BufferSize, sizeof ( int ) );
			if ( waveDataHeader.BytesPerPoint == 4 ) {
				FormatType = 'f';
			} else if ( waveDataHeader.BytesPerPoint == 1 ) {
				FormatType = 'b';
			} else {
				cerr << "Error during conversion of: " << FileName.c_str() << endl;
				cerr << "Type of Agilent data must be float ('f') or int8 ('b')" << endl;
				exit ( 2 );
			}
			Trace = new uint8_t[waveDataHeader.BufferSize];
			FormatSize = waveDataHeader.BytesPerPoint;
			NSample = settings.getOutputTraceLength();
			DataFile.seekg ( settings.getOutputTraceOffset() *waveDataHeader.BytesPerPoint, ios::cur );
			DataFile.read ( ( char* ) Trace, settings.getOutputTraceLength() *waveDataHeader.BytesPerPoint );
			Traces.push_back ( Trace );
		}
		cout << "...done!" << endl;
	} else {
		DataFile.close();
		cerr << "Invalid format: expected Agilent file" << endl;
		exit ( 2 );
	}
	DataFile.close();
}

void FileReader::readTxt ( FileConfigReader& settings )
{
	//Added by TFT: support for reading text files
	FormatType = 'f'; //The Txt reader expects a file with floating point numbers
	FormatSize = 4;
	NSample = settings.getOutputTraceLength();
	DataFile.open ( FileName.c_str() );
	for ( int i = 0; i < settings.getOutputTraceOffset(); i++ ) {
		float f;
		DataFile >> f; //Discard
	}
	if ( !DataFile ) { throw ( runtime_error ( string ( "File " ) + FileName + " is too short" ) ); }
	float* data = new float[NSample];
	for ( int i = 0; i < NSample; i++ ) {
		float f;
		DataFile >> f;
		data[i] = -f; //NOTE: the numbers found in the txt file are *negated*
	}
	if ( !DataFile ) {
		delete[] data;
		throw ( runtime_error ( string ( "File " ) + FileName + " is too short" ) );
	}
	Traces.push_back ( reinterpret_cast<uint8_t*> ( data ) );
	DataFile.close();
}

