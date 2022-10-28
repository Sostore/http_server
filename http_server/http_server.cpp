#define WIN32_LEAN_AND_MEAN

#include <exception>
#include <typeinfo>
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>

#include "ServerListener.h"

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        std::cout << "Goodbye" << std::endl;
        WSACleanup();
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        return FALSE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        return FALSE;

    default:
        return FALSE;
    }
}

int main(void)
{
    
    try
    {
        SetConsoleCtrlHandler(CtrlHandler, TRUE);
        ServerListener().run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Caught: " << e.what() << std::endl;
        std::cerr << "Type: " << typeid(e).name() << std::endl;
    }
}