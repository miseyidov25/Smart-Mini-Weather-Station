#include "../include/events.h"

// global variables
char * eventEnumToText[] =
{
    "E_NO",                    ///< Uninitialized event

    "E_START",                 ///< Program has started
    "E_INIT_DONE",             ///< System initialization completed
    "E_SENSOR_INIT_DONE",      ///< Sensors initialized successfully
    "E_INIT_ERROR",            ///< Initialization or config error

    "E_TICK",                  ///< Main simulation tick (time to simulate new step)
    "E_ENV_UPDATED",           ///< Environment model has been updated
    "E_SENSOR_READ",           ///< Simulated sensor data ready
    "E_DATA_PROCESSED",        ///< Processing finished (dew point, trends, etc.)
    "E_DATA_STORED",           ///< Data storage completed

    "E_PAUSE",                 ///< User paused simulation
    "E_RESUME",                ///< User resumed simulation

    "E_ERROR",                 ///< Runtime error occurred
    "E_RECOVERED",             ///< Recovery successful

    "E_STOP"                   ///< User or system requests shutdown
};
