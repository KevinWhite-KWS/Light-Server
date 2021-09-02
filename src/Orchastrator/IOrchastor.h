#ifndef _Orchastor_H
#define _Orchastor_H

namespace LS {
	/*!
	@brief  Interface that defines the contract for a class that acts as the
			orchastrating instance for executing commands on the Light Server.
	@author	Kevin White
	@date	21 Jan 21
	*/
	class IOrchastor
	{
	public:
		virtual void StopPrograms() = 0;

		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual bool Execute(bool isInSetupMode) = 0;
	};
}

#endif
