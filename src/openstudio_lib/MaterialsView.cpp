/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "MaterialsView.hpp"

#include "MaterialAirGapInspectorView.hpp"
#include "MaterialInfraredTransparentInspectorView.hpp"
#include "MaterialInspectorView.hpp"
#include "MaterialNoMassInspectorView.hpp"
#include "MaterialRoofVegetationInspectorView.hpp"
#include "ModelObjectTypeListView.hpp"
#include "WindowMaterialBlindInspectorView.hpp"
#include "WindowMaterialDaylightRedirectionDeviceInspectorView.hpp"
#include "WindowMaterialGasInspectorView.hpp"
#include "WindowMaterialGasMixtureInspectorView.hpp"
#include "WindowMaterialGlazingGroupThermochromicInspectorView.hpp"
#include "WindowMaterialGlazingInspectorView.hpp"
#include "WindowMaterialGlazingRefractionExtinctionMethodInspectorView.hpp"
#include "WindowMaterialScreenInspectorView.hpp"
#include "WindowMaterialShadeInspectorView.hpp"
#include "WindowMaterialSimpleGlazingSystemInspectorView.hpp"

#include <openstudio/model/Model_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <openstudio/utilities/idd/IddEnums.hxx>

#include <QStackedWidget>
#include <QStringLiteral>
#include <tuple>

namespace openstudio {

MaterialsView::MaterialsView(bool isIP, const openstudio::model::Model& model, const QString& tabLabel, bool hasSubTabs, QWidget* parent)
  : ModelSubTabView(
      new ModelObjectTypeListView(MaterialsView::modelObjectTypesNamesAndUrls(), model, true, OSItemType::CollapsibleListHeader, false, parent),
      new MaterialsInspectorView(isIP, model, parent), false, parent) {
  // ModelObjectTypeListView will call reportItems for each IddObjectType, this results in inspector being build for each IddObjecType then thrown away
  connect(this, &MaterialsView::toggleUnitsClicked, modelObjectInspectorView(), &ModelObjectInspectorView::toggleUnitsClicked);
}

std::vector<std::tuple<IddObjectType, QString, QString>> MaterialsView::modelObjectTypesNamesAndUrls() {
  static const QString base = QStringLiteral(BIGLADDERSOFTWARE_DOC_BASE_URL);
  static const QString sce = base + QStringLiteral("group-surface-construction-elements.html");

  using T = std::tuple<IddObjectType, QString, QString>;
  return {
    T{IddObjectType::OS_Material, tr("Materials"), sce + "#material"},
    T{IddObjectType::OS_Material_NoMass, tr("No Mass Materials"), sce + "#materialnomass"},
    T{IddObjectType::OS_Material_AirGap, tr("Air Gap Materials"), sce + "#materialairgap"},
    T{IddObjectType::OS_WindowMaterial_SimpleGlazingSystem, tr("Simple Glazing System Window Materials"),
      sce + "#windowmaterialsimpleglazingsystem"},
    T{IddObjectType::OS_WindowMaterial_Glazing, tr("Glazing Window Materials"), sce + "#windowmaterialglazing"},
    T{IddObjectType::OS_WindowMaterial_Gas, tr("Gas Window Materials"), sce + "#windowmaterialgas"},
    T{IddObjectType::OS_WindowMaterial_GasMixture, tr("Gas Mixture Window Materials"), sce + "#windowmaterialgasmixture"},
    T{IddObjectType::OS_WindowMaterial_Blind, tr("Blind Window Materials"), sce + "#windowmaterialblind"},
    T{IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice, tr("Daylight Redirection Device Window Materials"),
      sce + "#windowmaterialblind"},
    T{IddObjectType::OS_WindowMaterial_Screen, tr("Screen Window Materials"), sce + "#windowmaterialscreen"},
    T{IddObjectType::OS_WindowMaterial_Shade, tr("Shade Window Materials"), sce + "#windowmaterialshade"},
    // Oddballs at the bottom
    T{IddObjectType::OS_Material_InfraredTransparent, tr("Infrared Transparent Materials"), sce + "#materialinfraredtransparent"},
    T{IddObjectType::OS_Material_RoofVegetation, tr("Roof Vegetation Materials"), sce + "#materialroofvegetation"},
    T{IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod, tr("Refraction Extinction Method Glazing Window Materials"),
      sce + "#windowmaterialglazingrefractionextinctionmethod"},
    // TODO: commented out until ThermochromicGlazing is properly wrapped
    // T{IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic, tr("Glazing Group Thermochromic Window Materials"), sce + "#windowmaterialglazinggroupthermochromic"},
  };
}

MaterialsInspectorView::MaterialsInspectorView(bool isIP, const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, false, parent), m_isIP(isIP) {
  //// Hack code to remove when tab active
  //QLabel * underConstructionLabel = new QLabel();
  //underConstructionLabel->setPixmap(QPixmap(":/images/coming_soon_building_summary.png"));
  //underConstructionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  //int index = this->stackedWidget()->addWidget(underConstructionLabel);
  //OS_ASSERT(index == 0);

  //// index of hidden widget is 0
  ////QWidget* hiddenWidget = new QWidget();
  ////int index = this->stackedWidget()->addWidget(hiddenWidget);
  ////OS_ASSERT(index == 0);

  //// index of the default is 1
  //DefaultInspectorView* defaultInspectorView = new DefaultInspectorView(model, parent);
  //index = this->stackedWidget()->addWidget(defaultInspectorView);
  //OS_ASSERT(index == 1);

  ////StandardOpaqueMaterialInspectorView* standardOpaqueMaterialInspectorView = new StandardOpaqueMaterialInspectorView(model, parent);
  ////index = this->stackedWidget()->addWidget(standardOpaqueMaterialInspectorView);
  ////m_inspectorIndexMap[IddObjectType::OS_Material] = index;
}

void MaterialsInspectorView::onClearSelection() {
  QWidget* widget = this->stackedWidget()->currentWidget();
  auto* modelObjectInspectorView = qobject_cast<ModelObjectInspectorView*>(widget);
  if (modelObjectInspectorView) {
    modelObjectInspectorView->clearSelection();
  }

  this->stackedWidget()->setCurrentIndex(0);
}

void MaterialsInspectorView::onUpdate() {}

void MaterialsInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  switch (modelObject.iddObjectType().value()) {
    case IddObjectType::OS_Material:
      this->showMaterialInspectorView(modelObject);
      break;
    case IddObjectType::OS_Material_AirGap:
      this->showMaterialAirGapInspectorView(modelObject);
      break;
    case IddObjectType::OS_Material_InfraredTransparent:
      this->showMaterialInfraredTransparentInspectorView(modelObject);
      break;
    case IddObjectType::OS_Material_NoMass:
      this->showMaterialNoMassInspectorView(modelObject);
      break;
    case IddObjectType::OS_Material_RoofVegetation:
      this->showMaterialRoofVegetationInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_Blind:
      this->showWindowMaterialBlindInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_DaylightRedirectionDevice:
      this->showWindowMaterialDaylightRedirectionDeviceInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_Gas:
      this->showWindowMaterialGasInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_GasMixture:
      this->showWindowMaterialGasMixtureInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_Glazing:
      this->showWindowMaterialGlazingInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_Glazing_RefractionExtinctionMethod:
      this->showWindowMaterialGlazingRefractionExtinctionMethodInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_GlazingGroup_Thermochromic:
      this->showWindowMaterialGlazingGroupThermochromicInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_Screen:
      this->showWindowMaterialScreenInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_Shade:
      this->showWindowMaterialShadeInspectorView(modelObject);
      break;
    case IddObjectType::OS_WindowMaterial_SimpleGlazingSystem:
      this->showWindowMaterialSimpleGlazingSystemInspectorView(modelObject);
      break;
    default:
      showDefaultView();
  }
}

void MaterialsInspectorView::showMaterialAirGapInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new MaterialAirGapInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &MaterialAirGapInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showMaterialInfraredTransparentInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new MaterialInfraredTransparentInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &MaterialInfraredTransparentInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showMaterialInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new MaterialInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &MaterialInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showMaterialNoMassInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new MaterialNoMassInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &MaterialNoMassInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showMaterialRoofVegetationInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new MaterialRoofVegetationInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &MaterialRoofVegetationInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialBlindInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialBlindInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialBlindInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialDaylightRedirectionDeviceInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialDaylightRedirectionDeviceInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialDaylightRedirectionDeviceInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialGasInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialGasInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialGasInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialGasMixtureInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialGasMixtureInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialGasMixtureInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialGlazingGroupThermochromicInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialGlazingGroupThermochromicInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialGlazingGroupThermochromicInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialGlazingInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialGlazingInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialGlazingInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialGlazingRefractionExtinctionMethodInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialGlazingRefractionExtinctionMethodInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialGlazingRefractionExtinctionMethodInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialScreenInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialScreenInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialScreenInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialShadeInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialShadeInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialShadeInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showWindowMaterialSimpleGlazingSystemInspectorView(const openstudio::model::ModelObject& modelObject) {
  auto* view = new WindowMaterialSimpleGlazingSystemInspectorView(m_isIP, m_model);
  connect(this, &MaterialsInspectorView::toggleUnitsClicked, view, &WindowMaterialSimpleGlazingSystemInspectorView::toggleUnitsClicked);

  view->selectModelObject(modelObject);

  this->showInspector(view);
}

void MaterialsInspectorView::showInspector(QWidget* widget) {
  if (QWidget* _widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(_widget);

    delete _widget;
  }

  this->stackedWidget()->addWidget(widget);
}

void MaterialsInspectorView::showDefaultView() {
  if (QWidget* widget = this->stackedWidget()->currentWidget()) {
    this->stackedWidget()->removeWidget(widget);

    delete widget;
  }
}

/****************** SLOTS ******************/

void MaterialsInspectorView::toggleUnits(bool displayIP) {
  m_isIP = displayIP;
}

}  // namespace openstudio
