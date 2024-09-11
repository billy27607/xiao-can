/*
    Arduino library for communicating with an AlorAirHD65i dehumidifier via the CAN bus
    created by Billy Willis
*/

#ifndef AlorAirHDi65_h
#define AlorAirHDi65_h

#define SPI_CS_PIN D7 // this is for the xiao can expansion

class AlorAirHDi65
{
    public:
        void begin();
        bool status();
        void print_status();
        bool set_power(bool state);
        bool set_target_humidity(int humidity);
        bool pumpout();

    private:
        bool send_get();
        // mcp2515_can;
};

// the #include statement and code go here...

#endif