#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include "fsm_functions/fsm.h"
#include "console_functions/keyboard.h"
#include "console_functions/display.h"
#include "console_functions/devConsole.h"
#include "console_functions/systemErrors.h"

#include "../include/events.h"
#include "../include/states.h"

extern char * eventEnumToText[];
extern char * stateEnumToText[];

state_t state;
event_t event;
state_t previousState;
event_t previousEvent;

// --------------------
// Local state handlers
// --------------------
void S_Start_onEntry(void);
void S_InitSystem_onEntry(void);
void S_InitSensors_onEntry(void);
void S_Idle_onEntry(void);
void S_UpdateEnvironment_onEntry(void);
void S_ReadSensors_onEntry(void);
void S_ProcessData_onEntry(void);
void S_StoreData_onEntry(void);
void S_Pause_onEntry(void);
void S_Error_onEntry(void);
void S_Recover_onEntry(void);

// Exception function, for gracefull shutdown
void S_Stop_onEntry(void);

void delay_ms(uint32_t d);

int main(void) {

	// --------------------
    // Register states
    // --------------------

	//           State                                   onEntry()                    onExit()
	FSM_AddState(S_START,              &(state_funcs_t){ S_Start_onEntry,             NULL });
	FSM_AddState(S_INITIALISE_SYSTEM,  &(state_funcs_t){ S_InitSystem_onEntry,        NULL });
	FSM_AddState(S_INITIALISE_SENSORS, &(state_funcs_t){ S_InitSensors_onEntry,       NULL });
	FSM_AddState(S_IDLE,               &(state_funcs_t){ S_Idle_onEntry,              NULL });
	FSM_AddState(S_UPDATE_ENVIRONMENT, &(state_funcs_t){ S_UpdateEnvironment_onEntry, NULL });
	FSM_AddState(S_READ_SENSORS,       &(state_funcs_t){ S_ReadSensors_onEntry,       NULL });
	FSM_AddState(S_PROCESS_DATA,       &(state_funcs_t){ S_ProcessData_onEntry,       NULL });
	FSM_AddState(S_STORE_DATA,         &(state_funcs_t){ S_StoreData_onEntry,         NULL });
	FSM_AddState(S_PAUSE,              &(state_funcs_t){ S_Pause_onEntry, 			  NULL });
	FSM_AddState(S_ERROR,              &(state_funcs_t){ S_Error_onEntry, 			  NULL });
	FSM_AddState(S_RECOVER,            &(state_funcs_t){ S_Recover_onEntry, 		  NULL });
	FSM_AddState(S_STOP,               &(state_funcs_t){ S_Stop_onEntry,              NULL });

    // --------------------
    // Register transitions
    // --------------------

   //                                  From                     Event                To
    FSM_AddTransition(&(transition_t){ S_START,              E_START,             S_INITIALISE_SYSTEM});
    FSM_AddTransition(&(transition_t){ S_INITIALISE_SYSTEM,  E_INIT_DONE,         S_INITIALISE_SENSORS});
    FSM_AddTransition(&(transition_t){ S_INITIALISE_SYSTEM,  E_INIT_ERROR,        S_ERROR});
    FSM_AddTransition(&(transition_t){ S_INITIALISE_SENSORS, E_SENSOR_INIT_DONE,  S_IDLE});
    FSM_AddTransition(&(transition_t){ S_IDLE,               E_TICK,              S_UPDATE_ENVIRONMENT});
    FSM_AddTransition(&(transition_t){ S_UPDATE_ENVIRONMENT, E_ENV_UPDATED,       S_READ_SENSORS});
    FSM_AddTransition(&(transition_t){ S_READ_SENSORS,       E_SENSOR_READ,       S_PROCESS_DATA});
    FSM_AddTransition(&(transition_t){ S_PROCESS_DATA,       E_DATA_PROCESSED,    S_STORE_DATA});
    FSM_AddTransition(&(transition_t){ S_STORE_DATA,         E_DATA_STORED,       S_IDLE});
    FSM_AddTransition(&(transition_t){ S_IDLE,               E_PAUSE,             S_PAUSE});
    FSM_AddTransition(&(transition_t){ S_PAUSE,              E_RESUME,            S_IDLE});
    FSM_AddTransition(&(transition_t){ S_ERROR,              E_RECOVERED,         S_IDLE});

    FSM_FlushEnexpectedEvents(true);

    // --------------------
    // Start FSM
    // --------------------
    FSM_RunStateMachine(S_START, E_START);

    return 0;
}

static event_t ReadKeyboardEvent(void)
{
    char c = KYBgetchar();

    switch (tolower(c))
    {
        case 'p': return E_PAUSE;
        case 'r': return E_RESUME;
        case 'q': return E_STOP;
        default:  return E_NO;
    }
}

// --------------------
// State implementations
// --------------------

void S_Start_onEntry(void)
{
    DCSsimulationSystemInfo("Weather Station Simulation Started");
    FSM_AddEvent(E_START);
}

void S_InitSystem_onEntry(void)
{
    DSPinitialise();
    DSPshowDisplay();
    DCSinitialise();
    KYBinitialise();

    FSM_AddEvent(E_INIT_DONE);
}

void S_InitSensors_onEntry(void)
{
    // Simulated sensor init OK
    FSM_AddEvent(E_SENSOR_INIT_DONE);
}

void S_Idle_onEntry(void)
{
    delay_ms(1000);
    FSM_AddEvent(E_TICK);
}

void S_UpdateEnvironment_onEntry(void)
{
    // Simulate environment update
    DCSdebugSystemInfo("Environment updated");
    FSM_AddEvent(E_ENV_UPDATED);
}

void S_ReadSensors_onEntry(void)
{
    // Simulate sensor readings
    DCSdebugSystemInfo("Sensors read");
    FSM_AddEvent(E_SENSOR_READ);
}

void S_ProcessData_onEntry(void)
{
    // Simulate data processing
    DCSdebugSystemInfo("Data processed");
    FSM_AddEvent(E_DATA_PROCESSED);
}

void S_StoreData_onEntry(void)
{
    // Simulate logging
    DCSdebugSystemInfo("Data stored");
    FSM_AddEvent(E_DATA_STORED);
}

void S_Pause_onEntry(void)
{
    DSPshow(1, "Simulation paused");
    event_t key = ReadKeyboardEvent();
    if (key == E_RESUME)
    {
        FSM_AddEvent(E_RESUME);
    }
}

void S_Error_onEntry(void)
{
    SystemError_Report("Runtime error");
    FSM_AddEvent(E_RECOVERED);
}

void S_Recover_onEntry(void)
{
    DCSdebugSystemInfo("Recovery successful");
    FSM_AddEvent(E_RECOVERED);
}

void S_Stop_onEntry(void)
{
    DCSsimulationSystemInfo("Simulation stopped");
    exit(0);
}

// --------------------
// Helpers
// --------------------
void delay_ms(uint32_t d)
{
   DCSdebugSystemInfo("Delay waiting for %d micro-seconds", d);
   sleep(d/1000);
}

