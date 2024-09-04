/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCHEDULESETSVIEW_HPP
#define OPENSTUDIO_SCHEDULESETSVIEW_HPP

#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

class QStackedWidget;

namespace openstudio {

class ScheduleSetsView : public ModelSubTabView
{
  Q_OBJECT

 public:
  explicit ScheduleSetsView(const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleSetsView() {}

 private:
  static std::vector<std::pair<IddObjectType, std::string>> modelObjectTypesAndNames();
};

class ScheduleSetsInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  explicit ScheduleSetsInspectorView(const model::Model& model, QWidget* parent = nullptr);

  virtual ~ScheduleSetsInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  std::map<openstudio::IddObjectType, int> m_inspectorIndexMap;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCHEDULESETSVIEW_HPP
