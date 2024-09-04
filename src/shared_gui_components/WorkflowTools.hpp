/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_WORKFLOWTOOLS_HPP
#define SHAREDGUICOMPONENTS_WORKFLOWTOOLS_HPP

#include <vector>
#include <string>
#include <boost/optional.hpp>

namespace openstudio {

namespace analysis {
class Problem;
}

//void getRadiancePreRunWarningsAndErrors(std::vector<std::string> &t_warnings,
//    std::vector<std::string> &t_errors,
//    openstudio::runmanager::RunManager &t_runManager,
//    boost::optional<openstudio::model::Model> &t_model);

//bool projectHasRadiance(const openstudio::analysisdriver::SimpleProject &t_project);

//void addRadianceToProject(openstudio::analysisdriver::SimpleProject &t_project);

//void removeRadianceFromProject(openstudio::analysisdriver::SimpleProject &t_project);

//boost::optional<int> getModelMeasureInsertStep(const openstudio::analysis::Problem &t_problem);
}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_WORKFLOWTOOLS_HPP
