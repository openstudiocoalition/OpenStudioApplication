/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
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

#include "FacilityShadingGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

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
#include <openstudio/model/ShadingSurface.hpp>
#include <openstudio/model/ShadingSurface_Impl.hpp>
#include <openstudio/model/ShadingSurfaceGroup.hpp>
#include <openstudio/model/ShadingSurfaceGroup_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_ShadingSurface_FieldEnums.hxx>
#include <openstudio/utilities/idd/OS_ShadingSurfaceGroup_FieldEnums.hxx>
#include <openstudio/utilities/idd/OS_WindowMaterial_Blind_FieldEnums.hxx>
#include <openstudio/utilities/units/QuantityConverter.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Shading Surface Group Name"
#define SELECTED "All"
#define DISPLAYNAME "Display Name"
#define CADOBJECTID "CAD Object ID"

// GENERAL
#define TYPE "Type"                                // read only
#define SHADINGSURFACENAME "Shading Surface Name"  // read only
#define CONSTRUCTIONNAME "Construction Name"
#define TRANSMITTANCESCHEDULENAME "Transmittance Schedule Name"

// FILTERS
#define SHADINGSURFACETYPE "Shading Surface Type"
#define SHADINGSURFACEGROUPNAME "Shading Surface Group Name"
#define ORIENTATIONGREATERTHAN "Degrees Orientation >"
#define ORIENTATIONLESSTHAN "Degrees Orientation <"
#define TILTGREATERTHAN "Degrees Tilt >"
#define TILTLESSTHAN "Degrees Tilt <"

namespace openstudio {

FacilityShadingGridView::FacilityShadingGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : GridViewSubTab(isIP, displayAdditionalProps, model, parent) {
  std::vector<model::ShadingSurfaceGroup> shadingGroups = model.getConcreteModelObjects<model::ShadingSurfaceGroup>();
  // Filter out the 'Space' shadingSurfaceTypes
  // These are displayed on the Space's "Shading" subtab
  shadingGroups.erase(std::remove_if(shadingGroups.begin(), shadingGroups.end(),
                                     [](const model::ShadingSurfaceGroup& sg) { return sg.shadingSurfaceType() == "Space"; }),
                      shadingGroups.end());

  auto modelObjects = subsetCastVector<model::ModelObject>(shadingGroups);
  std::sort(modelObjects.begin(), modelObjects.end(), openstudio::WorkspaceObjectNameLess());

  m_gridController = new FacilityShadingGridController(isIP, displayAdditionalProps, "Shading Surface Group", IddObjectType::OS_ShadingSurfaceGroup,
                                                       model, modelObjects);
  m_gridView = new OSGridView(m_gridController, "Shading Surface Group", "Drop Shading\nSurface Group", false, parent);

  setGridController(m_gridController);
  setGridView(m_gridView);

  // Filters

  QLabel* label = nullptr;

  QVBoxLayout* layout = nullptr;

  auto* filterGridLayout = new QGridLayout();
  filterGridLayout->setContentsMargins(7, 4, 0, 8);
  filterGridLayout->setSpacing(5);

  label = new QLabel();
  label->setText("Filters:");
  label->setObjectName("H2");
  filterGridLayout->addWidget(label, filterGridLayout->rowCount(), filterGridLayout->columnCount(), Qt::AlignTop | Qt::AlignLeft);

  // SHADINGSURFACEGROUPNAME

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(SHADINGSURFACENAME);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_nameFilter = new QLineEdit();
  m_nameFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  // Evan note: there are issues with using the signal textChanged or textEdited, related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  connect(m_nameFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingGridView::nameFilterChanged);

  QRegularExpression nameRegex("^\\S.*");
  auto* nameValidator = new QRegularExpressionValidator(nameRegex, this);
  m_nameFilter->setValidator(nameValidator);

  layout->addWidget(m_nameFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // SHADINGSURFACETYPE

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(SHADINGSURFACETYPE);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_typeFilter = new QComboBox();
  m_typeFilter->addItem("All");
  m_typeFilter->addItem("Site");
  m_typeFilter->addItem("Building");
  // Space-level shading is on the Space's "Shading" subtab
  //m_typeFilter->addItem("Space");
  m_typeFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_typeFilter, &QComboBox::currentTextChanged, this, &openstudio::FacilityShadingGridView::typeFilterChanged);

  layout->addWidget(m_typeFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // TILTGREATERTHAN

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(TILTGREATERTHAN);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_tiltGreaterThanFilter = new QLineEdit();
  m_tiltGreaterThanFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_tiltGreaterThanFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingGridView::tiltFilterChanged);

  QRegularExpression regex("^(-?\\d*\\.?\\d+)?$");
  auto* validator = new QRegularExpressionValidator(regex, this);
  m_tiltGreaterThanFilter->setValidator(validator);

  layout->addWidget(m_tiltGreaterThanFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  //TILTLESSTHAN

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(TILTLESSTHAN);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_tiltLessThanFilter = new QLineEdit();
  m_tiltLessThanFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  // Evan note: there are issues with using the signal textChanged or textEdited, related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  connect(m_tiltLessThanFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingGridView::tiltFilterChanged);

  validator = new QRegularExpressionValidator(regex, this);
  m_tiltLessThanFilter->setValidator(validator);

  layout->addWidget(m_tiltLessThanFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // ORIENTATIONGREATERTHAN

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(ORIENTATIONGREATERTHAN);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_orientationGreaterThanFilter = new QLineEdit();
  m_orientationGreaterThanFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_orientationGreaterThanFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingGridView::orientationFilterChanged);

  validator = new QRegularExpressionValidator(regex, this);
  m_orientationGreaterThanFilter->setValidator(validator);

  layout->addWidget(m_orientationGreaterThanFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // ORIENTATIONLESSTHAN

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(ORIENTATIONLESSTHAN);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_orientationLessThanFilter = new QLineEdit();
  m_orientationLessThanFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  // Evan note: there are issues with using the signal textChanged or textEdited, related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  connect(m_orientationLessThanFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityShadingGridView::orientationFilterChanged);

  validator = new QRegularExpressionValidator(regex, this);
  m_orientationLessThanFilter->setValidator(validator);

  layout->addWidget(m_orientationLessThanFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  filterGridLayout->setRowStretch(filterGridLayout->rowCount(), 100);
  filterGridLayout->setColumnStretch(filterGridLayout->columnCount(), 100);

  m_gridView->addLayoutToContentLayout(filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);
}

void FacilityShadingGridView::nameFilterChanged() {
  m_objectsFilteredByName.clear();

  if (m_nameFilter->text().isEmpty()) {
    // nothing to filter
  } else {
    for (const auto& obj : this->m_gridController->selectableObjects()) {
      QString objName(obj.name().get().c_str());
      if (!objName.contains(m_nameFilter->text(), Qt::CaseInsensitive)) {
        m_objectsFilteredByName.insert(obj);
      }
    }
  }

  filterChanged();
}

void FacilityShadingGridView::typeFilterChanged(const QString& text) {
  m_objectsFilteredByType.clear();
  if (m_typeFilter->currentText() == "All") {
    // Nothing to filter
  } else {
    for (const auto& obj : this->m_gridController->selectableObjects()) {
      auto parent = obj.parent();
      if (parent && parent->iddObjectType() == IddObjectType::OS_ShadingSurfaceGroup) {
        if (m_typeFilter->currentText() != parent->cast<model::ShadingSurfaceGroup>().shadingSurfaceType().c_str()) {
          m_objectsFilteredByType.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void FacilityShadingGridView::orientationFilterChanged() {
  m_objectsFilteredByOrientation.clear();

  auto upperLimit = std::numeric_limits<double>::max();
  auto lowerLimit = std::numeric_limits<double>::lowest();

  if (!this->m_orientationLessThanFilter->text().isEmpty()) {
    upperLimit = this->m_orientationLessThanFilter->text().toDouble();
  }

  if (!this->m_orientationGreaterThanFilter->text().isEmpty()) {
    lowerLimit = this->m_orientationGreaterThanFilter->text().toDouble();
  }

  auto convertedValue = convert(upperLimit, "deg", "rad");
  OS_ASSERT(convertedValue);
  upperLimit = *convertedValue;

  convertedValue = convert(lowerLimit, "deg", "rad");
  OS_ASSERT(convertedValue);
  lowerLimit = *convertedValue;

  for (const auto& obj : this->m_gridController->selectableObjects()) {
    if (obj.iddObjectType() == IddObjectType::OS_ShadingSurfaceGroup) {
      for (const auto& shadingSurface : obj.cast<model::ShadingSurfaceGroup>().shadingSurfaces()) {
        auto orientation = shadingSurface.azimuth();
        if (orientation >= upperLimit || orientation <= lowerLimit) {
          m_objectsFilteredByOrientation.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void FacilityShadingGridView::tiltFilterChanged() {
  m_objectsFilteredByTilt.clear();

  auto upperLimit = std::numeric_limits<double>::max();
  auto lowerLimit = std::numeric_limits<double>::lowest();

  if (!this->m_tiltLessThanFilter->text().isEmpty()) {
    upperLimit = this->m_tiltLessThanFilter->text().toDouble();
  }

  if (!this->m_tiltGreaterThanFilter->text().isEmpty()) {
    lowerLimit = this->m_tiltGreaterThanFilter->text().toDouble();
  }

  auto convertedValue = convert(upperLimit, "deg", "rad");
  OS_ASSERT(convertedValue);
  upperLimit = *convertedValue;

  convertedValue = convert(lowerLimit, "deg", "rad");
  OS_ASSERT(convertedValue);
  lowerLimit = *convertedValue;

  for (const auto& obj : this->m_gridController->selectableObjects()) {
    if (obj.iddObjectType() == IddObjectType::OS_ShadingSurfaceGroup) {
      for (const auto& shadingSurface : obj.cast<model::ShadingSurfaceGroup>().shadingSurfaces()) {
        auto tilt = shadingSurface.tilt();
        if (tilt >= upperLimit || tilt <= lowerLimit) {
          m_objectsFilteredByOrientation.insert(obj);
        }
      }
    }
  }

  filterChanged();
}

void FacilityShadingGridView::filterChanged() {
  std::set<openstudio::model::ModelObject> allFilteredObjects = m_objectsFilteredByName;

  for (const auto& obj : m_objectsFilteredByTilt) {
    allFilteredObjects.insert(obj);
  }

  for (const auto& obj : m_objectsFilteredByType) {
    allFilteredObjects.insert(obj);
  }

  for (const auto& obj : m_objectsFilteredByOrientation) {
    allFilteredObjects.insert(obj);
  }

  this->m_gridController->setObjectFilter([allFilteredObjects](const model::ModelObject& obj) -> bool {
    // return false if object in allFilteredObjects
    return allFilteredObjects.count(obj) == 0;
  });
}

void FacilityShadingGridView::addObject(const IddObjectType& iddObjectType) {
  if (IddObjectType::OS_ShadingSurfaceGroup == iddObjectType.value()) {
    model::ShadingSurfaceGroup(this->m_model);
  }
}

void FacilityShadingGridView::purgeObjects(const IddObjectType& iddObjectType) {
  // If no shading surfaces in the Shading Surface Group -> remove
  for (auto& mo : this->m_model.getConcreteModelObjects<model::ShadingSurfaceGroup>()) {
    if (mo.shadingSurfaces().empty()) {
      mo.remove();
    }
  }

  // If a shading surface isn't part of a Shading Surface Group, it won't be translated to IDF anyways and should be considered orphaned
  for (auto& mo : this->m_model.getConcreteModelObjects<model::ShadingSurface>()) {
    if (!mo.shadingSurfaceGroup().has_value()) {
      mo.remove();
    }
  }
}

void FacilityShadingGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  //m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void FacilityShadingGridView::onClearSelection() {
  m_itemSelectorButtons->disableAddButton();
  //m_itemSelectorButtons->disableCopyButton();
  //m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

FacilityShadingGridController::FacilityShadingGridController(bool isIP, bool displayAdditionalProps, const QString& headerText,
                                                             IddObjectType iddObjectType, const model::Model& model,
                                                             const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void FacilityShadingGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      SHADINGSURFACENAME,
      TRANSMITTANCESCHEDULENAME,
      CONSTRUCTIONNAME,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void FacilityShadingGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void FacilityShadingGridController::addColumns(const QString& category, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    // We place it after the SHADINGSURFACENAME
    fields.insert(fields.begin(), {DISPLAYNAME, CADOBJECTID});
  }
  // always show name and selected columns
  // show type next to name, since it comes from the groups
  fields.insert(fields.begin(), {NAME, TYPE, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addNameLineEditColumn(Heading(QString(NAME), false, false), false, false,
                            CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::name),
                            CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::setName));
    }
    // Evan note: TODO to correctly use this column we need a new control --
    // a dropzone for spaces, and a combo box with site and building as choices
    else if (field == TYPE) {
      std::function<std::vector<std::string>()> choices([]() {
        std::vector<std::string> choices{
          "Site",
          "Building",
        };  // "Space" };
        return choices;
      });

      addComboBoxColumn<std::string, model::ShadingSurfaceGroup>(
        Heading(QString(TYPE)), static_cast<std::string (*)(const std::string&)>(&openstudio::toString), choices,
        CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::shadingSurfaceType),
        CastNullAdapter<model::ShadingSurfaceGroup>(&model::ShadingSurfaceGroup::setShadingSurfaceType),
        boost::optional<std::function<void(model::ShadingSurfaceGroup*)>>(), boost::optional<std::function<bool(model::ShadingSurfaceGroup*)>>());
    } else {

      std::function<std::vector<model::ModelObject>(const model::ShadingSurfaceGroup&)> allShadingSurfaces(
        [](const model::ShadingSurfaceGroup& t_shadingSurfaceGroup) {
          std::vector<model::ModelObject> allModelObjects;
          // Only keep shadingSurfaces that are not of the 'Space' shadingSurfaceType
          //if( t_shadingSurfaceGroup.shadingSurfaceType() != "Space" ) {
          auto shadingSurfaces = t_shadingSurfaceGroup.shadingSurfaces();
          allModelObjects.insert(allModelObjects.end(), shadingSurfaces.begin(), shadingSurfaces.end());
          // }
          return allModelObjects;
        });

      if (field == SELECTED) {
        auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
        checkbox->setToolTip("Check to select all rows");
        connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &FacilityShadingGridController::onSelectAllStateChanged);
        connect(this, &FacilityShadingGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
        addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row", DataSource(allShadingSurfaces, true));
      } else if (field == SHADINGSURFACENAME) {
        addLoadNameColumn(Heading(QString(SHADINGSURFACENAME), true, false), CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::name),
                          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::setName),
                          boost::optional<std::function<void(model::ShadingSurface*)>>(
                            std::function<void(model::ShadingSurface*)>([](model::ShadingSurface* t_ss) { t_ss->remove(); })),
                          boost::optional<std::function<bool(model::ShadingSurface*)>>(), DataSource(allShadingSurfaces, true));
      } else if (field == DISPLAYNAME) {
        addLoadNameColumn(Heading(QString(DISPLAYNAME), false, false),                                        // heading
                          DisplayNameAdapter<model::ShadingSurface>(&model::ShadingSurface::displayName),     // getter
                          DisplayNameAdapter<model::ShadingSurface>(&model::ShadingSurface::setDisplayName),  // setter
                          boost::optional<std::function<void(model::ShadingSurface*)>>(),                     // resetter
                          boost::optional<std::function<bool(model::ShadingSurface*)>>(),                     // isDefaulted
                          DataSource(allShadingSurfaces, true)                                                // t_source
        );
      } else if (field == CADOBJECTID) {
        addLoadNameColumn(Heading(QString(CADOBJECTID), false, false),                                        // heading
                          DisplayNameAdapter<model::ShadingSurface>(&model::ShadingSurface::cadObjectId),     // getter
                          DisplayNameAdapter<model::ShadingSurface>(&model::ShadingSurface::setCADObjectId),  // setter
                          boost::optional<std::function<void(model::ShadingSurface*)>>(),                     // resetter
                          boost::optional<std::function<bool(model::ShadingSurface*)>>(),                     // isDefaulted
                          DataSource(allShadingSurfaces, true)                                                // t_source
        );
      } else if (field == CONSTRUCTIONNAME) {
        addDropZoneColumn(
          Heading(QString(CONSTRUCTIONNAME), true, false), CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::construction),
          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::setConstruction),
          boost::optional<std::function<void(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::resetConstruction)),
          boost::optional<std::function<bool(model::ShadingSurface*)>>(NullAdapter(&model::ShadingSurface::isConstructionDefaulted)),
          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingSurface*)>>(), DataSource(allShadingSurfaces, true));
      } else if (field == TRANSMITTANCESCHEDULENAME) {
        std::function<bool(model::ShadingSurface*, const model::Schedule&)> setter(
          [](model::ShadingSurface* t_shadingSurface, const model::Schedule& t_schedule) {
            auto copy = t_schedule;
            return t_shadingSurface->setTransmittanceSchedule(copy);
          });

        addDropZoneColumn(Heading(QString(TRANSMITTANCESCHEDULENAME), true, false),
                          CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::transmittanceSchedule), setter,
                          boost::optional<std::function<void(model::ShadingSurface*)>>(
                            CastNullAdapter<model::ShadingSurface>(&model::ShadingSurface::resetTransmittanceSchedule)),
                          boost::optional<std::function<bool(model::ShadingSurface*)>>(),
                          boost::optional<std::function<std::vector<model::ModelObject>(const model::ShadingSurface*)>>(),
                          DataSource(allShadingSurfaces, true));
      } else {
        // unhandled
        OS_ASSERT(false);
      }
    }
  }
}

QString FacilityShadingGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void FacilityShadingGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) return;

  OSGridController::checkSelectedFields();
}

void FacilityShadingGridController::onItemDropped(const OSItemId& itemId) {}

void FacilityShadingGridController::refreshModelObjects() {
  auto shadingGroups = model().getConcreteModelObjects<model::ShadingSurfaceGroup>();

  // Filter out the 'Space' shadingSurfaceTypes
  // These are displayed on the Space's "Shading" subtab
  shadingGroups.erase(std::remove_if(shadingGroups.begin(), shadingGroups.end(),
                                     [](const model::ShadingSurfaceGroup& sg) { return sg.shadingSurfaceType() == "Space"; }),
                      shadingGroups.end());

  std::sort(shadingGroups.begin(), shadingGroups.end(), openstudio::WorkspaceObjectNameLess());

  setModelObjects(subsetCastVector<model::ModelObject>(shadingGroups));
}

}  // namespace openstudio
