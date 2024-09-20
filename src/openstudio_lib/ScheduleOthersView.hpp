/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULEOTHERSVIEW_HPP
#define OPENSTUDIO_SCHEDULEOTHERSVIEW_HPP

#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

class QStackedWidget;

namespace openstudio {

class ScheduleOthersView : public ModelSubTabView
{
  Q_OBJECT

 public:
  explicit ScheduleOthersView(const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleOthersView() = default;

 private:
  static std::vector<std::pair<IddObjectType, std::string>> modelObjectTypesAndNames();
};

class ScheduleOthersInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit ScheduleOthersInspectorView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleOthersInspectorView() = default;

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void showScheduleConstantView(const openstudio::model::ModelObject& modelObject);
  void showScheduleCompactView(const openstudio::model::ModelObject& modelObject);
  void showScheduleFileView(const openstudio::model::ModelObject& modelObject);

  void showInspector(QWidget* widget);

  void showDefaultView();

  std::map<openstudio::IddObjectType, int> m_inspectorIndexMap;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULEOTHERSVIEW_HPP
