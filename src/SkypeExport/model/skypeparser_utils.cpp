#include "skypeparser.h"

namespace SkypeParser
{
    bool isForbiddenCharacter( char c )
    {
        static std::string forbiddenChars( "\\/:?\"<>|*" );
        return ( std::string::npos != forbiddenChars.find( c ) );
    }

    std::string makeSafeFilename( const std::string &input, char replacement )
    {
        std::string result = input;
        std::replace_if( result.begin(), result.end(), isForbiddenCharacter, replacement );
        return result;
    }
}
