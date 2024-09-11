#include <Arduino.h>
#include <AlorAirHDi65.h>
#include <SPI.h>
#include "mcp2515_can.h"

mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

struct AlorairHDi65
{
    float currentHumidity;
    float targetHumidity;
    float currentTemperature;
    bool currentStatus;
    bool powerState;
    bool pumpState;

   void begin()
    {
        while (CAN_OK != CAN.begin(CAN_50KBPS))
        { // init can bus : baudrate = 50k
            Serial.println("CAN init fail, retry...");
            delay(100);
        }
        Serial.println("CAN init ok!");
    }

    boolean send_get(unsigned char *to_be_sent, unsigned char *received)
    {
        CAN.sendMsgBuf(0x123, 0, 8, to_be_sent);
        unsigned long start_time = millis();
        unsigned long time_out = 400;
        unsigned char len = 0;

        while (millis() - start_time < time_out)
        {
            if (CAN_MSGAVAIL == CAN.checkReceive())
            {                                   // check if data coming
                CAN.readMsgBuf(&len, received); // read data,  len: data length, buf: data buf
                return 1;                       // Exit the function after receiving a response
            }
        }
        return 0;
    }

    bool status()
    {
        unsigned char out_msg[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char in_msg[8];
        if (send_get(out_msg, in_msg))
        {
            char bit1 = (in_msg[5] & 0b1) != 0;
            char bit2 = (in_msg[5] & 0b10) != 0;
            pumpState = (in_msg[5] & 0b10000) != 0;

            powerState = !!(bit1 | bit2);
            currentStatus = bit1;
            targetHumidity = in_msg[1];
            currentHumidity = in_msg[0];
            currentTemperature = in_msg[3];
            return true;
        }
        else
        {
            return false;
        }
    }

    void print_status()
    {
        Serial.print("Current Humidity: ");
        Serial.println(currentHumidity);
        Serial.print("Target Humidity: ");
        Serial.println(targetHumidity);
        Serial.print("Current Temperature: ");
        Serial.println(currentTemperature);
        Serial.print("Current Status: ");
        Serial.println(currentStatus ? "On" : "Off");
        Serial.print("Power State: ");
        Serial.println(powerState ? "On" : "Off");
        Serial.print("Pump State: ");
        Serial.println(pumpState ? "On" : "Off");
        Serial.println();
    }

    bool set_power(bool state)
    {
        unsigned char out_msg[8] = {0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char in_msg[8];
        out_msg[2] = state;
        if (send_get(out_msg, in_msg))
        {
            if (status())
            {
                return true;
            }
        }
        return false;
    }

    bool set_target_humidity(int humidity)
    {
        unsigned char out_msg[8] = {0x01, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char in_msg[8];
        out_msg[2] = humidity;
        if (send_get(out_msg, in_msg))
        {
            if (status())
            {
                return true;
            }
        }
        return false;
    }

    bool pumpout()
    {
        unsigned char out_msg[8] = {0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
        unsigned char in_msg[8];
        if (send_get(out_msg, in_msg))
        {
            if (status())
            {
                return true;
            }
        }
        return false;
    }
};
