/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOADSVIEW_HPP
#define OPENSTUDIO_LOADSVIEW_HPP

#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/idf/WorkspaceObject_Impl.hpp>

class QStackedWidget;

namespace openstudio {

class LoadsView : public ModelSubTabView
{
  Q_OBJECT

 public:
  LoadsView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~LoadsView() {}

 private:
  static std::vector<std::pair<IddObjectType, std::string>> modelObjectTypesAndNames();

 public slots:

  void toggleUnits(bool displayIP);
};

class LoadsInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  LoadsInspectorView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~LoadsInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

  void showPeopleDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showInternalMassDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showLightsDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showLuminaireDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showElectricEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showGasEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showSteamEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showOtherEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showWaterUseEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showHotWaterEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject);

  void showInspector(QWidget* widget);

  void showDefaultView();

 private:
  bool m_isIP;

 public slots:

  void toggleUnits(bool displayIP) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOADSVIEW_HPP
