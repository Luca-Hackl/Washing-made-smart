# Washing-Machine

## Table of contents
* [General info](#general-info)
* [Setup](#setup)

## General info
This project utilizes an RC522 RFID Kit along with an ESP32 and a SW420 vibration sensor to measure whether or not a washing machine is running. This program also sends a discord message when the washing machine has finished according to the person that triggered the program (identified via the chip). 

## Setup
Wire the components just like in the following picture.
For the RC522:
RST - 15
MISO - 19
MOSI - 23
SCK - 18
SDA - 5

For the SW420:
DO - 21
![image](https://user-images.githubusercontent.com/63552660/157085542-66ebd799-fb30-46ee-99e4-afe52e3ad241.png)
