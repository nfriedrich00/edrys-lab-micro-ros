FROM crosslab/edrys_pyxtermjs:latest

# change the user, so that the packages can be installed on top of edrys_pyxtermjs
USER root

RUN DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y apt-transport-https

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y mono-complete mono-mcs

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y g++ make

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y cmake protobuf-compiler gcc-arm-none-eabi doxygen libnewlib-arm-none-eabi git python3 build-essential pkg-config libusb-1.0-0-dev iproute2 avrdude tmux

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y ca-certificates curl && \
    install -m 0755 -d /etc/apt/keyrings && \
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc && \
    chmod a+r /etc/apt/keyrings/docker.asc && \
    echo \
      "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
      $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
      tee /etc/apt/sources.list.d/docker.list > /dev/null && \
    apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y golang-go

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y rustc

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y default-jdk

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y r-base r-base-dev r-recommended \
    r-cran-car \
    r-cran-caret \
    r-cran-data.table \
    r-cran-dplyr \
    r-cran-gdata \
    r-cran-ggplot2 \
    r-cran-lattice \
    r-cran-lme4 \
    r-cran-mapdata \
    r-cran-maps \
    r-cran-maptools \
    r-cran-mgcv \
    r-cran-mvtnorm \
    r-cran-nlme \
    r-cran-reshape \
    r-cran-stringr \
    r-cran-survival \
    r-cran-tidyr \
    r-cran-xml \
    r-cran-xml2 \
    r-cran-xtable \
    r-cran-xts \ 
    r-cran-zoo

# RUN DEBIAN_FRONTEND=noninteractive apt-get install -y julia

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y ghc

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y clojure

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y lua5.3

RUN DEBIAN_FRONTEND=noninteractive apt-get install nodejs

ENV ROS_DISTRO humble
ARG INSTALL_PACKAGE=ros-base
RUN apt-get update -q && \
    apt-get install -y curl gnupg2 lsb-release && \
    curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null && \
    apt-get update -q && \
    apt-get install -y ros-${ROS_DISTRO}-${INSTALL_PACKAGE} \
    python3-argcomplete \
    python3-colcon-common-extensions \
    python3-rosdep python3-vcstool && \
    rosdep init && \
    rm -rf /var/lib/apt/lists/*
    
RUN rosdep update

COPY home/micro_ros_pico_w/ /home/appuser/micro_ros_pico_w
COPY home/Documents/ /home/appuser/Documents
RUN chown -R appuser:appuser /home/appuser/micro_ros_pico_w
RUN chown -R appuser:appuser /home/appuser/Documents
RUN chmod 0444 /home/appuser/Documents/first_node.c
RUN chmod 0444 /home/appuser/Documents/.first_publisher.c
RUN cp /home/appuser/Documents/restart_pico.sh /usr/bin/
RUN usermod -aG plugdev appuser

# get back to the default appuser
USER appuser

RUN DEBIAN_FRONTEND=noninteractive cd /home/appuser/ && \
	git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git /home/appuser/pico-sdk && \
	echo "export PICO_SDK_PATH=/home/appuser/pico-sdk" >> /home/appuser/.bashrc

RUN DEBIAN_FRONTEND=noninteractive cd /home/appuser/ && \
	git clone https://github.com/raspberrypi/picotool.git && \
    cd /home/appuser/picotool && \
    mkdir build && \
    cd build && \
    export PICO_SDK_PATH=/home/appuser/pico-sdk && \
    cmake .. && \
    make

EXPOSE 5000

ENTRYPOINT python3 -m pyxtermjs --cors True --host 0.0.0.0 --command bash
