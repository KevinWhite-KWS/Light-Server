#include "LpiJsonInstructionValidator.h"

namespace LS {
	/*!
		@brief		Constructs a new LPInstructionValidator instance.
		@param		factory		Pointer to the LpiExecitorFactory class that gets instances of LpiExecutors.
		@author		Kevin White
		@date		14 Dec 2020
	*/
	//LpiJsonInstructionValidator::LpiJsonInstructionValidator(LPIFactory* factory) {
	//	lpiFactory = factory;
	//}
	LpiJsonInstructionValidator::LpiJsonInstructionValidator(LpiExecutorFactory* factory, StringProcessor* stringProcessor, LEDConfig* ledConfig) {
		lpiExecutorFactory = factory;
		this->stringProcessor = stringProcessor;
		this->ledConfig = ledConfig;
		lpiExecutorParams.Reset(&lpiToBeValidatedBuffer, this->ledConfig, this->stringProcessor);
	}

	/*!
		@brief		Validates the LP instruction
		@param		jsonVar		Pointer to the elemnt that contains the LPI string
		@returns	True if the LPI is well formed and valid according to the requirements
					of the specific LPI.
		@author		Kevin White
		@date		14 Dec 2020
	*/
	void LpiJsonInstructionValidator::Validate(JsonVariant* jsonVar, LPValidateResult* result) {
		if (jsonVar == nullptr) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}

		const char* lpiStr = jsonVar->as<char*>();
		if (lpiStr == nullptr) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}

		// extract the basic LPI details - this validates that they are valid
		bool isValid = stringProcessor->ExtractLPIFromHexEncoded(lpiStr, &lpiToBeValidated);
		if (!isValid) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}

		// now, validate the specific LPI
		lpiToBeValidatedBuffer.LoadFromBuffer(lpiStr);
		LpiExecutor* lpiExecutor = lpiExecutorFactory->GetLpiExecutor(lpiToBeValidated.opcode);

		isValid = lpiExecutor->ValidateLpi(&lpiExecutorParams);
		if (!isValid) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}

		// now, get the LPI and validate the actual instruction against the
		// rules specific to that LPI
		// lpiToBeValidatedBuffer.LoadFromBuffer(lpiStr);
		// LPI* lpi = lpiFactory->GetLPI(&lpiToBeValidatedBuffer, &lpiToBeValidated);
		/*if (lpi == nullptr) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}

		if (!lpi->Validate()) {
			result->ResetResult(LPValidateCode::InvalidInstruction);
			return;
		}*/
	}
}