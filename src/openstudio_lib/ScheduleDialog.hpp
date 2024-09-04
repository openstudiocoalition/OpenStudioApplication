/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULEDIALOG_HPP
#define OPENSTUDIO_SCHEDULEDIALOG_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ScheduleTypeRegistry.hpp>
#include <openstudio/model/ScheduleTypeLimits.hpp>

#include "../shared_gui_components/OSDialog.hpp"

#include <set>

class QLabel;

class QComboBox;

namespace openstudio {

class ScheduleDialog : public OSDialog
{
  Q_OBJECT

 public:
  ScheduleDialog(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleDialog() = default;

  void setIsIP(bool isIP);

 private slots:

  void onCurrentIndexChanged(int index);

 private:
  virtual void createLayout() override;
  void createLayoutInternal();

  model::Model m_model;

  boost::optional<model::ScheduleTypeLimits> m_scheduleTypeLimits;

  QComboBox* m_scheduleTypeComboBox;

  QLabel* m_numericTypeLabel;

  QLabel* m_lowerLimitLabel;

  QLabel* m_upperLimitLabel;

 protected slots:

  virtual void on_okButton(bool checked) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULEDIALOG_HPP
