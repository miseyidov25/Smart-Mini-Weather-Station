# Smart Mini Weather Station Simulation

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c11
CONFIG += warn_on

# --------------------
# Source files
# --------------------
SOURCES += \
    app/src/main.c \
    app/src/fsm_functions/fsm.c \
    app/src/console_functions/display.c \
    app/src/console_functions/devConsole.c \
    app/src/console_functions/keyboard.c \
    app/src/console_functions/systemErrors.c \
    app/src/states.c \
    app/src/events.c

# --------------------
# Header files
# --------------------
HEADERS += \
    app/include/states.h \
    app/include/events.h \
    app/src/fsm_functions/fsm.h \
    app/src/console_functions/display.h \
    app/src/console_functions/devConsole.h \
    app/src/console_functions/keyboard.h \
    app/src/console_functions/systemErrors.h

# --------------------
# Compiler settings
# --------------------
QMAKE_CFLAGS += -Wall -g
QMAKE_CFLAGS += -std=c11

# --------------------
# Output
# --------------------
TARGET = weather_station

