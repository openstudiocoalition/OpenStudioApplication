/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULECOMPACTINSPECTORVIEW_HPP
#define OPENSTUDIO_SCHEDULECOMPACTINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include <openstudio/model/ScheduleCompact.hpp>

class QPlainTextEdit;

namespace openstudio {

class OSLineEdit2;

class ScheduleCompactInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit ScheduleCompactInspectorView(const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleCompactInspectorView() = default;

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(openstudio::model::ScheduleCompact& sch);

  void detach();

  void refresh();

  boost::optional<model::ScheduleCompact> m_sch;

  OSLineEdit2* m_nameEdit = nullptr;

  QPlainTextEdit* m_content = nullptr;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULECOMPACTINSPECTORVIEW_HPP
