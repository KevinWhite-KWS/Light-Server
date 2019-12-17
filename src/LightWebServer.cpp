#include "LightWebServer.h"

namespace LS{
	LightWebServer::LightWebServer(IWebServer* webServer, FixedSizeCharBuffer* loadingBuffer) {
		this->webServer = webServer;
		this->loadingBuffer = loadingBuffer;

		// Initialises the event hooks
		webServer->addCommand("program", &LightWebServer::HandleCommandLoadProgram);
		webServer->setDefaultCommand(&LightWebServer::HandleCommandInvalid);
		webServer->setFailureCommand(&LightWebServer::HandleCommandInvalid);

		webServer->setLightWebServer(this);
	}

	void LightWebServer::Start() {
		webServer->begin();
	}

	char* LightWebServer::GetLoadingBuffer(bool clearBuffer) {
		if (clearBuffer) {
			loadingBuffer->ClearBuffer();
		}
		return loadingBuffer->GetBuffer();
	}

	void LightWebServer::SetCommandType(CommandType commandType) {
		currentCommand = commandType;
	}

	void LightWebServer::HandleCommandInvalid(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		lightWebServer->SetCommandType(CommandType::INVALID);
	}

	void LightWebServer::HandleCommandLoadProgram(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		lightWebServer->SetCommandType(CommandType::LOADPROGRAM);

		char* loadingBuffer = lightWebServer->GetLoadingBuffer();
		char b = 0;

		while ((b = server.read()) != -1
			&& b != 255) {			// Cruically important to ALSO check for 255.  Otherwise, will be stuck in an infinite loop.
			Serial.print(b);

			*loadingBuffer = b;
			loadingBuffer++;
		}
	}

	CommandType LightWebServer::HandleNextCommand() {
		currentCommand = CommandType::NONE;

		char* buf = GetLoadingBuffer();
		int bufLen = loadingBuffer->GetBufferSize();
		webServer->processConnection(buf, &bufLen);

		return currentCommand;
	}

	void LightWebServer::RespondOK() {
		webServer->httpSuccess();
		webServer->closeConnection();
	}

	void LightWebServer::RespondError() {
		webServer->httpFail();
		webServer->closeConnection();
	}
}