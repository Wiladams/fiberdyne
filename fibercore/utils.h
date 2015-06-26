#pragma once

#include <stdint.h>

namespace hexutils {

bool hex_decode( const uint8_t *data, uint8_t **decoded, uint64_t &length );
bool hex_print( const uint8_t *data, uint64_t length );
bool hex_puts(const uint8_t *data, uint64_t length);


}