#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "fsm_functions/fsm.h"

extern char * eventEnumToText[];
extern char * stateEnumToText[];

state_t state;
event_t event;
state_t previousState;
event_t previousEvent;


int main(void) {
	puts("Hello, World!");
    
	return 0;
}