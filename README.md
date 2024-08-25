# Edrys-lab - Micro-ROS

Welcome to the Micro-ROS Remote Lab! This interactive learning environment is designed to introduce beginners to give an overview over the fundamentals of ROS 2 with Micro-ROS. This way, beginners can build and test their own programs, without the need to set up their own ROS environment. 

## Lab Overview

This lab consists of two modules:

1. [module-markdown-it](https://github.com/Cross-Lab-Project/edrys_module-markdown-it):
This module is used to render the markdown content of the lab.
2. [module-editor](https://github.com/edrys-labs/module-editor):
This module is used to give the user a simple code editor.
3. [module-station stream](https://github.com/edrys-labs/module-station-stream):
This module is used to stream a video of the micro controller.
2. [module-pyxtermjs](https://github.com/edrys-labs/module-pyxtermjs):
This module is used to provide a terminal interface to the lab. Use the Dockerfile of this container to create a modified ROS2 terminal server.

---

The current lab-configuration can be imported from here:

https://raw.githubusercontent.com/nfriedrich00/edrys-lab-micro-ros/main/laboratory/micro-ros.yaml

---

If you already have created a new classroom at:

https://edrys-labs.github.io

then simply copy the lab-config URL and import the lab configuration:

... Otherwise try out the fast deploy by clicking onto the Deploy-Button. This will load the configuration automatically within your browser. The created classroom will be unique and can be shared with your students.

[<img src="https://img.shields.io/badge/%F0%9F%9A%80%20-%20Deploy%20Lab%20-%20light?style=plastic" height="25" />](https://edrys-labs.github.io/?/deploy/https://raw.githubusercontent.com/nfriedrich00/edrys-lab-micro-ros/main/laboratory/micro-ros.yaml)

## Stations

The module-pyxtermjs does require a terminal-server with access to a ROS 2 environment. This project contains a Dockerfile to create a terminal-server. Use the following commands to build the image and start the container:

```bash
docker build -t pyxtermjs_microros .
```

```bash
docker run --name edrys --rm -it \
    --privileged --security-opt seccomp=unconfined \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v /usr/bin/docker:/usr/bin/docker \
    -v /dev/udev/rules.d:/etc/udev/rules.d \
    -v /dev:/dev -p 5000:5000 --network host pyxtermjs_microros
```
In order for the micro-ros node to communicate with the ROS-network, a micro-ros-agent docker container needs to be running. With the command avove, the docker socket is shared, so the micro-ros-agent container can be started from within another container. However, for this to work, you need to change the permission of the socket:

```bash
sudo chmod 666 /var/run/docker.sock
```

Otherwise, you could just start the micro-ros-agent in advance:

```bash
docker run -d --name micro_ros_agent \
    --rm -v /dev:/dev \
    --privileged --net=host \
    microros/micro-ros-agent:humble udp4 \
    --port 8888
```


Finally, the Arduino Uno is used to reset the pico. Since the adress of the Arduino Uno changes, it is recommended to use a udev rule. This will make sure, the Arduino Uno is always accessible at /dev/arduino_uno. To create a udev rule, use the following command and paste the rule:

```bash
sudo nano /etc/udev/rules.d/99-arduino.rules
```


```
SUBSYSTEM=="tty", \
    ATTRS{idVendor}=="2341", \
    ATTRS{idProduct}=="0043", \
    MODE="0660", \
    GROUP="plugdev", \
    SYMLINK+="arduino_uno"
```