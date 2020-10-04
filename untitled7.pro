TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -lWs2_32

SOURCES += \
        BackgroundFunctional..cpp \
        Client.cpp \
        CmdFuncs.cpp \
        CommandsTable.cpp \
        Executor.cpp \
        NetCommandsQueue.cpp \
        Server.cpp \
        Socket.cpp \
        main.cpp

HEADERS += \
    BackgroundFunctional.h \
    Client.h \
    CmdFuncs.h \
    CommandsTable.h \
    Executor.h \
    ICommand.h \
    NetCommandsQueue.h \
    Server.h \
    Socket.h
