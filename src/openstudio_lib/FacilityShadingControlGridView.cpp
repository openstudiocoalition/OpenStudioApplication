/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2008-2019, Alliance for Sustainable Energy, LLC, and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include "FacilityShadingControlGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSGridView.hpp"
#include "model_editor/Utilities.hpp"

#include <openstudio/model/ShadingControl.hpp>
#include <openstudio/model/ShadingControl_Impl.hpp>
#include <openstudio/model/Blind.hpp>
#include <openstudio/model/Blind_Impl.hpp>
#include <openstudio/model/ConstructionBase.hpp>
#include <openstudio/model/ConstructionBase_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/Schedule.hpp>
#include <openstudio/model/Schedule_Impl.hpp>
#include <openstudio/model/SubSurface.hpp>
#include <openstudio/model/SubSurface_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_ShadingControl_FieldEnums.hxx>
#include <openstudio/utilities/idd/OS_WindowMaterial_Blind_FieldEnums.hxx>
#include <openstudio/utilities/units/QuantityConverter.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

// ALL GRID BUTTONS
#define NAME "Shading Control Name"
#define SELECTED "All"

// GENERAL "General"
#define SHADINGTYPE "Shading Type"
#define CONSTRUCTIONWITHSHADINGNAME "Construction with Shading Name"
#define SHADINGDEVICEMATERIALNAME "Shading Device Material Name"
#define SHADINGCONTROLTYPE "Shading Control Type"
#define SCHEDULENAME "Schedule Name"
#define SETPOINT "Setpoint"
#define SHADINGCONTROLISSCHEDULED "Shading Control Is Scheduled"
#define GLARECONTROLISACTIVE "Glare Control Is Active"
#define TYPEOFSLATANGLECONTROLFORBLINDS "Type of Slat Angle Control for Blinds"
#define SLATANGLESCHEDULENAME "Slat Angle Schedule Name"
#define SETPOINT2 "Setpoint 2"
#define MULTIPLESURFACECONTROLTYPE "Multiple Surface Control Type"

// FILTERS
// NAME
// SHADINGTYPE
// SHADINGCONTROLTYPE
#define SUBSURFACENAME "SubSurface Name"
// #define SUBSURFACETYPE "SubSurface Type"

namespace openstudio {

FacilityShadingControlGridView::FacilityShadingControlGridView(bool isIP, const model::Model& model, QWidget* parent)
  : GridViewSubTab(isIP, model, parent) {
  std::vector<model::ShadingControl> shadingControls = model.getConcreteModelObjects<model::ShadingControl>();
  auto modelObjects = subsetCastVector<model::ModelObject>(shadingControls);
  std::sort(modelObjects.begin(), modelObjects.end(), openstudio::WorkspaceObjectNameLess());

  m_gridController = new FacilityShadingControlGridController(isIP, "Shading Control", IddObjectType::OS_ShadingControl, model, modelObjects);
  m_gridView = new OSGridView(m_gridController, "Shading Control", "Drop\nShading Control", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  // Filters

  QLabel* label = nullptr;

  QVBoxLayout* layout = nullptr;

  auto filterGridLayout = new QGridLayout();
  filterGridLayout->setContentsMargins(7, 4, 0, 8);
  filterGridLayout->setSpacing(5);

  label = new QLabel();
  label->setText("Filters:");
  label->setObjectName("H2");
  filterGridLayout->addWidget(label, filterGridLayout->rowCount(), filterGridLayout->columnCount(), Qt::AlignTop | Qt::AlignLeft);

  // SHADINGCONTROLNAME

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(NAME);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_nameFilter = new QLineEdit();
  m_nameFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  // Evan note: there are issues with using the signal textChanged or textEdited, related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  connect(m_nameFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingControlGridView::nameFilterChanged);

  QRegExp nameRegex("^\\S.*");
  auto nameValidator = new QRegExpValidator(nameRegex, this);
  m_nameFilter->setValidator(nameValidator);

  layout->addWidget(m_nameFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // SHADINGTYPE

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(SHADINGTYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_shadingTypeFilter = new QComboBox();
  m_shadingTypeFilter->addItem("All");
  for (const auto& t : model::ShadingControl::shadingTypeValues()) {
    m_shadingTypeFilter->addItem(openstudio::toQString(t));
  }
  m_shadingTypeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_shadingTypeFilter, &QComboBox::currentTextChanged, this, &openstudio::FacilityShadingControlGridView::shadingTypeFilterChanged);

  layout->addWidget(m_shadingTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // SHADINGCONTROLTYPE
  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(SHADINGCONTROLTYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_shadingControlTypeFilter = new QComboBox();
  m_shadingControlTypeFilter->addItem("All");
  for (const auto& t : model::ShadingControl::shadingControlTypeValues()) {
    m_shadingControlTypeFilter->addItem(openstudio::toQString(t));
  }
  m_shadingControlTypeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_shadingControlTypeFilter, &QComboBox::currentTextChanged, this,
          &openstudio::FacilityShadingControlGridView::shadingControlTypeFilterChanged);

  layout->addWidget(m_shadingControlTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // SUBSURFACENAME
  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(SUBSURFACENAME);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_subSurfaceNameFilter = new QLineEdit();
  m_subSurfaceNameFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  // Evan note: there are issues with using the signal textChanged or textEdited, related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  connect(m_subSurfaceNameFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingControlGridView::subSurfaceNameFilterChanged);

  m_subSurfaceNameFilter->setValidator(nameValidator);

  layout->addWidget(m_subSurfaceNameFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // SUBSURFACETYPE
  //layout = new QVBoxLayout();

  //label = new QLabel();
  //label->setText(SUBSURFACETYPE);
  //label->setObjectName("H3");
  //layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  //m_subSurfaceTypeFilter = new QComboBox();
  //m_subSurfaceTypeFilter->addItem("All");
  //for (const auto& t: model::SubSurface::validSubSurfaceTypeValues()) {
  //m_subSurfaceTypeFilter->addItem(openstudio::toQString(t));
  //}
  //m_subSurfaceTypeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  //connect(m_subSurfaceTypeFilter, &QComboBox::currentTextChanged, this, &openstudio::FacilityShadingControlGridView::subSurfaceTypeFilterChanged);

  //layout->addWidget(m_subSurfaceTypeFilter, Qt::AlignTop | Qt::AlignLeft);
  //layout->addStretch();
  //filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  filterGridLayout->setRowStretch(filterGridLayout->rowCount(), 100);
  filterGridLayout->setColumnStretch(filterGridLayout->columnCount(), 100);

  m_gridView->m_contentLayout->addLayout(filterGridLayout);

  m_gridView->m_contentLayout->addSpacing(7);
}

void FacilityShadingControlGridView::nameFilterChanged() {
  m_objectsFilteredByName.clear();

  if (m_nameFilter->text().isEmpty()) {
    // nothing to filter
  } else {
    for (auto obj : this->m_gridController->getObjectSelector()->m_selectorObjects) {
      QString objName(obj.name().get().c_str());
      if (!objName.contains(m_nameFilter->text(), Qt::CaseInsensitive)) {
        m_objectsFilteredByName.insert(obj);
      }
    }
  }

  filterChanged();
}

void FacilityShadingControlGridView::shadingTypeFilterChanged(const QString& text) {
  m_objectsFilteredByShadingType.clear();
  if (m_shadingTypeFilter->currentText() == "All") {
    // Nothing to filter
  } else {
    for (auto obj : this->m_gridController->getObjectSelector()->m_selectorObjects) {
      auto shadingType = obj.cast<model::ShadingControl>().shadingType();
      if (!openstudio::istringEqual(openstudio::toString(text), shadingType)) {
        m_objectsFilteredByShadingType.insert(obj);
      }
    }
  }
  filterChanged();
}

void FacilityShadingControlGridView::shadingControlTypeFilterChanged(const QString& text) {
  m_objectsFilteredByShadingControlType.clear();
  if (m_shadingControlTypeFilter->currentText() == "All") {
    // Nothing to filter
  } else {
    for (auto obj : this->m_gridController->getObjectSelector()->m_selectorObjects) {
      auto shadingControlType = obj.cast<model::ShadingControl>().shadingControlType();
      if (!openstudio::istringEqual(openstudio::toString(text), shadingControlType)) {
        m_objectsFilteredByShadingControlType.insert(obj);
      }
    }
  }
  filterChanged();
}

void FacilityShadingControlGridView::subSurfaceNameFilterChanged() {
  m_objectsFilteredBySubSurfaceName.clear();

  if (m_subSurfaceNameFilter->text().isEmpty()) {
    // nothing to filter
  } else {
    for (auto obj : this->m_gridController->getObjectSelector()->m_selectorObjects) {
      bool found = false;
      for (const auto subSurface : obj.cast<model::ShadingControl>().subSurfaces()) {
        QString objName(subSurface.name().get().c_str());
        if (objName.contains(m_subSurfaceNameFilter->text(), Qt::CaseInsensitive)) {
          found = true;
          break;
        }
      }
      if (!found) {
        m_objectsFilteredByName.insert(obj);
      }
    }
  }

  filterChanged();
}

void FacilityShadingControlGridView::filterChanged() {
  std::set<openstudio::model::ModelObject> allFilteredObjects = m_objectsFilteredByName;

  allFilteredObjects.insert(m_objectsFilteredByShadingType.begin(), m_objectsFilteredByShadingType.end());
  allFilteredObjects.insert(m_objectsFilteredByShadingControlType.begin(), m_objectsFilteredByShadingControlType.end());
  allFilteredObjects.insert(m_objectsFilteredBySubSurfaceName.begin(), m_objectsFilteredBySubSurfaceName.end());

  this->m_gridController->getObjectSelector()->m_filteredObjects = allFilteredObjects;

  this->m_gridView->requestRefreshAll();

  onClearSelection();
}

void FacilityShadingControlGridView::addObject(const IddObjectType& iddObjectType) {
  if (IddObjectType::OS_ShadingControl == iddObjectType.value()) {
    model::Blind b(this->m_model);
    model::ShadingControl{b};
  }
}

void FacilityShadingControlGridView::purgeObjects(const IddObjectType& iddObjectType) {
  // If no SubSurfaces in the Shading Control -> remove
  for (auto mo : this->m_model.getConcreteModelObjects<model::ShadingControl>()) {
    if (mo.subSurfaces().empty()) {
      mo.remove();
    }
  }
}

void FacilityShadingControlGridView::onSelectItem() {
  m_itemSelectorButtons->enableAddButton();
  m_itemSelectorButtons->enableCopyButton();
  m_itemSelectorButtons->enableRemoveButton();
  m_itemSelectorButtons->enablePurgeButton();
}

void FacilityShadingControlGridView::onClearSelection() {
  //m_itemSelectorButtons->disableAddButton();
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  //m_itemSelectorButtons->disablePurgeButton();
}

FacilityShadingControlGridController::FacilityShadingControlGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType,
                                                                           const model::Model& model,
                                                                           const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects) {
  setCategoriesAndFields();
}

void FacilityShadingControlGridController::setCategoriesAndFields() {

  // GENERAL
  {
    std::vector<QString> fields;
    fields.push_back(NAME);
    fields.push_back(SHADINGTYPE);
    //fields.push_back(CONSTRUCTIONWITHSHADINGNAME);
    //fields.push_back(SHADINGDEVICEMATERIALNAME);
    fields.push_back(SHADINGCONTROLTYPE);
    fields.push_back(SCHEDULENAME);
    fields.push_back(SETPOINT);
    //fields.push_back(SHADINGCONTROLISSCHEDULED);       IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
    //fields.push_back(GLARECONTROLISACTIVE);            IN IDD, BUT NOT EXPOSED IN MODEL OBJECT
    fields.push_back(TYPEOFSLATANGLECONTROLFORBLINDS);
    fields.push_back(SLATANGLESCHEDULENAME);
    fields.push_back(MULTIPLESURFACECONTROLTYPE);
    fields.push_back(SETPOINT2);
    fields.push_back(SUBSURFACENAME);
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    m_categoriesAndFields.push_back(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void FacilityShadingControlGridController::categorySelected(int index) {
  OSGridController::categorySelected(index);
}

void FacilityShadingControlGridController::addColumns(const QString& category, std::vector<QString>& fields) {
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  m_baseConcepts.clear();

  for (const auto& field : fields) {

    if (field == NAME) {
      addNameLineEditColumn(Heading(QString(NAME), false, false), false, false, CastNullAdapter<model::ShadingControl>(&model::ShadingControl::name),
                            CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setName));
    } else if (field == SELECTED) {
      auto checkbox = QSharedPointer<QCheckBox>(new QCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &QCheckBox::stateChanged, this, &FacilityShadingControlGridController::selectAllStateChanged);
      connect(checkbox.data(), &QCheckBox::stateChanged, this->gridView(), &OSGridView::gridRowSelectionChanged);

      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");

    } else if (field == SHADINGTYPE) {
      addComboBoxColumn<std::string, model::ShadingControl>(Heading(QString(SHADINGTYPE)),
                                                            static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
                                                            std::function<std::vector<std::string>()>(&model::ShadingControl::shadingTypeValues),
                                                            CastNullAdapter<model::ShadingControl>(&model::ShadingControl::shadingType),
                                                            CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setShadingType));

    } else if (field == SHADINGCONTROLTYPE) {
      // TODO: this field will update depending fields, like clearing the Setpoint / Setpoint2 / ScheduleName
      // So when value changes, we should trigger a row refresh
      addComboBoxColumn<std::string, model::ShadingControl>(
        Heading(QString(SHADINGTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::ShadingControl::shadingControlTypeValues),
        CastNullAdapter<model::ShadingControl>(&model::ShadingControl::shadingControlType),
        CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setShadingControlType));

    } else if (field == MULTIPLESURFACECONTROLTYPE) {
      addComboBoxColumn<std::string, model::ShadingControl>(
        Heading(QString(SHADINGTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::ShadingControl::multipleSurfaceControlTypeValues),
        CastNullAdapter<model::ShadingControl>(&model::ShadingControl::multipleSurfaceControlType),
        CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setMultipleSurfaceControlType));

    } else if (field == TYPEOFSLATANGLECONTROLFORBLINDS) {

      addComboBoxColumn<std::string, model::ShadingControl>(
        Heading(QString(SHADINGTYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString),
        std::function<std::vector<std::string>()>(&model::ShadingControl::typeofSlatAngleControlforBlindsValues),
        CastNullAdapter<model::ShadingControl>(&model::ShadingControl::typeofSlatAngleControlforBlinds),
        CastNullAdapter<model::ShadingControl>(&model::ShadingControl::setTypeofSlatAngleControlforBlinds));

    } else if (field == SCHEDULENAME) {
      std::function<bool(model::ShadingControl*, const model::Schedule&)> setter(
        [](model::ShadingControl* sc, const model::Schedule& s) { return sc->setSchedule(s); });

      addDropZoneColumn(
        Heading(QString(SCHEDULENAME)), CastNullAdapter<model::ShadingControl>(&model::ShadingControl::schedule), setter,
        boost::optional<std::function<void(model::ShadingControl*)>>(CastNullAdapter<model::ShadingControl>(&model::ShadingControl::resetSchedule)));

    } else if (field == SLATANGLESCHEDULENAME) {
      std::function<bool(model::ShadingControl*, const model::Schedule&)> setter(
        [](model::ShadingControl* sc, const model::Schedule& s) { return sc->setSlatAngleSchedule(s); });

      addDropZoneColumn(Heading(QString(SLATANGLESCHEDULENAME)), CastNullAdapter<model::ShadingControl>(&model::ShadingControl::slatAngleSchedule),
                        setter,
                        boost::optional<std::function<void(model::ShadingControl*)>>(
                          CastNullAdapter<model::ShadingControl>(&model::ShadingControl::resetSlatAngleSchedule)));

    } else if (field == CONSTRUCTIONWITHSHADINGNAME) {
    } else if (field == SHADINGDEVICEMATERIALNAME) {

    } else if (field == SHADINGCONTROLISSCHEDULED) {

    } else if (field == GLARECONTROLISACTIVE) {

    } else if (field == SETPOINT) {
      // This field is very poorly implemented in the SDK, since the getter returns an optional, BUT it has a isDefaulted method due to the fact that a default
      // is provided by only one of the Shading Control Types... So it doesn't match any addValueEditColumn prototypes that takes isDefaulted
      addValueEditColumn(Heading(QString(SETPOINT)), NullAdapter(&model::ShadingControl::setpoint), NullAdapter(&model::ShadingControl::setSetpoint),
                         boost::optional<DataSource>());

    } else if (field == SETPOINT2) {
      addValueEditColumn(Heading(QString(SETPOINT2)), NullAdapter(&model::ShadingControl::setpoint2),
                         NullAdapter(&model::ShadingControl::setSetpoint2), boost::optional<DataSource>());

    } else {

      if (field == SUBSURFACENAME) {

        // ValueType = model::SubSurface
        // DataSourceType = model::ShadingControl

        std::function<boost::optional<model::ModelObject>(model::ShadingControl*)> getter;

        std::function<bool(model::ShadingControl*, const model::ModelObject&)> setter(
          [](model::ShadingControl* t_shadingControl, const model::ModelObject& t_mo) {
            if (auto ss = t_mo.optionalCast<model::SubSurface>()) {
              return t_shadingControl->addSubSurface(ss.get());
            }
            return false;
          });

        std::function<void(model::ShadingControl*)> reset;
        std::function<std::vector<model::ModelObject>(const model::ShadingControl&)> subSurfaces([](const model::ShadingControl& t_shadingControl) {
          auto subSurfaces = t_shadingControl.subSurfaces();
          auto allModelObjects = subsetCastVector<model::ModelObject>(subSurfaces);
          return allModelObjects;
        });

        addNameLineEditColumn(
          Heading(QString(SUBSURFACENAME)), true, false,  // Is Inspectable, but not Deletable
          CastNullAdapter<model::ModelObject>(&model::ModelObject::name), CastNullAdapter<model::ModelObject>(&model::ModelObject::setName),
          boost::optional<std::function<void(model::ModelObject*)>>(std::function<void(model::ModelObject*)>([](model::ModelObject* t_mo) {
            if (auto ss = t_mo->optionalCast<model::SubSurface>()) {
              // TODO: Ideally we should remove the subSurface only from the current Shading Control, but I don't see how...
              ss->removeAllShadingControls();
            }
          })),
          DataSource(subSurfaces, false,
                     QSharedPointer<DropZoneConcept>(
                       new DropZoneConceptImpl<model::ModelObject, model::ShadingControl>(Heading(SUBSURFACENAME), getter, setter, reset))));

      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString FacilityShadingControlGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void FacilityShadingControlGridController::checkSelectedFields() {
  if (!this->m_hasHorizontalHeader) return;

  OSGridController::checkSelectedFields();
}

void FacilityShadingControlGridController::onItemDropped(const OSItemId& itemId) {}

void FacilityShadingControlGridController::refreshModelObjects() {
  std::vector<model::ShadingControl> shadingControls = m_model.getConcreteModelObjects<model::ShadingControl>();
  m_modelObjects = subsetCastVector<model::ModelObject>(shadingControls);

  // Sort them
  std::sort(m_modelObjects.begin(), m_modelObjects.end(), openstudio::WorkspaceObjectNameLess());
}

}  // namespace openstudio
