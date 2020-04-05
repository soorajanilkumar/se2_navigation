/*
 * PlannerRos.hpp
 *
 *  Created on: Apr 5, 2020
 *      Author: jelavice
 */

#pragma once

#include <ros/ros.h>
#include "se2_navigation_msgs/PathRequestSrv.h"
#include "se2_planning/Planner.hpp"

namespace se2_planning {

class PlannerRos : public Planner {
 public:
  void setStartingState(const State& startingState) override;
  void setGoalState(const State& goalState) override;
  bool plan() override;
  void getPath(Path* path) const override;
  bool reset() override;
  bool initialize() override;
  void getStartingState(State* startingState) override;
  void getGoalState(State* goalState) override;

  void setPlanner(std::unique_ptr<Planner> planner);

 protected:
  using PlanningService = se2_navigation_msgs::PathRequestSrv;

  explicit PlannerRos(ros::NodeHandlePtr nh);
  ~PlannerRos() override = default;
  virtual bool planningService(PlanningService::Request& req, PlanningService::Response& res) = 0;

  ros::NodeHandlePtr nh_;
  std::unique_ptr<Planner> planner_;
};

} /* namespace se2_planning*/
