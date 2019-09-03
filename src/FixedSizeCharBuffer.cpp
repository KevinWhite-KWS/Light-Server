#include "FixedSizeCharBuffer.h"



namespace LS {
	/*!
	  @brief	Constructor allocates the memory required for the fixed sized buffer and it is set to 0.
	  @param	bufferSize		The size of the buffer to be allocated.
	*/
	FixedSizeCharBuffer::FixedSizeCharBuffer(uint16_t bufferSize) {
		this->bufferSize = bufferSize;
		this->buffer = (char*)calloc(bufferSize, sizeof(char));
	}

	/*!
	  @brief	Destructor ensures the memory allocated to the buffer is freed.
	*/
	FixedSizeCharBuffer::~FixedSizeCharBuffer() {
		free(this->buffer);
	}

	/*!
	  @brief	Returns a reference to the allocated buffer.
	  @returns	A pointer to the char buffer.
	*/
	char* FixedSizeCharBuffer::GetBuffer() {
		return this->buffer;
	}

	/*!
	  @brief	Clears the allocated buffer by resetting the value to 0.
	*/
	void FixedSizeCharBuffer::ClearBuffer() {
		memset(this->buffer, 0, this->bufferSize);
	}

	/*!
	  @brief	Loads the internal buffer from an existing buffer.  If the bufferToCopy is nullptr then this function leaves the internal bufffer untouched.
	  @param	bufferToCopy	The buffer from which to copy to the internal buffer.
	*/
	void FixedSizeCharBuffer::LoadFromBuffer(const char* bufferToCopy) {
		if (bufferToCopy == nullptr) return;

		const char* pBufferToCopy = bufferToCopy;
		char* internalBuffer = this->buffer;
		uint16_t countCopied = 0;
		uint16_t bufferSize = this->bufferSize-1;
		char currentChar;

		while ((currentChar = *pBufferToCopy++) != '\0' && countCopied++ < bufferSize) {
			*internalBuffer++ = currentChar;
		}

		*internalBuffer = '\0';
	}

	/*!
      @brief	Gets the size of the allocated buffer.
	  @returns	The size in bytes of the allocated buffer.
	*/
	const uint16_t FixedSizeCharBuffer::GetBufferSize() {
		return this->bufferSize;
	}
}