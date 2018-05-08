#include "String.h"
#include "windows.h"

bool WStr2Str( const std::wstring& wstr, std::string& str )
{
	uint len = wstr.length( );
	str.resize( len );

	uint result = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR) wstr.c_str(), len, (LPSTR) str.c_str(), len, nullptr, nullptr );
	if ( result == false )
		return false;

	return true;
}