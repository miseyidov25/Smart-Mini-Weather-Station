#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <conio.h>

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

float g_temperature = 0.0f;
float g_humidity = 0.0f;
int   g_unit_celsius = 1; // 1=C, 0=F

float temp_history[24];
float hum_history[24];

float rand_range(float min, float max);
char get_key_nonblocking(void);


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
void S_Menu_onEntry(void);
void S_Dashboard_onEntry(void);


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
    FSM_AddState(S_MENU,               &(state_funcs_t){ S_Menu_onEntry,              NULL });
    FSM_AddState(S_DASHBOARD,          &(state_funcs_t){ S_Dashboard_onEntry,         NULL });

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
    FSM_AddTransition(&(transition_t){ S_INITIALISE_SENSORS, E_SENSOR_INIT_DONE,  S_MENU});

    FSM_AddTransition(&(transition_t){ S_MENU,               E_MENU_START,        S_DASHBOARD });
    FSM_AddTransition(&(transition_t){ S_MENU,               E_MENU_PAUSE,        S_PAUSE });
    FSM_AddTransition(&(transition_t){ S_MENU,               E_MENU_EXIT,         S_STOP });
    FSM_AddTransition(&(transition_t){ S_PAUSE,              E_RESUME,            S_MENU });
    FSM_AddTransition(&(transition_t){ S_MENU,               E_NO,                S_MENU });
    
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_NO,                S_DASHBOARD });
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_BACK_TO_MENU,      S_MENU });
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_SHOW_HISTORY,      S_HISTORY });

    FSM_AddTransition(&(transition_t){ S_IDLE,               E_TICK,              S_UPDATE_ENVIRONMENT});
    FSM_AddTransition(&(transition_t){ S_UPDATE_ENVIRONMENT, E_ENV_UPDATED,       S_READ_SENSORS});
    FSM_AddTransition(&(transition_t){ S_READ_SENSORS,       E_SENSOR_READ,       S_PROCESS_DATA});
    FSM_AddTransition(&(transition_t){ S_PROCESS_DATA,       E_DATA_PROCESSED,    S_STORE_DATA});
    FSM_AddTransition(&(transition_t){ S_STORE_DATA,         E_DATA_STORED,       S_IDLE});
    FSM_AddTransition(&(transition_t){ S_IDLE,               E_PAUSE,             S_PAUSE});
    FSM_AddTransition(&(transition_t){ S_ERROR,              E_RECOVERED,         S_IDLE});

    FSM_FlushEnexpectedEvents(false);

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
    delay_ms(5000);
    FSM_AddEvent(E_TICK);
}

void S_Menu_onEntry(void)
{
    DSPclear();
    DSPshow(0, "=== Weather Station Menu ===");
    DSPshow(1, "1 - Start simulation");
    DSPshow(2, "2 - Pause simulation");
    DSPshow(3, "q - Exit");

    char c = KYBgetchar();

    switch (tolower(c))
    {
        case '1':
            FSM_AddEvent(E_MENU_START);
            break;

        case '2':
            FSM_AddEvent(E_MENU_PAUSE);
            break;

        case 'q':
            FSM_AddEvent(E_MENU_EXIT);
            break;  

        default:
            DSPshow(4, "Invalid option");
            FSM_AddEvent(E_NO);
            break;
    }
}

void S_Dashboard_onEntry(void)
{
    while (1)
    {
        float temp = g_unit_celsius
            ? g_temperature
            : (g_temperature * 9.0f / 5.0f + 32.0f);
        const char* unit = g_unit_celsius ? "C" : "F";

        const char* icon = "[SUN]";
        if (g_humidity < 40) icon = "[SUN]";
        else if (g_humidity < 75) icon = "[CLOUD]";
        else icon = "[RAIN]";

        DSPclear();
        DSPshow(0, "=== Weather Dashboard ===");
        DSPshow(1, "Temp: %.1f %s", temp, unit);
        DSPshow(2, "Humidity: %.0f %%", g_humidity);
        DSPshow(3, "Weather: %s", icon);

        // LED indicators
        if (g_temperature < 5) DCSdebugSystemInfo("LED: BLUE (Cold)");
        else if (g_temperature > 28) DCSdebugSystemInfo("LED: RED (Hot)");
        else DCSdebugSystemInfo("LED: GREEN (Comfortable)");

        DSPshow(4, "u - switch units");
        DSPshow(5, "a - set alert");
        DSPshow(6, "h - history");
        DSPshow(7, "b - back to menu");

        // Non-blocking keyboard on Windows
        char c = get_key_nonblocking();
        if (c) {
            switch (tolower(c)) {
                case 'u': g_unit_celsius = !g_unit_celsius; break;
                case 'a': DSPshow(8, "Alert set (simulated)"); delay_ms(1000); break;
                case 'h': FSM_AddEvent(E_SHOW_HISTORY); return;
                case 'b': FSM_AddEvent(E_BACK_TO_MENU); return;
                default: break;
            }
        }

        delay_ms(1000);
    }
}




void S_UpdateEnvironment_onEntry(void)
{
    // Simulate environment update
    DCSdebugSystemInfo("Environment updated");
    FSM_AddEvent(E_ENV_UPDATED);
}

void S_ReadSensors_onEntry(void)
{
    // Generate realistic fake sensor data only once per read
    g_temperature = rand_range(-5.0f, 35.0f);  // Celsius
    g_humidity    = rand_range(20.0f, 90.0f);  // Percent

    DCSdebugSystemInfo("Sensors read: Temp=%.1f C, Humidity=%.1f%%", g_temperature, g_humidity);

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

float rand_range(float min, float max)
{
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

char get_key_nonblocking(void) {
    if (_kbhit()) {
        return _getch();  // read key without waiting for Enter
    }
    return 0; // no key pressed
}