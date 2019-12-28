#include "LightWebServer.h"

namespace LS{
	LightWebServer::LightWebServer(IWebServer* webServer, FixedSizeCharBuffer* loadingBuffer) {
		this->webServer = webServer;
		this->loadingBuffer = loadingBuffer;

		// Initialises the event hooks
		webServer->addCommand("program", &LightWebServer::HandleCommandLoadProgram);
		webServer->addCommand("power/off", &LightWebServer::HandleCommandPowerOff);
		webServer->addCommand("power/on", &LightWebServer::HandleCommandPowerOn);
		webServer->addCommand("power", &LightWebServer::HandleCommandCheckPower);
		webServer->addCommand("about", &LightWebServer::HandleCommandGetAbout);
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

	CommandType LightWebServer::GetCommandType() {
		return currentCommand;
	}

	void LightWebServer::LoadBody(ILightWebServer* lightWebServer, IWebServer& server) {
		char* loadingBuffer = lightWebServer->GetLoadingBuffer();
		char b = 0;

		while ((b = server.read()) != -1
			&& b != 255) {			// Cruically important to ALSO check for 255.  Otherwise, will be stuck in an infinite loop.
			*loadingBuffer = b;
			loadingBuffer++;
		}
	}

	void LightWebServer::HandleCommandInvalid(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		lightWebServer->SetCommandType(CommandType::INVALID);
	}

	void LightWebServer::HandleCommandPowerOff(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		lightWebServer->SetCommandType(CommandType::POWEROFF);
	}

	void LightWebServer::HandleCommandLoadProgram(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		lightWebServer->SetCommandType(CommandType::LOADPROGRAM);

		LightWebServer::LoadBody(lightWebServer, server);
	}

	void LightWebServer::HandleCommandPowerOn(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		lightWebServer->SetCommandType(CommandType::POWERON);

		LightWebServer::LoadBody(lightWebServer, server);
	}

	void LightWebServer::HandleCommandCheckPower(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		if (type != IWebServer::ConnectionType::GET) {
			lightWebServer->SetCommandType(CommandType::INVALID);
			return;
		}

		lightWebServer->SetCommandType(CommandType::CHECKPOWER);
	}

	void LightWebServer::HandleCommandGetAbout(ILightWebServer* lightWebServer, IWebServer& server, IWebServer::ConnectionType type, char*, bool) {
		if (type != IWebServer::ConnectionType::GET) {
			lightWebServer->SetCommandType(CommandType::INVALID);
			return;
		}

		lightWebServer->SetCommandType(CommandType::GETABOUT);
	}

	CommandType LightWebServer::HandleNextCommand() {
		currentCommand = CommandType::NONE;

		char* buf = GetLoadingBuffer();
		int bufLen = loadingBuffer->GetBufferSize();
		webServer->processConnection(buf, &bufLen);

		return currentCommand;
	}

	void LightWebServer::RespondOK(const char* str) {
		webServer->httpSuccess();

		if (str != nullptr) {
			webServer->printP(str);
		}

		webServer->closeConnection();
	}

	void LightWebServer::RespondNoContent() {
		webServer->httpNoContent();
		webServer->closeConnection();
	}

	void LightWebServer::RespondError() {
		webServer->httpFail();
		webServer->closeConnection();
	}
}