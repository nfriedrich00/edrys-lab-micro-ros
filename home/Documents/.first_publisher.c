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

// Network settings
char ssid[] = "ssid";
char pass[] = "pass";
char agent_ip[] = "192.168.204.126";     // host computer running the micro_ros_agent node (or snap or docker agent)
int agent_port = 8888;


// Hardware settings
const int LED_RED = 10;
const int LED_GREEN = 11;
const int LED_BLUE = 12;

// Simple publisher
rcl_publisher_t publisher_counter;
std_msgs__msg__Int8 msg_counter;

int counter = 0;

void turn_on_green_led()
{
  gpio_put(LED_GREEN, 1);
}

void turn_off_green_led()
{
  gpio_put(LED_GREEN, 0);
}

void timer_callback(rcl_timer_t *timer, uint8_t last_call_time)
{
  toggle_green_led();
  rcl_ret_t ret = rcl_publish(&publisher_counter, &msg_counter, NULL);
  msg_counter.data = counter;

}

void toggle_green_led()
{
  if(counter % 2 == 0)
  {
    turn_on_green_led();
  }
  else
  {
    turn_off_green_led();
  }
  counter++;
}


int main()
{
  set_microros_wifi_transports(ssid, pass, agent_ip, agent_port);
  gpio_init(LED_GREEN);
  gpio_set_dir(LED_GREEN, GPIO_OUT);
  gpio_init(LED_GREEN);
  gpio_set_dir(LED_GREEN, GPIO_OUT);
  stdio_init_all();

  // ROS client Library timer, allocater, executer and support
  rcl_timer_t timer;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_init_options_init(&init_options, allocator);
  const size_t ros_domain_id = 0;
  rcl_init_options_set_domain_id(&init_options, ros_domain_id);
  rclc_support_t support;
  rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);
  rclc_executor_t executor;

  rcl_node_t node;
  const char * node_name = "pico_node";
  const char * node_namespace = "";

  rclc_node_init_default(&node, node_name, node_namespace, &support);

  // Wait for agent successful ping for 2 minutes.
  const int timeout_ms = 1000;
  const uint8_t attempts = 120;
  rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);

  rclc_publisher_init_best_effort(
    &publisher_counter,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8),
    "/counter");

  rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(1000),
    timer_callback);

  rclc_executor_init(&executor, &support.context, 2, &allocator);
  rclc_executor_add_timer(&executor, &timer);

  while (true)
  {
    rcl_ret_t ret = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1000));
  }
  return 0;
}

