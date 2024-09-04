/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ProcessEventsProgressBar.hpp"

#include "../model_editor/Application.hpp"

namespace openstudio {

ProcessEventsProgressBar::ProcessEventsProgressBar() : OSProgressBar(false, nullptr) {}

void ProcessEventsProgressBar::onPercentageUpdated(double percentage) {
  openstudio::Application::instance().processEvents();
}

}  // namespace openstudio
