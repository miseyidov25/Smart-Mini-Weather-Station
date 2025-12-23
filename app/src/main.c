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
void S_ReadSensors_onEntry(void);
void S_SwitchUnits_onEntry(void);
void S_Menu_onEntry(void);
void S_Dashboard_onEntry(void);
void S_Init_Dashboard_onEntry(void);


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
	FSM_AddState(S_READ_SENSORS,       &(state_funcs_t){ S_ReadSensors_onEntry,       NULL });
    FSM_AddState(S_INIT_DASHBOARD,     &(state_funcs_t){ S_Init_Dashboard_onEntry,    NULL });
    FSM_AddState(S_DASHBOARD,          &(state_funcs_t){ S_Dashboard_onEntry,         NULL });
    FSM_AddState(S_READ_SENSORS,       &(state_funcs_t){ S_ReadSensors_onEntry,       NULL });
    FSM_AddState(S_STOP,               &(state_funcs_t){ S_Stop_onEntry,              NULL });
	FSM_AddState(S_UPDATE_ENVIRONMENT, &(state_funcs_t){ S_UpdateEnvironment_onEntry, NULL });
    FSM_AddState(S_SWITCH_UNITS,       &(state_funcs_t){ S_SwitchUnits_onEntry, 	  NULL });

    // --------------------
    // Register transitions
    // --------------------

   //                                  From                     Event                To
    FSM_AddTransition(&(transition_t){ S_START,              E_START,             S_INITIALISE_SYSTEM });
    FSM_AddTransition(&(transition_t){ S_INITIALISE_SYSTEM,  E_INIT_DONE,         S_INIT_DASHBOARD });
    FSM_AddTransition(&(transition_t){ S_INIT_DASHBOARD,     E_DASHBOARD_INIT,    S_READ_SENSORS });
    FSM_AddTransition(&(transition_t){ S_READ_SENSORS,       E_SENSOR_READ,       S_DASHBOARD });
    FSM_AddTransition(&(transition_t){ S_SWITCH_UNITS,       E_SWITCH_UNITS,      S_DASHBOARD });
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_SWITCH_UNITS,      S_SWITCH_UNITS });
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_SHOW_HISTORY,      S_HISTORY });
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_SENSOR_READ,       S_READ_SENSORS });
    FSM_AddTransition(&(transition_t){ S_DASHBOARD,          E_DASHBOARD_EXIT,    S_STOP });



    FSM_FlushEnexpectedEvents(false);

    // --------------------
    // Start FSM
    // --------------------
    FSM_RunStateMachine(S_START, E_START);

    return 0;
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

void S_ReadSensors_onEntry(void)
{
    DSPclear();

        // Generate realistic fake sensor data only once per read
    g_temperature = rand_range(-5.0f, 35.0f);  // Celsius
    g_humidity    = rand_range(20.0f, 90.0f);  // Percent

    float temp = g_unit_celsius
        ? g_temperature
        : (g_temperature * 9.0f / 5.0f + 32.0f);
    const char* unit = g_unit_celsius ? "C" : "F";

    const char* icon = "[SUN]";
    if (g_humidity < 40) icon = "[SUN]";
    else if (g_humidity < 75) icon = "[CLOUD]";
    else icon = "[RAIN]";

    DSPshow(1, "Temp: %.1f %s", temp, unit);
    DSPshow(2, "Humidity: %.0f %%", g_humidity);
    DSPshow(3, "Weather: %s", icon);

    // Simulated sensor init OK
    FSM_AddEvent(E_SENSOR_READ);
}


void S_Init_Dashboard_onEntry(void)
{
    DSPshow(0, "=== Weather Dashboard ===");


    DSPshow(5, "r - Reread sensors");
    DSPshow(6, "u - switch units");
    DSPshow(7, "h - history");
    DSPshow(8, "q - quit");
    DSPshow(30, "");
    
    FSM_AddEvent(E_DASHBOARD_INIT);
}

void S_Dashboard_onEntry(void)
{
    
    float temp = g_unit_celsius
        ? g_temperature
        : (g_temperature * 9.0f / 5.0f + 32.0f);
    const char* unit = g_unit_celsius ? "C" : "F";

    const char* icon = "[SUN]";
    if (g_humidity < 40) icon = "[SUN]";
    else if (g_humidity < 75) icon = "[CLOUD]";
    else icon = "[RAIN]";


    DSPshow(1, "Temp: %.1f %s", temp, unit);
    DSPshow(2, "Humidity: %.0f %%", g_humidity);
    DSPshow(3, "Weather: %s", icon);

    while (1)
    {
       
        // Non-blocking keyboard on Windows
        char c = get_key_nonblocking();
        if (c) {
            switch (tolower(c)) {
                case 'r': FSM_AddEvent(E_SENSOR_READ); return;
                case 'u': FSM_AddEvent(E_SWITCH_UNITS); return;
                case 'h': FSM_AddEvent(E_SHOW_HISTORY); return;
                case 'q': FSM_AddEvent(E_DASHBOARD_EXIT); return;
                default: break;
            }
        }

        delay_ms(500);
    }
}




void S_UpdateEnvironment_onEntry(void)
{
    // TODO: Use this to update Temp/Humidity
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

void S_SwitchUnits_onEntry(void)
{
    g_unit_celsius = !g_unit_celsius;
    
    FSM_AddEvent(E_SWITCH_UNITS);
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