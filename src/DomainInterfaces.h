#ifndef _DOMAININTERFACES_H
#define _DOMAININTERFACES_H

namespace LS {
	/*!
	@brief  Interface that defines the contract for a class that controls a set of LEDs.
	*/
	class IPixelController
	{
		public:
			IPixelController() { }
			virtual void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) = 0;
			virtual void show() = 0;
			virtual uint32_t getPixelColor(uint16_t n) const = 0;
	};
}

#endif
