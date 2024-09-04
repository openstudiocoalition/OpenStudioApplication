/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_PROGRESSBARWITHERROR_HPP
#define SHAREDGUICOMPONENTS_PROGRESSBARWITHERROR_HPP

#include <QProgressBar>

namespace openstudio {

class ProgressBarWithError : public QProgressBar
{
  Q_OBJECT

 public:
  explicit ProgressBarWithError(QWidget* parent = nullptr);
  virtual ~ProgressBarWithError() {}
  void setError(bool error);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_PROGRESSBARWITHERROR_HPP
