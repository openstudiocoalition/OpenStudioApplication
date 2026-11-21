/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LoadsView.hpp"
#include "ModelObjectTypeListView.hpp"
#include "../model_editor/IddObjectDocUrl.hpp"
#include "PeopleInspectorView.hpp"
#include "InternalMassInspectorView.hpp"
#include "LightsInspectorView.hpp"
#include "LuminaireInspectorView.hpp"
#include "ElectricEquipmentInspectorView.hpp"
#include "GasEquipmentInspectorView.hpp"
#include "SteamEquipmentInspectorView.hpp"
#include "OtherEquipmentInspectorView.hpp"
#include "WaterUseEquipmentInspectorView.hpp"
#include "HotWaterEquipmentInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QStringLiteral>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <sstream>
#include <tuple>

#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

LoadsView::LoadsView(bool isIP, const openstudio::model::Model& model, QWidget* parent)
  : ModelSubTabView(
      new ModelObjectTypeListView(LoadsView::modelObjectTypesNamesAndUrls(), model, true, OSItemType::CollapsibleListHeader, false, parent),
      new LoadsInspectorView(isIP, model, parent), false, parent) {
  connect(this, &LoadsView::toggleUnitsClicked, modelObjectInspectorView(), &ModelObjectInspectorView::toggleUnitsClicked);
}

std::vector<std::tuple<IddObjectType, QString, QString>> LoadsView::modelObjectTypesNamesAndUrls() {
  using T = std::tuple<IddObjectType, QString, QString>;
  return {
    T{IddObjectType::OS_People_Definition, tr("People Definitions"), iddObjectDocUrl(QStringLiteral("OS:People"))},
    T{IddObjectType::OS_Lights_Definition, tr("Lights Definitions"), iddObjectDocUrl(QStringLiteral("OS:Lights"))},
    T{IddObjectType::OS_Luminaire_Definition, tr("Luminaire Definitions"), {}},
    T{IddObjectType::OS_ElectricEquipment_Definition, tr("Electric Equipment Definitions"), iddObjectDocUrl(QStringLiteral("OS:ElectricEquipment"))},
    T{IddObjectType::OS_GasEquipment_Definition, tr("Gas Equipment Definitions"), iddObjectDocUrl(QStringLiteral("OS:GasEquipment"))},
    T{IddObjectType::OS_SteamEquipment_Definition, tr("Steam Equipment Definitions"), iddObjectDocUrl(QStringLiteral("OS:SteamEquipment"))},
    T{IddObjectType::OS_OtherEquipment_Definition, tr("Other Equipment Definitions"), iddObjectDocUrl(QStringLiteral("OS:OtherEquipment"))},
    T{IddObjectType::OS_InternalMass_Definition, tr("Internal Mass Definitions"), iddObjectDocUrl(QStringLiteral("OS:InternalMass"))},
    T{IddObjectType::OS_WaterUse_Equipment_Definition, tr("Water Use Equipment Definitions"), iddObjectDocUrl(QStringLiteral("OS:WaterUse:Equipment"))},
    T{IddObjectType::OS_HotWaterEquipment_Definition, tr("Hot Water Equipment Definitions"), iddObjectDocUrl(QStringLiteral("OS:HotWaterEquipment"))},
  };
}

void LoadsView::toggleUnits(bool displayIP) {}

LoadsInspectorView::LoadsInspectorView(bool isIP, const model::Model& model, QWidget* parent) : ModelObjectInspectorView(model, false, parent) {
  m_isIP = isIP;

  this->showDefaultView();
}

void LoadsInspectorView::onClearSelection() {
  this->showDefaultView();
}

void LoadsInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  switch (modelObject.iddObjectType().value()) {
    case IddObjectType::OS_People_Definition:
      this->showPeopleDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_Lights_Definition:
      this->showLightsDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_Luminaire_Definition:
      this->showLuminaireDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_ElectricEquipment_Definition:
      this->showElectricEquipmentDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_GasEquipment_Definition:
      this->showGasEquipmentDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_SteamEquipment_Definition:
      this->showSteamEquipmentDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_OtherEquipment_Definition:
      this->showOtherEquipmentDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_InternalMass_Definition:
      this->showInternalMassDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_WaterUse_Equipment_Definition:
      this->showWaterUseEquipmentDefinitionsInspector(modelObject);
      break;
    case IddObjectType::OS_HotWaterEquipment_Definition:
      this->showHotWaterEquipmentDefinitionsInspector(modelObject);
      break;
    default:
      showDefaultView();
  }
}

void LoadsInspectorView::onUpdate() {}

void LoadsInspectorView::showInspector(QWidget* widget) {
  if (QWidget* _widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(_widget);

    delete _widget;
  }

  this->stackedWidget()->addWidget(widget);
}

void LoadsInspectorView::showPeopleDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* peopleDefinitionInspectorView = new PeopleDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, peopleDefinitionInspectorView, &PeopleDefinitionInspectorView::toggleUnitsClicked);

  peopleDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(peopleDefinitionInspectorView);
}

void LoadsInspectorView::showLightsDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* lightsDefinitionInspectorView = new LightsDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, lightsDefinitionInspectorView, &LightsDefinitionInspectorView::toggleUnitsClicked);

  lightsDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(lightsDefinitionInspectorView);
}

void LoadsInspectorView::showLuminaireDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* luminaireDefinitionInspectorView = new LuminaireDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, luminaireDefinitionInspectorView, &LuminaireDefinitionInspectorView::toggleUnitsClicked);

  luminaireDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(luminaireDefinitionInspectorView);
}

void LoadsInspectorView::showElectricEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* electricEquipmentDefinitionInspectorView = new ElectricEquipmentDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, electricEquipmentDefinitionInspectorView,
          &ElectricEquipmentDefinitionInspectorView::toggleUnitsClicked);

  electricEquipmentDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(electricEquipmentDefinitionInspectorView);
}

void LoadsInspectorView::showGasEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* gasEquipmentDefinitionInspectorView = new GasEquipmentDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, gasEquipmentDefinitionInspectorView,
          &GasEquipmentDefinitionInspectorView::toggleUnitsClicked);

  gasEquipmentDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(gasEquipmentDefinitionInspectorView);
}

void LoadsInspectorView::showSteamEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* steamEquipmentDefinitionInspectorView = new SteamEquipmentDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, steamEquipmentDefinitionInspectorView,
          &SteamEquipmentDefinitionInspectorView::toggleUnitsClicked);

  steamEquipmentDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(steamEquipmentDefinitionInspectorView);
}

void LoadsInspectorView::showOtherEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* otherEquipmentDefinitionInspectorView = new OtherEquipmentDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, otherEquipmentDefinitionInspectorView,
          &OtherEquipmentDefinitionInspectorView::toggleUnitsClicked);

  otherEquipmentDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(otherEquipmentDefinitionInspectorView);
}

void LoadsInspectorView::showWaterUseEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* waterUseEquipmentDefinitionInspectorView = new WaterUseEquipmentDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, waterUseEquipmentDefinitionInspectorView,
          &WaterUseEquipmentDefinitionInspectorView::toggleUnitsClicked);

  waterUseEquipmentDefinitionInspectorView->selectModelObject(modelObject);

  showInspector(waterUseEquipmentDefinitionInspectorView);
}

void LoadsInspectorView::showInternalMassDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* internalMassDefinitionInspectorView = new InternalMassDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, internalMassDefinitionInspectorView,
          &InternalMassDefinitionInspectorView::toggleUnitsClicked);

  internalMassDefinitionInspectorView->selectModelObject(modelObject);

  this->showInspector(internalMassDefinitionInspectorView);
}

void LoadsInspectorView::showHotWaterEquipmentDefinitionsInspector(const openstudio::model::ModelObject& modelObject) {
  auto* hotWaterEquipmentDefinitionInspectorView = new HotWaterEquipmentDefinitionInspectorView(m_isIP, m_model);
  connect(this, &LoadsInspectorView::toggleUnitsClicked, hotWaterEquipmentDefinitionInspectorView,
          &HotWaterEquipmentDefinitionInspectorView::toggleUnitsClicked);

  hotWaterEquipmentDefinitionInspectorView->selectModelObject(modelObject);

  showInspector(hotWaterEquipmentDefinitionInspectorView);
}

void LoadsInspectorView::showDefaultView() {
  if (QWidget* widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(widget);

    delete widget;
  }
}

void LoadsInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
