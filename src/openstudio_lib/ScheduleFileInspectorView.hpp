/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULEFILEINSPECTORVIEW_HPP
#define OPENSTUDIO_SCHEDULEFILEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/ScheduleFile.hpp>

class QLineEdit;
class QPlainTextEdit;
class QPushButton;

namespace openstudio {

class OSLineEdit2;
class OSIntegerEdit2;
class OSComboBox2;
class OSSwitch2;

class ScheduleFileInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit ScheduleFileInspectorView(const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleFileInspectorView() = default;

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::ScheduleFile& sch);

  void detach();

  void refresh();

  void refreshContent();

  void refreshError();

  boost::optional<model::ScheduleFile> m_sch;

  OSLineEdit2* m_nameEdit = nullptr;
  QLineEdit* m_filePath = nullptr;
  OSIntegerEdit2* m_columnNumber = nullptr;
  OSIntegerEdit2* m_rowstoSkipatTop = nullptr;
  OSIntegerEdit2* m_numberofHoursofData = nullptr;
  OSComboBox2* m_columnSeparator = nullptr;
  OSSwitch2* m_interpolatetoTimestep = nullptr;
  OSComboBox2* m_minutesperItem = nullptr;
  OSSwitch2* m_adjustScheduleforDaylightSavings = nullptr;
  OSSwitch2* m_translateFileWithRelativePath = nullptr;

  QLineEdit* m_numLines = nullptr;
  QLabel* m_error = nullptr;
  bool m_displayAllContent = false;
  QPushButton* m_displayAllContentSwitch = nullptr;
  QPlainTextEdit* m_contentLines = nullptr;
  QStringList m_lines;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULEFILEINSPECTORVIEW_HPP
