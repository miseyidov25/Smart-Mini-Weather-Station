#include "../include/events.h"

// global variables
char * eventEnumToText[] =
{
    "E_NO",                    ///< Uninitialized event

    "E_START",                 ///< Program has started
    "E_INIT_DONE",             ///< System initialization completed
    "E_SENSOR_INIT_DONE",      ///< Sensors initialized successfully
    "E_INIT_ERROR",    
    
    "E_MENU_START",            ///< Main menu started
    "E_MENU_PAUSE",            ///< Main menu paused
    "E_MENU_EXIT",             ///< Main menu exited

    "E_SHOW_HISTORY",          ///< Show history command
    "E_BACK_TO_MENU",          ///< Back to menu command

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
