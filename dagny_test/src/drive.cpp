/*
 * drive.cpp
 *
 * Drive the robot in a straight line
 *
 * Author: Austin Hendrix
 */

#include <ros/ros.h>

#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>

double dist = 0.0;

ros::Publisher control_pub;

double x = 0.0;
double y = 0.0;
double theta = 0.0;

double steer;

void odoCallback(const nav_msgs::Odometry::ConstPtr & msg) {
   x += msg->pose.pose.position.x;
   y += msg->pose.pose.position.y;
   theta += msg->pose.pose.orientation.x;

   ROS_INFO("Odometry position (%lf, %lf, %lf)", x, y, theta);

   geometry_msgs::Twist c;

   if( hypot(x, y) < dist ) {
      c.linear.x = 0.15;
   } else {
      c.linear.x = 0;
   }
   c.angular.z = steer; // drive straight

   control_pub.publish(c);
}

int main(int argc, char ** argv) {
   ros::init(argc, argv, "drive");

   if( argc == 2 ) {
      sscanf(argv[1], "%lf", &dist);
      steer = 0;
      ROS_INFO("Driving forward %lf units", dist);
   }
   if( argc == 3 ) {
      sscanf(argv[1], "%lf", &dist);
      sscanf(argv[2], "%lf", &steer);
      ROS_INFO("Driving %lf units with steer %lf", dist, steer);
   }

   ros::NodeHandle n;

   control_pub = n.advertise<geometry_msgs::Twist>("control", 10);

   ros::Subscriber s = n.subscribe("base_odometry", 10, odoCallback);

   while( hypot(x, y) < dist && ros::ok() ) {
      ros::spinOnce();
   }
   geometry_msgs::Twist c;
   c.linear.x = 0;
   c.angular.z = 0;
   control_pub.publish(c);
   ROS_INFO("Goal reached");
   return 0;
}
