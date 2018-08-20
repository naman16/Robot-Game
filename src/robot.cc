
/**
 * @file robot.cc
 *
 * @copyright 2017 3081 Staff, All rights reserved.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "src/robot.h"
#include "src/params.h"
#include "src/base.h"
#include "src/pose.h"
/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NAMESPACE_BEGIN(csci3081);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
Robot::Robot() :
    motion_handler_(this),
    motion_behavior_(this),
    lives_(9),
    time_counter_(0),
    collision_tracker_(false) {
  set_type(kRobot);
  set_color(ROBOT_COLOR);
  set_pose(ROBOT_INIT_POS);
  set_radius(ROBOT_RADIUS);
}
/*******************************************************************************
 * Member Functions
 ******************************************************************************/
void Robot::TimestepUpdate(unsigned int dt) {
  // Update heading as indicated by touch sensor
  motion_handler_.UpdateVelocity();

  // Use velocity and position to update position
  motion_behavior_.UpdatePose(dt, motion_handler_.get_velocity());

  /*
   * Mercy invincibility
   */

  // if robot recently collided with an obstacle setting this flag to
  // true to give robot time to recover
  if (collision_tracker_) {
    time_counter_ += 1;
    // flickering the robot color to denote mercy invincibility
    if (time_counter_ % 10 == 0) {
      set_color(CHANGED_ROBOT_COLOR1);
    } else if (time_counter_ % 5 == 0) {
      set_color(CHANGED_ROBOT_COLOR2);
    }
  }
  // checking if approximately 2 seconds have elapsed or not
  // approximately 50 updates of TimestepUpdate corresponds to 2s.
  if (time_counter_ == 50) {
    time_counter_ = 0;
    collision_tracker_ = false;
    set_color(ROBOT_COLOR);
  }
  // Reset Sensor for next cycle
  sensor_touch_->Reset();
} /* TimestepUpdate() */

void Robot::Reset() {
  set_pose(SetPoseRandomly());
  motion_handler_.set_max_speed(ROBOT_MAX_SPEED);
  motion_handler_.set_max_angle(ROBOT_MAX_ANGLE);
  motion_handler_.set_velocity(0.0, 0.0);
  sensor_touch_->Reset();
  set_color(ROBOT_COLOR);
  lives_ = 9;
  time_counter_ = 0;
  collision_tracker_ = false;
} /* Reset() */


// Handling collisions of robots with other entities and walls
void Robot::HandleCollision(EntityType object_type, ArenaEntity * object) {
  sensor_touch_->HandleCollision(object_type, object);
  switch (object_type) {
    case (kObstacle) : motion_handler_.set_velocity(0.0, 0.0);
    if (!collision_tracker_) {  // checking if mercy invincibility required
    lives_ = lives_-1;
    collision_tracker_ = true;
    set_color(CHANGED_ROBOT_COLOR1);
    }
    break;
    case (kTopWall) : motion_handler_.set_velocity(0.0, 0.0);
    if (!collision_tracker_) {  // checking if mercy invincibility required
    lives_ = lives_-1;
    }
    break;
    case (kBottomWall) : motion_handler_.set_velocity(0.0, 0.0);
    if (!collision_tracker_) {  // checking if mercy invincibility required
    lives_ = lives_-1;
    }
    break;
    case (kLeftWall) : motion_handler_.set_velocity(0.0, 0.0);
    if (!collision_tracker_) {  // checking if mercy invincibility required
    lives_ = lives_-1;
    }
    break;
    case (kRightWall) : motion_handler_.set_velocity(0.0, 0.0);
    if (!collision_tracker_) {  // checking if mercy invincibility required
    lives_ = lives_-1;
    }
    break;
    case (kBase) : motion_handler_.set_velocity(0.0, 0.0);
    object->set_color(CHANGED_BASE_COLOR);
    dynamic_cast <Base*> (object)-> set_captured(true);
    break;
    default : break;
  }
}

void Robot::IncreaseSpeed() {
  motion_handler_.IncreaseSpeed();
}
void Robot::DecreaseSpeed() {
  motion_handler_.DecreaseSpeed();
}
void Robot::TurnRight() {
  motion_handler_.TurnRight();
}
void Robot::TurnLeft() {
  motion_handler_.TurnLeft();
}



NAMESPACE_END(csci3081);
