/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "Buttons.hpp"
#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>

namespace openstudio {

ZoomInButton::ZoomInButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setObjectName("ZoomInButton");
}

ZoomOutButton::ZoomOutButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setObjectName("ZoomOutButton");
}

OneLevelUpButton::OneLevelUpButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(22, 24);
  setObjectName("OneLevelUpButton");
}

OneLevelDownButton::OneLevelDownButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(22, 24);
  setObjectName("OneLevelDownButton");
}

GrayButton::GrayButton(QWidget* parent) : QPushButton(parent) {
  setObjectName("StandardGrayButton");
}

BlueButton::BlueButton(QWidget* parent) : QPushButton(parent) {
  setObjectName("StandardBlueButton");
}

AddButton::AddButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("AddButton");
}

SofterAddButton::SofterAddButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(16, 16);
  setObjectName("SofterAddButton");
}

RemoveButton::RemoveButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("DeleteButton");
}

SofterRemoveButton::SofterRemoveButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(16, 16);
  setObjectName("SofterDeleteButton");
}

AddScriptButton::AddScriptButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("AddScriptButton");
}

DuplicateButton::DuplicateButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("CopyButton");
}

SofterDuplicateButton::SofterDuplicateButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(16, 16);
  setObjectName("SofterCopyButton");
}

UpButton::UpButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(22, 24);
  setObjectName("UpButton");
}

DownButton::DownButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(22, 24);
  setObjectName("DownButton");
}

MyMeasuresFolderButton::MyMeasuresFolderButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("MyMeasuresFolderButton");
}

CheckForUpdateButton::CheckForUpdateButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("CheckForUpdateButton");
}

OpenDirectoryButton::OpenDirectoryButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(31, 24);
  setObjectName("OpenDirectoryButton");
}

CreateOneWithSelectedMeasuresButton::CreateOneWithSelectedMeasuresButton() : GrayButton() {
  setText("Create One With Selected Measures");
}

CreateOneForEachSelectedMeasureButton::CreateOneForEachSelectedMeasureButton() : GrayButton() {
  setText("Create One For Each Selected Measure");
}

CreateFromFileButton::CreateFromFileButton() : GrayButton() {
  setText("Create From External File");
}

PlayButton::PlayButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(35, 35);

  setStatus(IDLE);
}

PlayButton::Status PlayButton::status() const {
  return m_status;
}

void PlayButton::setStatus(const Status& status) {
  QString style;

  switch (status) {
    case IDLE:
      style.append("QPushButton {"
                   "  border: none;"
                   "  background: transparent;"
                   "  background-image:url(':/images/run_simulation_button.png');"
                   "}");
      style.append("QPushButton:hover {"
                   "  background-image:url(':/images/run_simulation_over.png');"
                   "}");
      style.append("QPushButton:pressed {"
                   "  background-image:url(':/images/run_simulation_press.png');"
                   "}");
      setStyleSheet(style);
      setEnabled(true);
      break;
    case IDLEDISABLED:
      style.append("QPushButton {"
                   "  border: none;"
                   "  background: transparent;"
                   "  background-image:url(':/images/run_simulation_button_disabled.png');"
                   "}");
      setStyleSheet(style);
      setEnabled(false);
      break;
    case STARTING:
      style.clear();
      style.append("QPushButton {"
                   "  border: none;"
                   "  background: transparent;"
                   "  background-image:url(':/images/starting_simulation_button.png');"
                   "}");

      setStyleSheet(style);
      setEnabled(false);
      break;
    case RUNNING:
      style.clear();
      style.append("QPushButton {"
                   "  border: none;"
                   "  background: transparent;"
                   "  background-image:url(':/images/run_cancel.png');"
                   "}");
      style.append("QPushButton:hover {"
                   "  background-image:url(':/images/run_cancel_over.png');"
                   "}");
      style.append("QPushButton:pressed {"
                   "  background-image:url(':/images/run_cancel_press.png');"
                   "}");

      setStyleSheet(style);
      setEnabled(true);
      break;
    case STOPPING:
      style.clear();
      style.append("QPushButton {"
                   "  border: none;"
                   "  background: transparent;"
                   "  background-image:url(':/images/stopping_simulation_button.png');"
                   "}");

      setStyleSheet(style);
      setEnabled(false);
      break;
    case ERROR:
      style.clear();
      style.append("QPushButton {"
                   "  border: none;"
                   "  background: transparent;"
                   "  background-image:url(':/images/run_simulation_button.png');"
                   "}");
      style.append("QPushButton:hover {"
                   "  background-image:url(':/images/run_simulation_over.png');"
                   "}");
      style.append("QPushButton:pressed {"
                   "  background-image:url(':/images/run_simulation_press.png');"
                   "}");
      setStyleSheet(style);
      setEnabled(true);
      break;
  }

  m_status = status;
}

}  // namespace openstudio
