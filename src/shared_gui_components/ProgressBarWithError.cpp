/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ProgressBarWithError.hpp"

namespace openstudio {

ProgressBarWithError::ProgressBarWithError(QWidget* parent) : QProgressBar(parent) {
  setObjectName("ProgressBarWithError");
  setError(false);
}

void ProgressBarWithError::setError(bool error) {
  if (error) {
    this->setFormat("");
    this->setTextVisible(false);
    this->setStyleSheet("QProgressBar::chunk{background-color: #FF0000;}");
  } else {
    this->setFormat("");
    this->setTextVisible(false);
    this->setStyleSheet("");
  }
}

}  // namespace openstudio
