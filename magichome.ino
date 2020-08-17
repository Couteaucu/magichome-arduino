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
    string ip;
    int type;
    bool keep_alive = true;
    time_t latest_connection = time(0);
    WebSocketsClient socket;//implement socket

    public:
    MagicHomeAPI(string ip, int type){
        //Initialize device
        this->ip = ip;
        this->type = type;
        
    }

    void turn_on(MagicHomeAPI device){
        if(device.type < 4){
            device.send_bytes(0x71, 0x23, 0x0F, 0xA3);
        }else{
            device.send_bytes(0xCC, 0x23, 0x33);
        }
    }

    void turn_off(MagicHomeAPI device){
        if(device.type < 4){
            device.send_bytes(0x71, 0x24, 0x0F, 0xA4);
        }else{
            device.send_bytes(0xCC, 0x24, 0x33);
        }
    }
    //change return type
    void get_status(MagicHomeAPI device){
        if(device.type == 2){
            device.send_bytes(0x81, 0x8A, 0x8B, 0x96);
            return device.socket.recv(15); //will need to figure out what this is getting
        }else{
            device.send_bytes(0x81, 0x8A, 0x8B, 0x96);
            return device.socket.recv(14);
        }
    }
};