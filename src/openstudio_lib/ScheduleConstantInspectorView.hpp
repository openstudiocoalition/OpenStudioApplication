/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULECONSTANTINSPECTORVIEW_HPP
#define OPENSTUDIO_SCHEDULECONSTANTINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/ScheduleConstant.hpp>

namespace openstudio {

class OSLineEdit2;

class OSDoubleEdit2;

class ScheduleConstantInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit ScheduleConstantInspectorView(const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleConstantInspectorView() = default;

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::ScheduleConstant& sch);

  void detach();

  void refresh();

  boost::optional<model::ScheduleConstant> m_sch;

  OSLineEdit2* m_nameEdit = nullptr;

  OSDoubleEdit2* m_value = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULECONSTANTINSPECTORVIEW_HPP
