/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
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

RecalculateButton::RecalculateButton(QWidget* parent) : QPushButton(parent) {
  setFlat(true);
  setFixedSize(24, 24);
  setObjectName("RecalculateButton");
}

}  // namespace openstudio
