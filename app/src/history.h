#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 24

typedef struct {
    float temperature_c;
    float humidity;
} HistoryEntry;

void history_add(float temp_c, float humidity);
int  history_count(void);
HistoryEntry history_get(int index);

#endif
