# FlightController
PID controller for a quadcopter.

The software is responsible for controlling the positioning of the quadcopter depending on the IMU and receiver values. The IMU
and receiver data is fed into a STM32F446RE microcontroller where a PID control loop is used to determine the optimal motor configuration
to operate the quadcopter.

Required PID Loop Time: 4ms
PID update time       : TBD
Software memory size  : TBD
