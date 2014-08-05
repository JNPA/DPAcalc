#ifndef _common_included_
#define _common_included_

#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <stdint.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <algorithm>
#include <vector>
#include <math.h>
#include <sstream>
#include <map>
#include <typeinfo>
#include <iterator>
#include <iomanip>


/**
 * @file common.h
 * @brief This file contains some commonly used includes, constants, defines, macros and typedefs.
 *
 * This file should be included by every .h file in the project.\n
 * User should not change this file directly.\n
 * The use of std namespace is declared.\n
 */

using namespace std;

#define FILE_TYPE_LECROY 1
#define FILE_TYPE_AGILENT 2
#define FILE_TYPE_TXT 3

#define DATE_TIME_STRING_LENGTH 16
#define FRAME_STRING_LENGTH 24
#define SIGNAL_STRING_LENGTH 16

/**
 * @brief The type used to represent unsigned integer number.
 * This value should be equal to 2^n.
 */
typedef unsigned int UINTN;

/**
 * @brief The type used to represent the header of an Agilent file.
 *
 * The struct represents the equivalent header of an Agilent file
 */
typedef struct AgilentFileHeaderTag {
	char cookie[2];
	char version[2];
	unsigned int fileSize;
	unsigned int numberOfWaveforms;
} AgilentFileHeader;

/**
 * @brief The type used to represent the header of each waveform in an Agilent file.
 *
 * The struct represents the equivalent header of a waveform in an Agilent file
 */
typedef struct AgilentWaveformHeaderTag {
	unsigned int HeaderSize;
	unsigned int WaveformType;
	unsigned int NWaveformBuffers;
	unsigned int Points;
	unsigned int Count;
	float XDisplayRange;
	double XDisplayOrigin;
	double XIncrement;
	double XOrigin;
	unsigned int XUnits;
	unsigned int YUnits;
	char Date[DATE_TIME_STRING_LENGTH];
	char Time[DATE_TIME_STRING_LENGTH];
	char Frame[FRAME_STRING_LENGTH];
	char WaveformLabel[SIGNAL_STRING_LENGTH];
	double TimeTag;
	unsigned int SegmentIndex;
} AgilentWaveformHeader;

/**
 * @brief The type used to represent the data header of each waveform in an Agilent file.
 *
 * The struct represents the equivalent data header of a waveform in an Agilent file
 */
typedef struct AgilentWaveformDataHeaderTag {
	unsigned int HeaderSize;
	unsigned short BufferType;
	unsigned short BytesPerPoint;
	unsigned int BufferSize;
} AgilentWaveformDataHeader;

#endif
