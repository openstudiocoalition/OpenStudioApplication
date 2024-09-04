/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "FacilityStoriesGridView.hpp"

#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"
#include "RenderingColorWidget.hpp"

#include "../shared_gui_components/OSCheckBox.hpp"
#include "../shared_gui_components/OSDoubleEdit.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSObjectSelector.hpp"

#include <openstudio/model/BuildingStory.hpp>
#include <openstudio/model/BuildingStory_Impl.hpp>
#include <openstudio/model/DefaultConstructionSet.hpp>
#include <openstudio/model/DefaultConstructionSet_Impl.hpp>
#include <openstudio/model/DefaultScheduleSet.hpp>
#include <openstudio/model/DefaultScheduleSet_Impl.hpp>
#include <openstudio/model/Model.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/ModelObject.hpp>
#include <openstudio/model/ModelObject_Impl.hpp>
#include <openstudio/model/RenderingColor.hpp>
#include <openstudio/model/RenderingColor_Impl.hpp>

// For purgeObjects
#include <openstudio/model/Space.hpp>

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>
#include <openstudio/utilities/idd/OS_BuildingStory_FieldEnums.hxx>
#include <openstudio/utilities/units/QuantityConverter.hpp>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QRegularExpression>

// These defines provide a common area for field display names
// used on column headers, and other grid widgets

#define NAME "Story Name"
#define SELECTED "All"
#define DISPLAYNAME "Display Name"
#define CADOBJECTID "CAD Object ID"

// GENERAL
#define NOMINALZCOORDINATE "Nominal Z Coordinate"
#define NOMINALFLOORTOFLOORHEIGHT "Nominal Floor to Floor Height"
#define DEFAULTCONSTRUCTIONSETNAME "Default Construction Set Name"
#define DEFAULTSCHEDULESETNAME "Default Schedule Set Name"
#define GROUPRENDERINGNAME "Group Rendering Name"
#define NOMINALFLOORTOCEILINGHEIGHT "Nominal Floor to Ceiling Height"

// FILTERS
#define NOMINALZCOORDINATEGREATERTHAN "Nominal Z Coordinate >"
#define NOMINALZCOORDINATELESSTHAN "Nominal Z Coordinate <"

namespace openstudio {

FacilityStoriesGridView::FacilityStoriesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent)
  : GridViewSubTab(isIP, displayAdditionalProps, model, parent) {
  auto modelObjects = subsetCastVector<model::ModelObject>(m_model.getConcreteModelObjects<model::BuildingStory>());
  std::sort(modelObjects.begin(), modelObjects.end(), openstudio::WorkspaceObjectNameLess());

  m_gridController =
    new FacilityStoriesGridController(isIP, displayAdditionalProps, "Building Stories", IddObjectType::OS_BuildingStory, model, modelObjects);
  m_gridView = new OSGridView(m_gridController, "Building Stories", "Drop\nStory", false, parent);

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

  // NOMINALZCOORDINATEGREATERTHAN

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(NOMINALZCOORDINATEGREATERTHAN);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_greaterThanFilter = new QLineEdit();
  m_greaterThanFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  connect(m_greaterThanFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityStoriesGridView::greaterThanFilterChanged);

  QRegularExpression regex("^(-?\\d*\\.?\\d+)?$");
  auto* validator = new QRegularExpressionValidator(regex, this);
  m_greaterThanFilter->setValidator(validator);

  layout->addWidget(m_greaterThanFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  // NOMINALZCOORDINATELESSTHAN

  layout = new QVBoxLayout();

  label = new QLabel();
  label->setText(NOMINALZCOORDINATELESSTHAN);
  label->setObjectName("H3");
  layout->addWidget(label, Qt::AlignTop | Qt::AlignLeft);

  m_lessThanFilter = new QLineEdit();
  m_lessThanFilter->setFixedWidth(OSItem::ITEM_WIDTH);
  // Evan note: there are issues with using the signal textChanged or textEdited, related to the design and updating of the gridview (loss of focus, and updates per key stroke)
  connect(m_lessThanFilter, &QLineEdit::editingFinished, this, &openstudio::FacilityStoriesGridView::lessThanFilterChanged);

  validator = new QRegularExpressionValidator(regex, this);
  m_greaterThanFilter->setValidator(validator);

  layout->addWidget(m_lessThanFilter, Qt::AlignTop | Qt::AlignLeft);
  layout->addStretch();
  filterGridLayout->addLayout(layout, filterGridLayout->rowCount() - 1, filterGridLayout->columnCount());

  filterGridLayout->setRowStretch(filterGridLayout->rowCount(), 100);
  filterGridLayout->setColumnStretch(filterGridLayout->columnCount(), 100);

  m_gridView->addLayoutToContentLayout(filterGridLayout);
  m_gridView->addSpacingToContentLayout(7);

  clearSelection();
}

void FacilityStoriesGridView::addObject(const IddObjectType& iddObjectType) {
  if (IddObjectType::OS_BuildingStory == iddObjectType.value()) {
    model::BuildingStory(this->m_model);
  }
}

void FacilityStoriesGridView::purgeObjects(const IddObjectType& iddObjectType) {
  // If no spaces in the story -> remove
  for (model::BuildingStory story : this->m_model.getConcreteModelObjects<model::BuildingStory>()) {
    if (story.spaces().empty()) {
      story.remove();
    }
  }
}

void FacilityStoriesGridView::greaterThanFilterChanged() {
  filterChanged();
}

void FacilityStoriesGridView::lessThanFilterChanged() {
  filterChanged();
}

void FacilityStoriesGridView::filterChanged() {

  auto upperLimit = std::numeric_limits<double>::max();
  auto lowerLimit = std::numeric_limits<double>::lowest();

  if (!this->m_lessThanFilter->text().isEmpty()) {
    upperLimit = this->m_lessThanFilter->text().toDouble();
  }

  if (!this->m_greaterThanFilter->text().isEmpty()) {
    lowerLimit = this->m_greaterThanFilter->text().toDouble();
  }

  if (m_isIP == true) {
    auto convertedValue = convert(upperLimit, "ft", "m");
    OS_ASSERT(convertedValue);
    upperLimit = *convertedValue;

    convertedValue = convert(lowerLimit, "ft", "m");
    OS_ASSERT(convertedValue);
    lowerLimit = *convertedValue;
  }

  this->m_gridController->setObjectFilter([upperLimit, lowerLimit](const model::ModelObject& obj) -> bool {
    boost::optional<model::BuildingStory> story = obj.optionalCast<model::BuildingStory>();
    if (story) {
      auto nominalZCoordinate = story->nominalZCoordinate();
      if (nominalZCoordinate) {
        if (*nominalZCoordinate >= upperLimit || *nominalZCoordinate <= lowerLimit) {
          return false;
        }
      }
    }
    return true;
  });
}

void FacilityStoriesGridView::onSelectItem() {
  //m_itemSelectorButtons->enableAddButton();
  //m_itemSelectorButtons->enableCopyButton();
  m_itemSelectorButtons->enableRemoveButton();
  //m_itemSelectorButtons->enablePurgeButton();
}

void FacilityStoriesGridView::onClearSelection() {
  clearSelection();
}

void FacilityStoriesGridView::clearSelection() {
  //m_itemSelectorButtons->disableAddButton();
  //m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  //m_itemSelectorButtons->disablePurgeButton();
}

FacilityStoriesGridController::FacilityStoriesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText,
                                                             IddObjectType iddObjectType, const model::Model& model,
                                                             const std::vector<model::ModelObject>& modelObjects)
  : OSGridController(isIP, headerText, iddObjectType, model, modelObjects, displayAdditionalProps) {
  setCategoriesAndFields();
}

void FacilityStoriesGridController::setCategoriesAndFields() {
  {
    std::vector<QString> fields{
      GROUPRENDERINGNAME,          NOMINALZCOORDINATE,         NOMINALFLOORTOFLOORHEIGHT,
      NOMINALFLOORTOCEILINGHEIGHT, DEFAULTCONSTRUCTIONSETNAME, DEFAULTSCHEDULESETNAME,
    };
    std::pair<QString, std::vector<QString>> categoryAndFields = std::make_pair(QString("General"), fields);
    addCategoryAndFields(categoryAndFields);
  }

  OSGridController::setCategoriesAndFields();
}

void FacilityStoriesGridController::onCategorySelected(int index) {
  OSGridController::onCategorySelected(index);
}

void FacilityStoriesGridController::addColumns(const QString& category, std::vector<QString>& fields) {

  if (isDisplayAdditionalProps()) {
    fields.insert(fields.begin(), {DISPLAYNAME, CADOBJECTID});
  }
  // always show name and selected columns
  fields.insert(fields.begin(), {NAME, SELECTED});

  resetBaseConcepts();

  for (const auto& field : fields) {

    if (field == NAME) {
      addParentNameLineEditColumn(Heading(QString(NAME), false, false), false, CastNullAdapter<model::BuildingStory>(&model::BuildingStory::name),
                                  CastNullAdapter<model::BuildingStory>(&model::BuildingStory::setName));
    } else if (field == DISPLAYNAME) {
      addNameLineEditColumn(Heading(QString(DISPLAYNAME), false, false),                                     // heading
                            false,                                                                           // isInspectable
                            false,                                                                           // isLocked
                            DisplayNameAdapter<model::BuildingStory>(&model::BuildingStory::displayName),    // getter
                            DisplayNameAdapter<model::BuildingStory>(&model::BuildingStory::setDisplayName)  // setter
      );
    } else if (field == CADOBJECTID) {
      addNameLineEditColumn(Heading(QString(CADOBJECTID), false, false),                                     // heading
                            false,                                                                           // isInspectable
                            false,                                                                           // isLocked
                            DisplayNameAdapter<model::BuildingStory>(&model::BuildingStory::cadObjectId),    // getter
                            DisplayNameAdapter<model::BuildingStory>(&model::BuildingStory::setCADObjectId)  // setter
      );
    } else if (field == SELECTED) {
      auto checkbox = QSharedPointer<OSSelectAllCheckBox>(new OSSelectAllCheckBox());
      checkbox->setToolTip("Check to select all rows");
      connect(checkbox.data(), &OSSelectAllCheckBox::stateChanged, this, &FacilityStoriesGridController::onSelectAllStateChanged);
      connect(this, &FacilityStoriesGridController::gridRowSelectionChanged, checkbox.data(), &OSSelectAllCheckBox::onGridRowSelectionChanged);
      addSelectColumn(Heading(QString(SELECTED), false, false, checkbox), "Check to select this row");
    } else if (field == NOMINALZCOORDINATE) {
      addQuantityEditColumn(Heading(QString(NOMINALFLOORTOFLOORHEIGHT)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::BuildingStory::nominalZCoordinate), NullAdapter(&model::BuildingStory::setNominalZCoordinate));
    } else if (field == NOMINALFLOORTOFLOORHEIGHT) {
      addQuantityEditColumn(Heading(QString(NOMINALFLOORTOFLOORHEIGHT)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::BuildingStory::nominalFloortoFloorHeight),
                            NullAdapter(&model::BuildingStory::setNominalFloortoFloorHeight));
    } else if (field == DEFAULTCONSTRUCTIONSETNAME) {
      addDropZoneColumn(Heading(QString(DEFAULTCONSTRUCTIONSETNAME)),
                        CastNullAdapter<model::BuildingStory>(&model::BuildingStory::defaultConstructionSet),
                        CastNullAdapter<model::BuildingStory>(&model::BuildingStory::setDefaultConstructionSet),
                        boost::optional<std::function<void(model::BuildingStory*)>>(
                          CastNullAdapter<model::BuildingStory>(&model::BuildingStory::resetDefaultConstructionSet)));
    } else if (field == DEFAULTSCHEDULESETNAME) {
      addDropZoneColumn(Heading(QString(DEFAULTSCHEDULESETNAME)), CastNullAdapter<model::BuildingStory>(&model::BuildingStory::defaultScheduleSet),
                        CastNullAdapter<model::BuildingStory>(&model::BuildingStory::setDefaultScheduleSet),
                        boost::optional<std::function<void(model::BuildingStory*)>>(
                          CastNullAdapter<model::BuildingStory>(&model::BuildingStory::resetDefaultScheduleSet)));
    } else if (field == GROUPRENDERINGNAME) {
      addRenderingColorColumn(Heading(QString(GROUPRENDERINGNAME), true, false),
                              CastNullAdapter<model::BuildingStory>(&model::BuildingStory::renderingColor),
                              CastNullAdapter<model::BuildingStory>(&model::BuildingStory::setRenderingColor));
    } else if (field == NOMINALFLOORTOCEILINGHEIGHT) {
      addQuantityEditColumn(Heading(QString(NOMINALFLOORTOCEILINGHEIGHT)), QString("m"), QString("m"), QString("ft"), isIP(),
                            NullAdapter(&model::BuildingStory::nominalFloortoCeilingHeight),
                            NullAdapter(&model::BuildingStory::setNominalFloortoCeilingHeight));
    } else {
      // unhandled
      OS_ASSERT(false);
    }
  }
}

QString FacilityStoriesGridController::getColor(const model::ModelObject& modelObject) {
  QColor defaultColor(Qt::lightGray);
  return defaultColor.name();
}

void FacilityStoriesGridController::checkSelectedFields() {
  if (!this->hasHorizontalHeader()) {
    return;
  }

  OSGridController::checkSelectedFields();
}

void FacilityStoriesGridController::onItemDropped(const OSItemId& itemId) {}

void FacilityStoriesGridController::refreshModelObjects() {
  auto stories = model().getConcreteModelObjects<model::BuildingStory>();
  std::sort(stories.begin(), stories.end(), openstudio::WorkspaceObjectNameLess());
  setModelObjects(subsetCastVector<model::ModelObject>(stories));
}

}  // namespace openstudio
