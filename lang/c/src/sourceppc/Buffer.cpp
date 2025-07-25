#include <sourceppc/Buffer.h>

#include <cstdlib>

#include <sourceppc/Helpers.h>

SOURCEPP_STATIC(sourcepp, buffer, sourcepp_buffer_t, new, size_t size) {
	sourcepp_buffer_t buffer;
	if (size > 0) {
		buffer.size = static_cast<int64_t>(size);
		buffer.data = static_cast<uint8_t*>(std::malloc(sizeof(uint8_t) * size));
	} else {
		buffer.size = 0;
		buffer.data = nullptr;
	}
	return buffer;
}

SOURCEPP_STATIC(sourcepp, buffer, void, free, sourcepp_buffer_t* buffer) {
	SOURCEPP_EARLY_RETURN(buffer);

	if (buffer->data) {
		std::free(buffer->data);
		buffer->data = nullptr;
	}
	buffer->size = 0;
}
