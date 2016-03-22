#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <nav_msgs/Odometry.h>

#include <iostream>
#include <vector>
#include <cmath>

#define N_WPS 9
#define THRESHOLD 0.05


ros::Publisher pub_wp_i, pub_wp_f;
geometry_msgs::Point wp_i_msg, wp_f_msg;

std::vector<geometry_msgs::Point> waypoints_i, waypoints_f;

double wp_i_x[N_WPS] = {10.875, 12.2, 14.3,   6.98,   -6.78, -13.05, -1.175,  -13.15, -10.725};
double wp_i_y[N_WPS] = {0.0	 , 10.5, -15.95, -20.17, -19.5, -12.68, -10.825, -9.075, 0.0};

double wp_f_x[N_WPS] = {10.725, 14.7, 9.35,   -4.2,   -14.03, -1.175, -11.8,   -13.15, 10.875};
double wp_f_y[N_WPS] = {-8.0,   14.1, -19.45, -20.17, -14.35, -12.68, -10.825, -1.625, 0.0};

int wp_index;


void fillWaypoints()
{
	geometry_msgs::Point pt_i, pt_f;
	for(int index = 0; index<N_WPS; index++)
	{
		pt_i.x = wp_i_x[index];
		pt_i.y = wp_i_y[index];
		
		pt_f.x = wp_f_x[index];
		pt_f.y = wp_f_y[index];
		
		waypoints_i.push_back(pt_i);
		waypoints_f.push_back(pt_f);
	}
	return;    
}

void initWPAndPublishFirst()
{
    wp_index = 0;
    
    wp_i_msg = waypoints_i[wp_index];
    wp_f_msg = waypoints_f[wp_index];
    
    pub_wp_i.publish(wp_i_msg);
    pub_wp_f.publish(wp_f_msg);
}

void odom_callback(const nav_msgs::OdometryConstPtr &odom_msg)
{
    geometry_msgs::Point pos = odom_msg->pose.pose.position;
    
    double distance = sqrt(pow(pos.x - waypoints_f[wp_index].x, 2) + pow(pos.y - waypoints_f[wp_index].y, 2));

    if(distance < THRESHOLD)
    {
        wp_index++;
        
        wp_i_msg = waypoints_i[wp_index];
        wp_f_msg = waypoints_f[wp_index];
        
        pub_wp_i.publish(wp_i_msg);
        pub_wp_f.publish(wp_f_msg);
    }
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "way_points");
    ros::NodeHandle node;
    ros::Subscriber sub_odom = node.subscribe("/vrep/vehicle/odometry", 1, odom_callback);

	fillWaypoints();
    
    for(int i=0; i<N_WPS; i++)
    {   
        std::cout << i << " - i: " << waypoints_i[i].x << ", " << waypoints_i[i].y << std::endl;
        std::cout << i << " - f: " << waypoints_f[i].x << ", " << waypoints_f[i].y << std::endl;
    }

    pub_wp_i = node.advertise<geometry_msgs::Point>("/amr/waypoints_i", 1);
    pub_wp_f = node.advertise<geometry_msgs::Point>("/amr/waypoints_f", 1);
    
    initWPAndPublishFirst();
	
	ros::spin();
}
