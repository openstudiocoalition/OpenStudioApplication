/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONSVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONSVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelSubTabView.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class ConstructionsView : public ModelSubTabView
{
  Q_OBJECT

 public:
  ConstructionsView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionsView() {}

 private:
  static std::vector<std::pair<IddObjectType, std::string>> modelObjectTypesAndNames();
};

class ConstructionsInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionsInspectorView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionsInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void showConstructionInspector(const openstudio::model::ModelObject& modelObject);

  void showAirBoundaryInspector(const openstudio::model::ModelObject& modelObject);

  void showCfactorUndergroundWallInspector(const openstudio::model::ModelObject& modelObject);

  void showFfactorGroundFloorInspector(const openstudio::model::ModelObject& modelObject);

  void showInternalSourceInspector(const openstudio::model::ModelObject& modelObject);

  void showWindowDataFileInspector(const openstudio::model::ModelObject& modelObject);

  void showDefaultView();

  void showInspector(QWidget* widget);

  std::map<openstudio::IddObjectType, int> m_inspectorIndexMap;

  bool m_isIP;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONSVIEW_HPP
