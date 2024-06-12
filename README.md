# TinyML on Arduino

Companion repo for the talk: **[Tiny ML on Arduino From the Edge to the Cloud](https://www.linkedin.com/posts/italian-embedded_embeddedsystems-machinelearning-software-activity-7181276255814651905-kb8p?utm_source=share&utm_medium=member_desktop)**

This repo contains:
- 3 sketches for:
  - Arduino Nicla Vision, for image detection on the Edge
  - Arduino Giga R1 + Arduino Giga Display Shield, for displaying the results on a local dashboard and optionally to send data to Arduino Cloud
  - Arduino Nano RP2040 for reading from the Cloud, via WiFi in pseudo real time, the results sent from from the Giga R1 to the Cloud, using the "Thing-to-thing" synchronization.
    This Sketch in built with Micropython, using the experimental Arduino IDE for MicroPython available at https://github.com/arduino/lab-micropython-editor. It also available the Wiring version.
 
- A library built with Edge Impulse which is responsible for the actual image detection. This library is embeded in the Sketch uploaded on the Nicla Vision

Please see individual README files in each folder for further details.

For any questions, please open an issue in this repo.

## References
- https://medium.com/arduino-engineering/car-license-plate-recognition-with-tinyml-e594f08ecacb
- https://projecthub.arduino.cc/c_sarnataro_arduino/cloud-controlled-traffic-barrier-5c5e0f
