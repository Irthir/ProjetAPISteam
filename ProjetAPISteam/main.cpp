#include <iostream>
#include "SteamAPIManager.h"

int main()
{
    SteamAPIManager pSteamManager;

    pSteamManager.SteamAPIInit();

    std::cout << "Hello World!\n";

    while (true)
    {
        pSteamManager.SteamUpdate();
    }

    pSteamManager.SteamAPIQuit();


    return 0;
}