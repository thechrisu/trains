#include "int32_t_buffer.h"

#define BUFFER_TYPE          int32_t_buffer
#define ELEMENT_TYPE         int32_t
#define BUFFER_SIZE_TYPE     uint32_t
#include "buffer.h"
#undef BUFFER_TYPE
#undef ELEMENT_TYPE
#undef BUFFER_SIZE_TYPE
