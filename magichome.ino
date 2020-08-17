#include <SocketIOclient.h>
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

/*MagicHome C++ API.

Copyright 2016, Adam Kempenich. Licensed under MIT.

It currently supports:
- Bulbs (Firmware v.4 and greater)
- Legacy Bulbs (Firmware v.3 and lower)
- RGB Controllers
- RGB+WW Controllers
- RGB+WW+CW Controllers
*/
#include <string>
#include <ctime>
#include <iostream>

using namespace std;

class MagicHomeAPI{
    private:
    const int API_PORT = 5577;
    string device_ip;
    int device_type;
    bool keep_alive = true;
    time_t latest_connection = time(0);
    WebSocketsClient webSocket;

    public:
    MagicHomeAPI(string device_ip, int device_type){
        //Initialize device
        this->device_ip = device_ip;
        this->device_type = device_type;
        
    }
};