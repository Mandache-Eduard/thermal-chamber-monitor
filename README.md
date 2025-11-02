# Thermal Chamber Monitor

An Arduino Nano–based temperature monitoring system for laboratory stress tests in controlled thermal environments.

## Features
- Dual K-type thermocouples (MAX6675 modules)
- Real-time temperature display on 16x2 I2C LCD
- Adjustable target temperatures via buttons
- LED indicators for below/above threshold
- Audible buzzer alert when targets are reached
- Hold-acceleration for faster setpoint adjustment

## Purpose
Designed to ensure accurate measurement and confirmation that test materials or components reach the same temperature as the surrounding air in a thermal chamber. Enables better data validation during long-duration heat stress tests.

## Hardware
- Arduino Nano
- 2 × MAX6675 + K-type thermocouples
- 16x2 I²C LCD module
- 4 × push buttons
- 4 × LEDs + 220 Ω resistors
- 1 × active buzzer
