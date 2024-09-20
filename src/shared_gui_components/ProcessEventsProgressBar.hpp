/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_PROCESSEVENTSPROGRESSBAR_HPP
#define SHAREDGUICOMPONENTS_PROCESSEVENTSPROGRESSBAR_HPP

#include "../model_editor/OSProgressBar.hpp"

namespace openstudio {

// uses the progress bar interface in energy plus and version translators
// to call process events
class ProcessEventsProgressBar : public OSProgressBar
{
 public:
  ProcessEventsProgressBar();
  virtual void onPercentageUpdated(double percentage) override;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_PROCESSEVENTSPROGRESSBAR_HPP
