/*!
 * @file GetAboutCommand.h
 *
 * Provides application logging functionality
 * to ensure diagnostics information can be captured
 * until fully tested.
 *
 *
 * Written by Kevin White.
 *
 * This file is part of the LS library.
 *
 */

#ifndef _APPLOGGER_H
#define _APPLOGGER_H

#include "ArduinoLog.h"
#include "DomainInterfaces.h"
#include "MemoryFree.h"
#include <string>

// #define DISABLE_LOGGING         // Enable to disable logging
using namespace std;

namespace LS {
    class AppLogger : public IAppLogger {
        public:
            void StartLogging() {
                Serial.begin(115200);
                Log.begin(LOG_LEVEL_VERBOSE, &Serial);
            }

            /*!
              @brief   Logs an event that has occurred, such as the start of the execution
                       cycle.  Events are logged in the following standard format:
                       [event]\t\t[trigger]\t\t[elapsed time in ms]\t[start time in ms]\t[end time in ms]\t[additonal msg]
              @param   start   The start time of the event in milliseconds
              @param   level   The 'level' of the event i.e. embedding level
              @param   event   The name of the event i.e. cycle for the execution cycle
              @param   trigger The trigger of the event i.e. started
              @param   isStartEvent True if the event being logged is the 'start' event e.g. at the beginning of a block.
              @param   end     The end time of the event in milliseconds (can be 0)
              @param   msg     Any additional message relevant to the event (can be null)
            */
            void logEvent(uint32_t start, uint8_t level, const char* event, const char* trigger, bool isStartEvent = true, uint32_t end = 0, const char* msg = nullptr);
    };
}
#endif