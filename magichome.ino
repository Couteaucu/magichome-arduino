#include <WiFi.h>
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
#include <list>

using namespace std;

class MagicHomeAPI
{
private:
    const uint16_t API_PORT = 5577;
    IPAddress ip;
    int type;
    bool keep_alive = true;
    time_t latest_connection = time(0);
    WiFiUDP udp; //implement socket

public:
    MagicHomeAPI(IPAddress ip, int type) {
        //Initialize device
        this->ip = ip;
        this->type = type;
    }

    void turn_on(){
        if (type < 4){
            int bytes[4] = {0x71, 0x23, 0x0F, 0xA3};
            send_bytes(bytes);
        }
        else{
            int bytes[3] = {0xCC, 0x23, 0x33};
            send_bytes(bytes);
        }
    }

    void turn_off(){
        if (type < 4){
            int bytes[4] = {0x71, 0x24, 0x0F, 0xA4};
            send_bytes(bytes);
        }
        else{
            int bytes[4] = {0xCC, 0x24, 0x33};
            send_bytes(bytes);
        }
    }
    //change return type
    int get_status(){
        if (type == 2){
            int bytes[4] = {0x81, 0x8A, 0x8B, 0x96};
            send_bytes(bytes);

            udp.parsePacket();

            const int BUFFERSIZE = 15;
            uint8_t buffer[BUFFERSIZE];
            memset(buffer, 0, BUFFERSIZE);
            uint8_t recieve_byte  = udp.read(buffer,BUFFERSIZE);
            if(recieve_byte > 0){
                return recieve_byte; //number is buffer size, The return value is a bytes object representing the data received.
            }
        }
        else{
           int bytes[4] = {0x81, 0x8A, 0x8B, 0x96};
           send_bytes(bytes);

            udp.parsePacket();

            const int BUFFERSIZE = 14;
            uint8_t buffer[BUFFERSIZE];
            memset(buffer, 0, BUFFERSIZE);
            uint8_t recieve_byte = udp.read(buffer,BUFFERSIZE);
            if(recieve_byte > 0){
                return recieve_byte; //number is buffer size, The return value is a bytes object representing the data received.
            }
        }
    }

    void update_device(int r = 0, int g = 0, int b = 0, int white1 = NULL, int white2 = NULL){
        /*Updates a device based upon what we're sending to it.
        Values are excepted as integers between 0-255.
        Whites can have a value of None.*/
        if (type <= 1){
            white1 = check_number_range(white1);
            int message[] = {0x31, r, g, b, white1, 0x00, 0x0f};
            send_bytes(*(message + [calculate_checksum(message)]));
        }
        else if (type == 2){
            //Update an RGB + WW + CW device
            int message[] = {0x31,
                             check_number_range(r),
                             check_number_range(g),
                             check_number_range(b),
                             check_number_range(white1),
                             check_number_range(white2),
                             0x0f, 0x0f};
            send_bytes(*(message + [calculate_checksum(message)]));
        }
        else if (type == 3){
            //Update the white, or color, of a bulb
            if (white1 != NULL){
                int message[] = {0x31, 0x00, 0x00, 0x00,
                                 check_number_range(white1),
                                 0x0f, 0x0f};
                send_bytes(*(message + [calculate_checksum(message)]))
            }
            else{
                int message[] = {0x31,
                                 check_number_range(r),
                                 check_number_range(g),
                                 check_number_range(b),
                                 0x00, 0xf0, 0x0f};
                send_bytes(*(message + [calculate_checksum(message)]));
            }
        }
        else if (type == 4){
            //Update the white, or color, of a legacy bulb
            if (white1 != NULL){
                int message[] = {0x56, 0x00, 0x00, 0x00,
                                 check_number_range(white1),
                                 0x0f, 0xaa, 0x56, 0x00, 0x00, 0x00,
                                 check_number_range(white1),
                                 0x0f, 0xaa};
                send_bytes(*(message + [calculate_checksum(message)]));
            }
            else{
                int message[] = {0x56,
                                 check_number_range(r),
                                 check_number_range(g),
                                 check_number_range(b),
                                 0x00, 0xf0, 0xaa};
                send_bytes(*(message + [calculate_checksum(message)]));
            }
        }
        else{
            Serial.println("Incompatible device type received...");
        }
    }

    //Check if the given number is in the allowed range.
    int check_number_range(int number){
        if (number < 0){
            return 0;
        }
        else if (number > 255){
            return 255;
        }
        else{
            return number;
        }
    }

    //Send a preset command to a device.
    void send_preset_function(int preset_number, int speed){
        //Presets can range from 0x25 (int 37) to 0x38 (int 56)
        if (preset_number < 37){
            preset_number = 37;
        }
        else if (preset_number > 56){
            preset_number = 56;
        }

        if (speed < 0){
            speed = 0;
        }
        else if (speed > 100){
            speed = 100;
        }

        if (type == 4){
            int bytes[4] = {0xBB, preset_number, speed, 0x44};
            send_bytes(bytes);
        }
        else{ //probably not an int, list in python
            int message[] = {0x61, preset_number, speed, 0x0F};
            send_bytes(*(message + [calculate_checksum(message)]));
        }
    }

    int calculate_checksum(int* bytes){
        //Calculate the checksum from an array of bytes.
        int sum = 0;
        for (int i = 0; i < sizeof(bytes); i++){
            sum += bytes[i];
        }
        return sum & 0xFF;
    }

    void send_bytes(int* bytes) {
        //Send commands to the device.
        udp.beginPacket(ip, API_PORT);

        try {
            auto message_length = sizeof(bytes);
            auto buffer = struct.pack("B"*message_length, *bytes);//need c++ equilvient
            auto buffersize;//how  to get?
            udp.write(buffer, buffersize);
            udp.endPacket();
        }
        catch (exception e) //subject to change
        {
            Serial.println(e.what());
        }
    }
};