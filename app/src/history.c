#include "history.h"

static HistoryEntry g_history[HISTORY_SIZE];
static int g_history_index = 0;
static int g_history_count = 0;

void history_add(float temp_c, float humidity)
{
    g_history[g_history_index].temperature_c = temp_c;
    g_history[g_history_index].humidity = humidity;

    g_history_index = (g_history_index + 1) % HISTORY_SIZE;

    if (g_history_count < HISTORY_SIZE)
        g_history_count++;
}

int history_count(void)
{
    return g_history_count;
}

HistoryEntry history_get(int index)
{
    return g_history[index];
}
