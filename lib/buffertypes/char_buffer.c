#include "char_buffer.h"

#define BUFFER_TYPE          char_buffer
#define ELEMENT_TYPE         char
#define BUFFER_SIZE_TYPE     uint32_t
#include "buffer.h"
#undef BUFFER_TYPE
#undef ELEMENT_TYPE
#undef BUFFER_SIZE_TYPE
