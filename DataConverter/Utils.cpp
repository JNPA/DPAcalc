#include "Utils.h"

bool Utils::isHexText ( string s )
{
	std::string::size_type found;
	if ( s.size() == 0 || s.size() % 2 != 0 ) {
		return false;
	}
	found = s.find_first_not_of ( "0123456789ABCDEFabcdef" );
	if ( found != string::npos ) {
		return false;
	}
	return true;
}

void Utils::HexToBin ( string s, uint8_t* b )
{
	unsigned int i, j;
	char ch;
	uint8_t temp;
	for ( i = 0; i < s.size(); i += 2 ) {
		temp = 0;
		for ( j = 0; j <= 1; j++ ) {
			ch = s.c_str() [i + j];
			if ( ch >= '0' && ch <= '9' ) {
				temp = ( temp << 4 ) + ( ch - '0' );
			} else if ( ch >= 'A' && ch <= 'F' ) {
				temp = ( temp << 4 ) + ( ch - 'A' + 10 );
			} else if ( ch >= 'a' && ch <= 'f' ) {
				temp = ( temp << 4 ) + ( ch - 'a' + 10 );
			}
		}
		b[i / 2] = temp;
	}
}

string Utils::CleanString ( string s )
{
	string temp;
	char* buf;
	unsigned int i, j;
	buf = new char[s.size()];
	for ( i = 0, j = 0; i < s.size(); i++ ) {
		if ( s.c_str() [i] != ' ' && s.c_str() [i] != '\r' && s.c_str() [i] != '\n' &&
			 s.c_str() [i] != '\t' ) {
			buf[j] = s.c_str() [i];
			j++;
		}
	}
	buf[j] = '\0';
	temp = string ( buf );
	delete[] buf;
	return temp;
}

string Utils::adjustPath ( string path )
{
	if ( path.empty() ) { return "/"; }
	if ( path[path.length() - 1] != '/' ) { return path + "/"; }
	return path;
}

string Utils::createOutputName ( string name, unsigned int num, unsigned int max )
{
	/* FIXME gcc bug
	    string num_file;
	    string pad;
	    stringstream out;
	    stringstream count;
	    stringstream ss;
	    out<<num;
	    count<<max;
	    num_file=out.str();
	    pad=count.str();
	*/
	int bin_pos;
	//FIXME gcc bug, remove these lines
	char n[7];
	sprintf ( n, "%06d", num );
	bin_pos = name.find ( ".bin" );
	if ( bin_pos == -1 ) {
		/* FIXME gcc bug
		ss << setfill('0') << setw(pad.length())<< num_file;
		return name+ss.str()+".bin";
		*/
		char p[100];
		sprintf ( p, "%s%s.bin", name.c_str(), n );
		return p;
	} else {
		/* FIXME gcc bug
		ss << setfill('0') << setw(pad.length())<< num_file;
		name.insert(bin_pos,ss.str());
		return name;
		*/
		name.insert ( bin_pos, n );
		return name;
	}
}