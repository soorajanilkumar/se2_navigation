/*
 * OmplPlanner.cpp
 *
 *  Created on: Apr 1, 2020
 *      Author: jelavice
 */

#include "se2_planning/OmplPlanner.hpp"

#include <functional>
#include <iostream>

namespace se2_planning {

void OmplPlanner::setStartingState(const State& startingState) {
  startState_ = convert(startingState);
}

void OmplPlanner::setGoalState(const State& goalState) {
  goalState_ = convert(goalState);
}

void OmplPlanner::getPath(Path* path) const {
  convert(*interpolatedPath_, path);
}

bool OmplPlanner::plan() {
  simpleSetup_->clear();
  simpleSetup_->setStartAndGoalStates(*startState_, *goalState_);
  if (!simpleSetup_->solve(maxPlanningDuration_)) {
    std::cout << "OmplPlanner: Solve failed" << std::endl;
    return false;
  }

  const ompl::geometric::PathGeometric solution = simpleSetup_->getSolutionPath();
  *path_ = solution;
  *interpolatedPath_ = solution;
  std::cout << "Solution plan has: " << solution.getStateCount() << " states." << std::endl;

  return true;
}
bool OmplPlanner::reset() {
  simpleSetup_->clear();
  return true;
}
bool OmplPlanner::initialize() {
  initializeStateSpace();
  if (stateSpace_ == nullptr) {
    std::cerr << "OmplPlanner:: state space is nullptr" << std::endl;
    return false;
  }
  simpleSetup_.reset(new ompl::geometric::SimpleSetup(stateSpace_));
  ompl::base::SpaceInformationPtr si = simpleSetup_->getSpaceInformation();
  simpleSetup_->setStateValidityChecker(std::bind(&OmplPlanner::isStateValid, this, si.get(), std::placeholders::_1));
  path_ = std::make_unique<ompl::geometric::PathGeometric>(si);
  interpolatedPath_ = std::make_unique<ompl::geometric::PathGeometric>(si);
  return true;
}

void OmplPlanner::setMaxPlanningDuration(double T) {
  maxPlanningDuration_ = T;
}

ompl::geometric::PathGeometric& OmplPlanner::getOmplPath() const {
  if (path_ == nullptr) {
    throw std::runtime_error("Ompl planner: path_ is nullptr");
  }
  return *path_;
}

ompl::geometric::PathGeometric& OmplPlanner::getOmplInterpolatedPath() const {
  if (interpolatedPath_ == nullptr) {
    throw std::runtime_error("Ompl planner: interpolatedPath_ is nullptr");
  }
  return *interpolatedPath_;
}

ompl::geometric::PathGeometric interpolatePath(const ompl::geometric::PathGeometric& inputPath, double desiredResolution) {
  ompl::geometric::PathGeometric interpolatedPath = inputPath;
  const unsigned int currentNumPoints = inputPath.getStateCount();
  const unsigned int desiredNumPoints = static_cast<unsigned int>(std::ceil(std::fabs(inputPath.length()) / desiredResolution));
  const unsigned int numPoints = std::max(currentNumPoints, desiredNumPoints);
  interpolatedPath.interpolate(numPoints);
  return interpolatedPath;
}

} /*namespace se2_planning */
