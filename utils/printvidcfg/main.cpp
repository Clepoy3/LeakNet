#include <iostream>
using namespace std;

typedef struct _GUID
{
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
} GUID;

int main()
{
/*
	FileHandle_t file = FileSystem()->Open( "vidcfg.bin", "rb", "EXECUTABLE_PATH" );
	if ( file )
	{
		GUID deviceId;
		int texSize;
		FileSystem()->Read( &deviceId, sizeof(deviceId), file );
		FileSystem()->Read( &texSize, sizeof(texSize), file );
		FileSystem()->Close( file );

		cout << texSize << endl;
	}
*/

	GUID deviceId;
	int texSize;

	FILE *fp = fopen( "vidcfg.bin", "rb" );
	if ( fp )
	{
		fseek( fp, 0, SEEK_END );
		int len = ftell( fp );
		rewind( fp );

		fread( &deviceId, 1, sizeof(deviceId), fp );
		fread( &texSize, 1, sizeof(texSize), fp );
		fclose( fp );

		cout << texSize << endl;
	}

	system("pause");
	return 0;
}