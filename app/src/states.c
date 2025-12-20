#include "../include/states.h"

char * stateEnumToText[] =
{
    "S_NO",                         ///< Unknown state (initial uninitialized state)
    "S_START",                      ///< Program start

    "S_INITIALISE_SYSTEM",          ///< Initialize simulation engine, load config
    "S_INITIALISE_SENSORS",         ///< Setup simulated sensors

    "S_MENU",                       ///< Main menu state
    "S_DASHBOARD",                  ///< Dashboard display state
    "S_HISTORY",                    ///< History display state

    "S_IDLE",                       ///< Waiting state between cycles
    "S_UPDATE_ENVIRONMENT",         ///< Update simulated environment (temperature, wind, etc.)
    "S_READ_SENSORS",               ///< Read simulated sensor values
    "S_PROCESS_DATA",               ///< Process readings (dew point, trending, etc.)
    "S_STORE_DATA",                 ///< Store logs or results

    "S_ERROR",                      ///< Error occurred
    "S_RECOVER",                    ///< Attempt to recover after an error

    "S_PAUSE",                      ///< Simulation paused
    "S_STOP"                        ///< End / shutdown simulation
};
