#include <stdio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int8.h>
#include <geometry_msgs/msg/twist.h>
#include <std_msgs/msg/string.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/u_int8.h>
#include <geometry_msgs/msg/vector3.h>


#include "pico/stdlib.h"
#include "pico_wifi_transport.h"
#include "hardware/adc.h"

//#ifndef __MAIN_H__
//#define __MAIN_H__
#include <geometry_msgs/msg/twist.h>

void
update_velocity_command(
  geometry_msgs__msg__Twist *cmd_vel,
  uint x,
  uint y);
//#endif /* __MAIN_H__ */


//JOYSTICK
rcl_publisher_t publisher_joy;
rcl_publisher_t publisher_counter;

std_msgs__msg__Int8 msg_counter;
geometry_msgs__msg__Twist msg_twist;
std_msgs__msg__Float32 msg_float;
std_msgs__msg__UInt8 msg_uint;


char ssid[] = "ssid";
char pass[] = "pass";
char agent_ip[] = "192.168.131.21";     // host computer running the micro_ros_agent node (or snap or docker agent)
int agent_port = 8888;

void command_callback(uint value_x)
{
    rcl_ret_t ret = rcl_publish(&publisher_joy, &msg_float, NULL);
}

void timer_callback(rcl_timer_t *timer, uint8_t last_call_time)
{
    rcl_ret_t ret = rcl_publish(&publisher_counter, &msg_float, NULL);
    //msg_counter.data++;
}

void update_velocity_command(geometry_msgs__msg__Twist *cmd_vel, uint x, uint y)
{
    int max_value = 4096;
    float threshold_low = 0.01 * max_value;
    float threshold_high = 0.99 * max_value;
    float min_linear_vel = -0.5;
    float max_linear_vel = 0.5;
    float min_angular_vel = -0.5;
    float max_angular_vel = 0.5;

    float linear_x = 0.5;
    float angular_z = 0.3;

    if (x < threshold_low) { linear_x = min_linear_vel; }
    else if (x > threshold_high) { linear_x = max_linear_vel; }
    else { linear_x = 0.0; }

    if (y < threshold_low) { angular_z = min_linear_vel; }
    else if (y > threshold_high) { angular_z = max_linear_vel; }
    else { angular_z = 0.0; }

    cmd_vel->linear = (geometry_msgs__msg__Vector3){linear_x, 0.0, 0.0};
    cmd_vel->angular = (geometry_msgs__msg__Vector3){0.0, 0.0, angular_z};
}

int main()
{
    set_microros_wifi_transports(ssid, pass, agent_ip, agent_port);
    msg_uint.data = 0;

    stdio_init_all();
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);

    gpio_init(10);
    gpio_set_dir(10, GPIO_OUT);
    gpio_put(10, 1);

    rcl_timer_t timer;
    rcl_allocator_t allocator = rcl_get_default_allocator();

    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    rcl_init_options_init(&init_options, allocator);
    const size_t ros_domain_id = 0;
    rcl_init_options_set_domain_id(&init_options, ros_domain_id);

    rclc_support_t support;
    rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);

    rcl_node_t node;
    const char * node_name = "pico_node";
    const char * node_namespace = "";

    rclc_node_init_default(&node, node_name, node_namespace, &support);


    rclc_executor_t executor;

    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 1000;
    const uint8_t attempts = 120;

    rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);

    if (ret != RCL_RET_OK)
    {
        // Unreachable agent, exiting program.
        printf("Agent unreachable. Exiting...");
        return ret;
    }


    rclc_publisher_init_best_effort(
        &publisher_counter,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
        "counter");

    rclc_publisher_init_best_effort(
        &publisher_joy,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel");

    rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(100),
        timer_callback);

    rclc_executor_init(&executor, &support.context, 2, &allocator);
    rclc_executor_add_timer(&executor, &timer);


    while (true)
    {
        rcl_ret_t ret = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

    //JOYSTICK

        adc_select_input(0);
        const float conversion_factor = 5.0f / (1 << 12);
        uint16_t adc_x_raw = adc_read();
        msg_float.data = adc_x_raw * conversion_factor;
        adc_select_input(1);
        uint16_t adc_y_raw = adc_read();
        update_velocity_command(&msg_twist, adc_x_raw, adc_y_raw);
        rcl_publish(&publisher_joy, &msg_twist, NULL);
        sleep_ms(50);       // timer?
    }
    return 0;
}
