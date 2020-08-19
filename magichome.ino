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

    void update_device(MagicHomeAPI device, int r = 0, int g = 0, int b = 0, int white1=NULL, int white2=NULL){
        /*Updates a device based upon what we're sending to it.
        Values are excepted as integers between 0-255.
        Whites can have a value of None.*/
        if(device.type <= 1){
            white1 = device.check_number_range(white1);
            int message[] = {0x31, r, g, b, white1, 0x00, 0x0f};
            device.send_bytes(*(message+[device.calculate_checksum(message)]));
        }else if(device.type == 2){
            //Update an RGB + WW + CW device
            int message[] = {0x31,
                       device.check_number_range(r),
                       device.check_number_range(g),
                       device.check_number_range(b),
                       device.check_number_range(white1),
                       device.check_number_range(white2),
                       0x0f, 0x0f};
            device.send_bytes(*(message+[device.calculate_checksum(message)]));
        } else if (device.type == 3){
            //Update the white, or color, of a bulb
            if (white1 != NULL){
                int message[] = {0x31, 0x00, 0x00, 0x00,
                           device.check_number_range(white1),
                           0x0f, 0x0f};
                device.send_bytes(*(message+[device.calculate_checksum(message)]))
            } else{
                int message[] = {0x31,
                           device.check_number_range(r),
                           device.check_number_range(g),
                           device.check_number_range(b),
                           0x00, 0xf0, 0x0f};
                device.send_bytes(*(message+[device.calculate_checksum(message)]));
            }
        } else if (device.type == 4){
             //Update the white, or color, of a legacy bulb
           if (white1 != NULL){
                int message[] = {0x56, 0x00, 0x00, 0x00,
                           device.check_number_range(white1),
                           0x0f, 0xaa, 0x56, 0x00, 0x00, 0x00,
                           device.check_number_range(white1),
                           0x0f, 0xaa};
                device.send_bytes(*(message+[device.calculate_checksum(message)]));
           } else{
                int message[] = {0x56,
                           device.check_number_range(r),
                           device.check_number_range(g),
                           device.check_number_range(b),
                           0x00, 0xf0, 0xaa};
                device.send_bytes(*(message+[device.calculate_checksum(message)]));
                }
        }else{
            Serial.println("Incompatible device type received...");
        }
    }

    //Check if the given number is in the allowed range.
    int check_number_range(MagicHomeAPI device, int number){
        if (number < 0){
            return 0;
        }else if (number > 255){
            return 255;
        }else{
            return number;
        }
    }

    //Send a preset command to a device.   
    void send_preset_function(MagicHomeAPI device, int preset_number, int speed){
        //Presets can range from 0x25 (int 37) to 0x38 (int 56)
        if(preset_number < 37){
            preset_number = 37;
        }
        else if(preset_number > 56){
            preset_number = 56;
        }

        if (speed < 0){
            speed = 0;
        }else if (speed > 100){
            speed = 100;
        }

        if(device.type == 4){
            device.send_bytes(0xBB, preset_number, speed, 0x44);
        }else{
            int message[] = {0x61, preset_number, speed, 0x0F};
            device.send_bytes(*(message+[device.calculate_checksum(message)]));
        }
    }

    int calculate_checksum(MagicHomeAPI device, int* bytes){
        //Calculate the checksum from an array of bytes.
        int sum = 0;
        for(int i = 0; i < sizeof(bytes); i++){
            sum += bytes[i];
        }
        return sum & 0xFF;
    }

    void send_bytes(MagicHomeAPI device, int* bytes){
         /*Send commands to the device.

        If the device hasn't been communicated to in 5 minutes, reestablish the
        connection.
        */

       //check connection time

       try{
           if(check_connection_time >= 290){
               Serial.println("Connection timed out, restablishing.");
               //connect to device
           }
        auto message_length = sizeof(bytes);
       }catch(exception e){
           Serial.println("");
       }
    }
};