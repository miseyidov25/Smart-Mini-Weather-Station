#include "../include/events.h"

// global variables
char * eventEnumToText[] =
{
    "E_NO",                    ///< Uninitialized event

    "E_START",                 ///< Program has started
    "E_INIT_DONE",             ///< System initialization completed
    "E_INIT_ERROR",    
    
    "E_DASHBOARD_INIT",        ///< Dashboard initialized
    "E_DASHBOARD_PAUSE",       ///< Main menu paused
    "E_DASHBOARD_EXIT",        ///< Main menu exited

    "E_SWITCH_UNITS",          ///< Switch temperature units
    "E_SHOW_HISTORY",          ///< Show history command

    "E_ENV_UPDATED",           ///< Environment model has been updated
    "E_SENSOR_READ",           ///< Simulated sensor data ready

    "E_STOP"                   ///< User or system requests shutdown
};
