while getopts c: flag
do
    case "${flag}" in
        c) CODE=${OPTARG};;
    esac
done 
cd ~/micro_ros_pico_w
echo $CODE | base64 --decode > main.c
#if ! [ -d "/home/appuser/micro_ros_pico_w/build" ]; then
#	mkdir build
#fi
#cd build
#cmake ..
#make

