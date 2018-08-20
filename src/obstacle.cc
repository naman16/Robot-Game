/**
 * @file obstacle.cc
 *
 * @copyright 2017 3081 Staff, All rights reserved.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "src/obstacle.h"
#include "src/params.h"
/*******************************************************************************
 * Namespaces
 ******************************************************************************/
NAMESPACE_BEGIN(csci3081);

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
Obstacle::Obstacle() :
    motion_handler_(this),
    motion_behavior_(this) {
  set_color(OBSTACLE_COLOR);
  set_pose(OBSTACLE_POSITION);
  set_radius(OBSTACLE_RADIUS);
  set_type(kObstacle);
  motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
}

void Obstacle::Reset() {
  set_pose(SetPoseRandomly());
  set_type(kObstacle);
  set_color(OBSTACLE_COLOR);
  set_radius(SetRadiusRandomly());
  sensor_touch_->Reset();
  motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
}/* Reset */


void Obstacle::TimestepUpdate(unsigned int dt) {
  motion_behavior_.UpdatePose(dt, motion_handler_.get_velocity());
  // Reset Sensor for next cycle
  sensor_touch_->Reset();
} /* TimestepUpdate() */


// Function to deal with the different cases when
// obstacle undergoes collision
void Obstacle::HandleCollision(EntityType object_type, ArenaEntity * object) {
  sensor_touch_->HandleCollision(object_type, object);
  switch (object_type) {
    case (kObstacle) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    break;
    case (kTopWall) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    break;
    case (kBottomWall) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    break;
    case (kLeftWall) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    break;
    case (kRightWall) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    break;
    case (kBase) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    case(kRobot) : ArcMovement();
    motion_handler_.set_velocity(OBSTACLE_SPEED, OBSTACLE_SPEED);
    break;
    default : break;
  }
}

// Function that gives obstacles the arc movement when obstacles collide
void Obstacle::ArcMovement() {
  /* 
   * Changing the headingangle of the obstacle by 180 and then
   * running a while loop 10 times in which the angle is made
   * negative and changing the angle by 10 in each iteration. 
   * Furthermore, in every iteration of the while loop, 
   * decreasing the left wheel velocity of the obstacle by 1 and increasing
   * the right wheel velocity by 1 and also updating the position according
   * to the new velocity and angle. 
   */
  double dt = 10;
  int angle = 180;
  while (dt >= 0) {
    // static_cast<double>  (random() %
    // (OBSTACLE_MAX_ANGLE + 1 - OBSTACLE_MIN_ANGLE) + OBSTACLE_MIN_ANGLE);
    (this)->RelativeChangeHeading(angle);;
    motion_handler_.set_velocity(motion_handler_.get_velocity().left-1,
    motion_handler_.get_velocity().right+1);
    motion_behavior_.UpdatePose(dt, motion_handler_.get_velocity());
    // adjusting angle so that it is moving backwards in an arc
    // doing the adjustment based on the sign of the angle so that it
    // doesn't get stuck
    if (angle > 0) {
      angle = angle + 10;
    } else {
      angle = -angle + 10;
    }
    dt = dt - 1;
  }
}

NAMESPACE_END(csci3081);
