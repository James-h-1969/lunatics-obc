#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <vector>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "I2C.h"

#define MAX_RECORDINGS 3000

class AS7265X {
private:
    // Constants
    static constexpr uint8_t I2C_ADDR = 0x49;
    
    // Status and communication registers
    static constexpr uint8_t STATUS_REG = 0x00;
    static constexpr uint8_t WRITE_REG = 0x01;
    static constexpr uint8_t READ_REG = 0x02;
    static constexpr uint8_t TX_VALID = 0x02;
    static constexpr uint8_t RX_VALID = 0x01;
    
    // Register addresses
    static constexpr uint8_t HW_VERSION_HIGH = 0x00;
    static constexpr uint8_t HW_VERSION_LOW = 0x01;
    static constexpr uint8_t FW_VERSION_HIGH = 0x02;
    static constexpr uint8_t FW_VERSION_LOW = 0x03;
    
    static constexpr uint8_t CONFIG = 0x04;
    static constexpr uint8_t INTEGRATION_TIME = 0x05;
    static constexpr uint8_t DEVICE_TEMP = 0x06;
    static constexpr uint8_t LED_CONFIG = 0x07;
    
    // Raw channel registers
    static constexpr uint8_t R_G_A = 0x08;
    static constexpr uint8_t S_H_B = 0x0a;
    static constexpr uint8_t T_I_C = 0x0c;
    static constexpr uint8_t U_J_D = 0x0e;
    static constexpr uint8_t V_K_E = 0x10;
    static constexpr uint8_t W_L_F = 0x12;
    
    // Calibrated channel registers
    static constexpr uint8_t R_G_A_CAL = 0x14;
    static constexpr uint8_t S_H_B_CAL = 0x18;
    static constexpr uint8_t T_I_C_CAL = 0x1c;
    static constexpr uint8_t U_J_D_CAL = 0x20;
    static constexpr uint8_t V_K_E_CAL = 0x24;
    static constexpr uint8_t W_L_F_CAL = 0x28;
    
    static constexpr uint8_t DEV_SELECT_CONTROL = 0x4F;
    
    static constexpr uint8_t COEF_DATA_0 = 0x50;
    static constexpr uint8_t COEF_DATA_1 = 0x51;
    static constexpr uint8_t COEF_DATA_2 = 0x52;
    static constexpr uint8_t COEF_DATA_3 = 0x52;
    static constexpr uint8_t COEF_DATA_READ = 0x54;
    static constexpr uint8_t COEF_DATA_WRITE = 0x55;
    
    // Settings
    static constexpr int POLLING_DELAY_MS = 10;
    
    RaspberryPiI2C* bus;
    
public:
    // Device types
    enum Device : uint8_t {
        NIR = 0x00,
        VISIBLE = 0x01,
        UV = 0x02
    };
    
    // LED types
    enum LEDType : uint8_t {
        LED_WHITE = 0x00,
        LED_IR = 0x01,
        LED_UV = 0x02
    };
    
    // LED current limits
    enum LEDCurrent : uint8_t {
        LED_CURRENT_LIMIT_12_5MA = 0b00,
        LED_CURRENT_LIMIT_25MA = 0b01,
        LED_CURRENT_LIMIT_50MA = 0b10,
        LED_CURRENT_LIMIT_100MA = 0b11
    };
    
    // Indicator current limits
    enum IndicatorCurrent : uint8_t {
        INDICATOR_CURRENT_LIMIT_1MA = 0b00,
        INDICATOR_CURRENT_LIMIT_2MA = 0b01,
        INDICATOR_CURRENT_LIMIT_4MA = 0b10,
        INDICATOR_CURRENT_LIMIT_8MA = 0b11
    };
    
    // Gain settings
    enum Gain : uint8_t {
        GAIN_1X = 0b00,
        GAIN_37X = 0b01,
        GAIN_16X = 0b10,
        GAIN_64X = 0b11
    };
    
    // Measurement modes
    enum MeasurementMode : uint8_t {
        MEASUREMENT_MODE_4CHAN = 0b00,
        MEASUREMENT_MODE_4CHAN_2 = 0b01,
        MEASUREMENT_MODE_6CHAN_CONTINUOUS = 0b10,
        MEASUREMENT_MODE_6CHAN_ONE_SHOT = 0b11
    };
    
    // Constructor
    explicit AS7265X(RaspberryPiI2C* i2cBus) : bus(i2cBus) {}
    
    // Initialization
    bool begin() {
        if (!isConnected()) {
            return false;
        }
        
        uint8_t value = virtualReadRegister(DEV_SELECT_CONTROL);
        if ((value & 0b00110000) == 0) {
            return false;
        }
        
        setBulbCurrent(LED_CURRENT_LIMIT_12_5MA, LED_WHITE);
        setBulbCurrent(LED_CURRENT_LIMIT_12_5MA, LED_IR);
        setBulbCurrent(LED_CURRENT_LIMIT_12_5MA, LED_UV);
        
        disableBulb(LED_WHITE);
        disableBulb(LED_IR);
        disableBulb(LED_UV);
        
        // setIndicatorCurrent(INDICATOR_CURRENT_LIMIT_8MA);
        disableIndicator();
        
        setIntegrationCycles(49); // 50 * 2.8ms = 140ms
        setGain(GAIN_64X);
        setMeasurementMode(MEASUREMENT_MODE_6CHAN_ONE_SHOT);
        enableInterrupt();
        
        return true;
    }
    
    // Device information
    uint8_t getDeviceType() {
        return virtualReadRegister(HW_VERSION_HIGH);
    }
    
    uint8_t getHardwareVersion() {
        return virtualReadRegister(HW_VERSION_LOW);
    }
    
    uint8_t getMajorFirmwareVersion() {
        virtualWriteRegister(FW_VERSION_HIGH, 0x01);
        virtualWriteRegister(FW_VERSION_LOW, 0x01);
        return virtualReadRegister(FW_VERSION_LOW);
    }
    
    uint8_t getPatchFirmwareVersion() {
        virtualWriteRegister(FW_VERSION_HIGH, 0x02);
        virtualWriteRegister(FW_VERSION_LOW, 0x02);
        return virtualReadRegister(FW_VERSION_LOW);
    }
    
    uint8_t getBuildFirmwareVersion() {
        virtualWriteRegister(FW_VERSION_HIGH, 0x03);
        virtualWriteRegister(FW_VERSION_LOW, 0x03);
        return virtualReadRegister(FW_VERSION_LOW);
    }
    
    bool isConnected() {
        try {
            bus->readByte(I2C_ADDR, STATUS_REG);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Measurement functions
    void takeMeasurements() {
        setMeasurementMode(MEASUREMENT_MODE_6CHAN_ONE_SHOT);
        
        while (!dataAvailable()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_DELAY_MS));
        }
    }
    
    void takeMeasurementsWithBulb() {
        enableBulb(LED_WHITE);
        enableBulb(LED_IR);
        enableBulb(LED_UV);
        
        takeMeasurements();
        
        disableBulb(LED_WHITE);
        disableBulb(LED_IR);
        disableBulb(LED_UV);
    }
    
    // Visible light readings (G, H, I, J, K, L)
    uint16_t getG() { return getChannel(R_G_A, VISIBLE); }
    uint16_t getH() { return getChannel(S_H_B, VISIBLE); }
    uint16_t getI() { return getChannel(T_I_C, VISIBLE); }
    uint16_t getJ() { return getChannel(U_J_D, VISIBLE); }
    uint16_t getK() { return getChannel(V_K_E, VISIBLE); }
    uint16_t getL() { return getChannel(W_L_F, VISIBLE); }
    
    // NIR readings (R, S, T, U, V, W)
    uint16_t getR() { return getChannel(R_G_A, NIR); }
    uint16_t getS() { return getChannel(S_H_B, NIR); }
    uint16_t getT() { return getChannel(T_I_C, NIR); }
    uint16_t getU() { return getChannel(U_J_D, NIR); }
    uint16_t getV() { return getChannel(V_K_E, NIR); }
    uint16_t getW() { return getChannel(W_L_F, NIR); }
    
    // UV readings (A, B, C, D, E, F)
    uint16_t getA() { return getChannel(R_G_A, UV); }
    uint16_t getB() { return getChannel(S_H_B, UV); }
    uint16_t getC() { return getChannel(T_I_C, UV); }
    uint16_t getD() { return getChannel(U_J_D, UV); }
    uint16_t getE() { return getChannel(V_K_E, UV); }
    uint16_t getF() { return getChannel(W_L_F, UV); }
    
    // Calibrated UV readings
    float getCalibratedA() { return getCalibratedValue(R_G_A_CAL, UV); }
    float getCalibratedB() { return getCalibratedValue(S_H_B_CAL, UV); }
    float getCalibratedC() { return getCalibratedValue(T_I_C_CAL, UV); }
    float getCalibratedD() { return getCalibratedValue(U_J_D_CAL, UV); }
    float getCalibratedE() { return getCalibratedValue(V_K_E_CAL, UV); }
    float getCalibratedF() { return getCalibratedValue(W_L_F_CAL, UV); }
    
    // Calibrated visible readings
    float getCalibratedG() { return getCalibratedValue(R_G_A_CAL, VISIBLE); }
    float getCalibratedH() { return getCalibratedValue(S_H_B_CAL, VISIBLE); }
    float getCalibratedI() { return getCalibratedValue(T_I_C_CAL, VISIBLE); }
    float getCalibratedJ() { return getCalibratedValue(U_J_D_CAL, VISIBLE); }
    float getCalibratedK() { return getCalibratedValue(V_K_E_CAL, VISIBLE); }
    float getCalibratedL() { return getCalibratedValue(W_L_F_CAL, VISIBLE); }
    
    // Calibrated NIR readings
    float getCalibratedR() { return getCalibratedValue(R_G_A_CAL, NIR); }
    float getCalibratedS() { return getCalibratedValue(S_H_B_CAL, NIR); }
    float getCalibratedT() { return getCalibratedValue(T_I_C_CAL, NIR); }
    float getCalibratedU() { return getCalibratedValue(U_J_D_CAL, NIR); }
    float getCalibratedV() { return getCalibratedValue(V_K_E_CAL, NIR); }
    float getCalibratedW() { return getCalibratedValue(W_L_F_CAL, NIR); }
    
    // Configuration functions
    void setMeasurementMode(MeasurementMode mode) {
        uint8_t modeValue = static_cast<uint8_t>(mode);
        if (modeValue > 0b11) modeValue = 0b11;
        
        uint8_t value = virtualReadRegister(CONFIG);
        value &= 0b11110011;
        value |= (modeValue << 2);
        virtualWriteRegister(CONFIG, value);
    }
    
    void setGain(Gain gain) {
        uint8_t gainValue = static_cast<uint8_t>(gain);
        if (gainValue > 0b11) gainValue = 0b11;
        
        uint8_t value = virtualReadRegister(CONFIG);
        value &= 0b11001111;
        value |= (gainValue << 4);
        virtualWriteRegister(CONFIG, value);
    }
    
    void setIntegrationCycles(uint8_t cycleValue) {
        virtualWriteRegister(INTEGRATION_TIME, cycleValue);
    }
    
    void enableInterrupt() {
        uint8_t value = virtualReadRegister(CONFIG);
        value |= 0b01000000;
        virtualWriteRegister(CONFIG, value);
    }
    
    void disableInterrupt() {
        uint8_t value = virtualReadRegister(CONFIG);
        value &= 0b10111111;
        virtualWriteRegister(CONFIG, value);
    }
    
    bool dataAvailable() {
        uint8_t value = virtualReadRegister(CONFIG);
        return value & 0x02;
    }
    
    // LED control
    void enableBulb(LEDType device) {
        selectDevice(static_cast<Device>(device));
        uint8_t value = virtualReadRegister(LED_CONFIG);
        value |= 0b00001000;
        virtualWriteRegister(LED_CONFIG, value);
    }
    
    void disableBulb(LEDType device) {
        selectDevice(static_cast<Device>(device));
        uint8_t value = virtualReadRegister(LED_CONFIG);
        value &= 0b11110111;
        virtualWriteRegister(LED_CONFIG, value);
    }
    
    void setBulbCurrent(LEDCurrent current, LEDType device) {
        selectDevice(static_cast<Device>(device));
        uint8_t currentValue = static_cast<uint8_t>(current);
        if (currentValue > 0b11) currentValue = 0b11;
        
        uint8_t value = virtualReadRegister(LED_CONFIG);
        value &= 0b11001111;
        value |= (currentValue << 4);
        virtualWriteRegister(LED_CONFIG, value);
    }
    
    void enableIndicator() {
        selectDevice(NIR);
        uint8_t value = virtualReadRegister(LED_CONFIG);
        value |= 0b00000001;
        virtualWriteRegister(LED_CONFIG, value);
    }
    
    void disableIndicator() {
        selectDevice(NIR);
        uint8_t value = virtualReadRegister(LED_CONFIG);
        value &= 0b11111110;
        virtualWriteRegister(LED_CONFIG, value);
    }
    
    void setIndicatorCurrent(IndicatorCurrent current) {
        uint8_t currentValue = static_cast<uint8_t>(current);
        if (currentValue > 0b11) currentValue = 0b11;
        
        uint8_t value = virtualReadRegister(LED_CONFIG);
        value &= 0b11111001;
        value |= (currentValue << 1);
        
        selectDevice(NIR);
        virtualWriteRegister(LED_CONFIG, value);
    }
    
    // Temperature functions
    uint8_t getTemperature(uint8_t deviceNumber) {
        selectDevice(static_cast<Device>(deviceNumber));
        return virtualReadRegister(DEVICE_TEMP);
    }
    
    float getTemperatureAverage() {
        float average = 0;
        for (int x = 0; x < 3; x++) {
            average += getTemperature(x);
        }
        return average / 3.0f;
    }
    
    void softReset() {
        uint8_t value = virtualReadRegister(CONFIG);
        value |= 0x80;
        virtualWriteRegister(CONFIG, value);
    }

private:
    uint16_t getChannel(uint8_t channelRegister, Device device) {
        selectDevice(device);
        uint16_t colorData = virtualReadRegister(channelRegister) << 8;
        colorData |= virtualReadRegister(channelRegister + 1);
        return colorData;
    }
    
    float getCalibratedValue(uint8_t calAddress, Device device) {
        selectDevice(device);
        
        uint8_t b0 = virtualReadRegister(calAddress + 0);
        uint8_t b1 = virtualReadRegister(calAddress + 1);
        uint8_t b2 = virtualReadRegister(calAddress + 2);
        uint8_t b3 = virtualReadRegister(calAddress + 3);
        
        // Channel calibrated values are stored big-endian
        uint32_t calBytes = 0;
        calBytes |= (static_cast<uint32_t>(b0) << 24);
        calBytes |= (static_cast<uint32_t>(b1) << 16);
        calBytes |= (static_cast<uint32_t>(b2) << 8);
        calBytes |= (static_cast<uint32_t>(b3) << 0);
        
        return convertBytesToFloat(calBytes);
    }
    
    float convertBytesToFloat(uint32_t value) {
        union {
            uint32_t i;
            float f;
        } converter;
        converter.i = value;
        return converter.f;
    }
    
    void selectDevice(Device device) {
        virtualWriteRegister(DEV_SELECT_CONTROL, static_cast<uint8_t>(device));
    }
    
    uint8_t virtualReadRegister(uint8_t virtualAddr) {
        const auto timeout = std::chrono::seconds(5);
        auto startTime = std::chrono::steady_clock::now();
        
        uint8_t status = readRegister(STATUS_REG);
        if ((status & RX_VALID) != 0) {
            uint8_t incoming = readRegister(READ_REG);
        }
        
        while (true) {
            status = readRegister(STATUS_REG);
            if ((status & TX_VALID) == 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_DELAY_MS));
            if (std::chrono::steady_clock::now() - startTime > timeout) {
                throw std::runtime_error("virtualReadRegister timeout");
            }
        }
        
        writeRegister(WRITE_REG, virtualAddr);
        
        while (true) {
            status = readRegister(STATUS_REG);
            if ((status & RX_VALID) != 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_DELAY_MS));
            if (std::chrono::steady_clock::now() - startTime > timeout) {
                throw std::runtime_error("virtualReadRegister timeout");
            }
        }
        
        uint8_t incoming = readRegister(READ_REG);
        return incoming;
    }
    
    void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite) {
        const auto timeout = std::chrono::seconds(5);
        auto startTime = std::chrono::steady_clock::now();
        
        while (true) {
            uint8_t status = readRegister(STATUS_REG);
            if ((status & TX_VALID) == 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_DELAY_MS));
            if (std::chrono::steady_clock::now() - startTime > timeout) {
                throw std::runtime_error("virtualWriteRegister timeout");
            }
        }
        
        writeRegister(WRITE_REG, virtualAddr | 0x80);
        
        while (true) {
            uint8_t status = readRegister(STATUS_REG);
            if ((status & TX_VALID) == 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(POLLING_DELAY_MS));
            if (std::chrono::steady_clock::now() - startTime > timeout) {
                throw std::runtime_error("virtualWriteRegister timeout");
            }
        }
        
        writeRegister(WRITE_REG, dataToWrite);
    }
    
    uint8_t readRegister(uint8_t addr) {
        return bus->readByte(I2C_ADDR, addr);
    }
    
    void writeRegister(uint8_t addr, uint8_t val) {
        bus->writeByte(I2C_ADDR, addr, val);
    }
};

struct PayloadReading {
    std::vector<float> readings;
    std::time_t reading_time;
};

class Payload {
    /*
    This class is used to abstract taking measurements from the payload.
    */
    public:
        Payload(const AS7265X& sensor) : sensor_(sensor) {}
        void take_reading(std::time_t current_time);
        void remove_before_time(std::time_t time_input);
        std::vector<struct PayloadReading> get_payload_storage();
        void delete_all_payload_readings();

    private:
        std::vector<struct PayloadReading> payload_storage_;
    
    AS7265X sensor_;

};


#endif