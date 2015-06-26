
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** 
 * Check to see if the input starts with "0x"; if it does, return the decoded
 * bytes of the following data (presumed to be hex coded). If not, just return
 * the contents. This routine allocates memory, so has to be free'd.
 */
bool hex_decode( const uint8_t *input, uint8_t **decoded, uint64_t &len)
{  
	if (!decoded) return false;
	uint8_t *output = *decoded;
	if (output == nullptr) return false;
    
    if ((input[0] != '0') && (input[1] != 'x')) {
      len = strlen((char *)input);
      *decoded = (uint8_t *)calloc(1, (size_t)len+1);
      memcpy(*decoded, input, (size_t)len);
    } else {
      len = ( strlen((const char *)input ) >> 1 ) - 1;
      *decoded = (uint8_t *)malloc( (size_t)len );
      for ( size_t i = 2; i < strlen( (const char *)input ); i += 2 )
      {
        output[ ( ( i / 2 ) - 1 ) ] =
          ( ( ( input[ i ] <= '9' ) ? input[ i ] - '0' : 
          ( ( tolower( input[ i ] ) ) - 'a' + 10 ) ) << 4 ) |
          ( ( input[ i + 1 ] <= '9' ) ? input[ i + 1 ] - '0' : 
          ( ( tolower( input[ i + 1 ] ) ) - 'a' + 10 ) );
      }
    } 
 
    return true;
}

bool hex_print( const uint8_t *data, uint64_t length )
{
    while ( length-- ) {
      printf( "%.02x", *data++ );
    }

    return true;
}

bool hex_puts( const uint8_t *data, uint64_t length )
{
    hex_print(data, length);
    printf("\n");

    return true;
}
