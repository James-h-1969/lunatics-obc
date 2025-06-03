#ifndef I2C_H
#define I2C_H

#include <cstdint>
#include <chrono>
#include <thread>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <stdexcept>


class RaspberryPiI2C {
private:
    int i2c_fd;
    uint8_t current_address;
    bool address_set;

public:
    explicit RaspberryPiI2C(int bus_number = 1) : current_address(0), address_set(false) {
        std::string device_path = "/dev/i2c-" + std::to_string(bus_number);
        
        i2c_fd = open(device_path.c_str(), O_RDWR);
        if (i2c_fd < 0) {
            throw std::runtime_error("Failed to open I2C device: " + device_path + 
                                   " - Error: " + std::string(strerror(errno)));
        }
    }

    ~RaspberryPiI2C() {
        if (i2c_fd >= 0) {
            close(i2c_fd);
        }
    }
    
    // make a singleton
    RaspberryPiI2C(const RaspberryPiI2C&) = delete;
    RaspberryPiI2C& operator=(const RaspberryPiI2C&) = delete;

    uint8_t readByte(uint8_t address, uint8_t reg) {
        if (!address_set || current_address != address) {
            if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
                throw std::runtime_error("Failed to set I2C slave address 0x" + 
                                       std::to_string(address) + " - Error: " + 
                                       std::string(strerror(errno)));
            }
            current_address = address;
            address_set = true;
        }

        if (write(i2c_fd, &reg, 1) != 1) {
            throw std::runtime_error("Failed to write register address 0x" + 
                                   std::to_string(reg) + " - Error: " + 
                                   std::string(strerror(errno)));
        }

        uint8_t data;
        if (read(i2c_fd, &data, 1) != 1) {
            throw std::runtime_error("Failed to read from register 0x" + 
                                   std::to_string(reg) + " - Error: " + 
                                   std::string(strerror(errno)));
        }

        return data;
    }

    void writeByte(uint8_t address, uint8_t reg, uint8_t value) {
        if (!address_set || current_address != address) {
            if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
                throw std::runtime_error("Failed to set I2C slave address 0x" + 
                                       std::to_string(address) + " - Error: " + 
                                       std::string(strerror(errno)));
            }
            current_address = address;
            address_set = true;
        }

        uint8_t buffer[2] = {reg, value};
        
        if (write(i2c_fd, buffer, 2) != 2) {
            throw std::runtime_error("Failed to write to register 0x" + 
                                   std::to_string(reg) + " - Error: " + 
                                   std::string(strerror(errno)));
        }
    }

    
    // Check if the I2C device is responsive
    bool isDeviceConnected(uint8_t address) {
        try {
            // Try to set the slave address and perform a quick read
            if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
                return false;
            }
            
            // Attempt a quick write/read test
            uint8_t dummy = 0;
            if (write(i2c_fd, &dummy, 0) < 0) {
                return false;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }

    // Get the current I2C file descriptor (for advanced usage)
    int getFileDescriptor() const {
        return i2c_fd;
    }
};


# endif