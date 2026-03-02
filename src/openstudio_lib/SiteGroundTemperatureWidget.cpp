/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SiteGroundTemperatureWidget.hpp"

#include "ModelObjectItem.hpp"
#include "ModelObjectListView.hpp"
#include "ModelObjectTypeItem.hpp"
#include "OSCollapsibleItemHeader.hpp"
#include "OSItem.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../shared_gui_components/OSDoubleEdit.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

#define TEMP_EDIT_WIDTH 90

namespace openstudio {

// SiteGroundTemperatureListView

SiteGroundTemperatureListView::SiteGroundTemperatureListView(const model::Model& model, bool addScrollArea, OSItemType headerType, QWidget* parent)
  : OSCollapsibleItemList(addScrollArea, parent), m_model(model) {
  // Unique object — ensure it exists before the list view queries the model
  m_model.getUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();

  auto* header = new OSCollapsibleItemHeader("Building Surface Ground Temperatures", OSItemId("", "", false), headerType);
  auto* listView = new ModelObjectListView(model::SiteGroundTemperatureBuildingSurface::iddObjectType(), model, false, false);
  auto* item = new ModelObjectTypeItem(header, listView);
  addCollapsibleItem(item);
}

// SiteGroundTemperatureView

SiteGroundTemperatureView::SiteGroundTemperatureView(const model::Model& model, QWidget* parent)
  : ModelSubTabView(new SiteGroundTemperatureListView(model, true, OSItemType::CollapsibleListHeader, parent),
                    new SiteGroundTemperatureInspectorView(model, parent), false, parent) {}

// SiteGroundTemperatureInspectorView

SiteGroundTemperatureInspectorView::SiteGroundTemperatureInspectorView(const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_hiddenWidgetIndex(0), m_buildingSurfaceWidgetIndex(0) {
  createWidgets();
}

SiteGroundTemperatureInspectorView::~SiteGroundTemperatureInspectorView() {
  detach();
}

void SiteGroundTemperatureInspectorView::createWidgets() {
  QVBoxLayout* vLayout = nullptr;
  QLabel* label = nullptr;

  auto* hiddenWidget = new QWidget();
  m_hiddenWidgetIndex = this->stackedWidget()->insertWidget(0, hiddenWidget);

  auto* bsWidget = new QWidget();
  m_buildingSurfaceWidgetIndex = this->stackedWidget()->addWidget(bsWidget);

  auto* mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(20);
  bsWidget->setLayout(mainLayout);

  label = new QLabel("Site:GroundTemperature:BuildingSurface");
  label->setObjectName("H2");
  mainLayout->addWidget(label);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(0, 0, 0, 0);
  gridLayout->setSpacing(10);
  mainLayout->addLayout(gridLayout);

  // Row 0: January - April

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("January (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_januaryEdit = new OSDoubleEdit2();
  m_januaryEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_januaryEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 0, 0, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("February (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_februaryEdit = new OSDoubleEdit2();
  m_februaryEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_februaryEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 0, 1, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("March (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_marchEdit = new OSDoubleEdit2();
  m_marchEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_marchEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 0, 2, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("April (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_aprilEdit = new OSDoubleEdit2();
  m_aprilEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_aprilEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 0, 3, Qt::AlignLeft);

  // Row 1: May - August

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("May (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_mayEdit = new OSDoubleEdit2();
  m_mayEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_mayEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 1, 0, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("June (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_juneEdit = new OSDoubleEdit2();
  m_juneEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_juneEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 1, 1, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("July (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_julyEdit = new OSDoubleEdit2();
  m_julyEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_julyEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 1, 2, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("August (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_augustEdit = new OSDoubleEdit2();
  m_augustEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_augustEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 1, 3, Qt::AlignLeft);

  // Row 2: September - December

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("September (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_septemberEdit = new OSDoubleEdit2();
  m_septemberEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_septemberEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 2, 0, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("October (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_octoberEdit = new OSDoubleEdit2();
  m_octoberEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_octoberEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 2, 1, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("November (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_novemberEdit = new OSDoubleEdit2();
  m_novemberEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_novemberEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 2, 2, Qt::AlignLeft);

  vLayout = new QVBoxLayout();
  vLayout->setSpacing(5);
  label = new QLabel("December (\xc2\xb0""C)");
  label->setObjectName("H2");
  vLayout->addWidget(label);
  m_decemberEdit = new OSDoubleEdit2();
  m_decemberEdit->setFixedWidth(TEMP_EDIT_WIDTH);
  vLayout->addWidget(m_decemberEdit);
  vLayout->addStretch();
  gridLayout->addLayout(vLayout, 2, 3, Qt::AlignLeft);

  gridLayout->setColumnStretch(100, 100);

  mainLayout->addStretch();
}

void SiteGroundTemperatureInspectorView::attachBuildingSurface(const model::SiteGroundTemperatureBuildingSurface& obj) {
  m_buildingSurface = obj;

  m_januaryEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::januaryGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setJanuaryGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetJanuaryGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isJanuaryGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_februaryEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::februaryGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setFebruaryGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetFebruaryGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isFebruaryGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_marchEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::marchGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setMarchGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetMarchGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isMarchGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_aprilEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::aprilGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setAprilGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetAprilGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isAprilGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_mayEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::mayGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setMayGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetMayGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isMayGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_juneEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::juneGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setJuneGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetJuneGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isJuneGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_julyEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::julyGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setJulyGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetJulyGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isJulyGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_augustEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::augustGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setAugustGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetAugustGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isAugustGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_septemberEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::septemberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setSeptemberGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetSeptemberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isSeptemberGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_octoberEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::octoberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setOctoberGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetOctoberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isOctoberGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_novemberEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::novemberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setNovemberGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetNovemberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isNovemberGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  m_decemberEdit->bind(
    *m_buildingSurface,
    DoubleGetter(std::bind(&model::SiteGroundTemperatureBuildingSurface::decemberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::optional<DoubleSetter>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::setDecemberGroundTemperature, m_buildingSurface.get_ptr(), std::placeholders::_1)),
    boost::optional<NoFailAction>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::resetDecemberGroundTemperature, m_buildingSurface.get_ptr())),
    boost::none, boost::none,
    boost::optional<BasicQuery>(
      std::bind(&model::SiteGroundTemperatureBuildingSurface::isDecemberGroundTemperatureDefaulted, m_buildingSurface.get_ptr())));

  this->stackedWidget()->setCurrentIndex(m_buildingSurfaceWidgetIndex);
}

void SiteGroundTemperatureInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(m_hiddenWidgetIndex);

  if (m_januaryEdit) m_januaryEdit->unbind();
  if (m_februaryEdit) m_februaryEdit->unbind();
  if (m_marchEdit) m_marchEdit->unbind();
  if (m_aprilEdit) m_aprilEdit->unbind();
  if (m_mayEdit) m_mayEdit->unbind();
  if (m_juneEdit) m_juneEdit->unbind();
  if (m_julyEdit) m_julyEdit->unbind();
  if (m_augustEdit) m_augustEdit->unbind();
  if (m_septemberEdit) m_septemberEdit->unbind();
  if (m_octoberEdit) m_octoberEdit->unbind();
  if (m_novemberEdit) m_novemberEdit->unbind();
  if (m_decemberEdit) m_decemberEdit->unbind();

  m_buildingSurface = boost::none;
}

void SiteGroundTemperatureInspectorView::onSelectItem(OSItem* item) {
  auto* modelObjectItem = qobject_cast<ModelObjectItem*>(item);
  OS_ASSERT(modelObjectItem);
  onSelectModelObject(modelObjectItem->modelObject());
}

void SiteGroundTemperatureInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();
  detach();
}

void SiteGroundTemperatureInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  if (auto obj = modelObject.optionalCast<model::SiteGroundTemperatureBuildingSurface>()) {
    attachBuildingSurface(*obj);
  }
}

void SiteGroundTemperatureInspectorView::onUpdate() {
  // nothing to refresh
}

}  // namespace openstudio
