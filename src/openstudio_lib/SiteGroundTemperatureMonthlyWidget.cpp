/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "SiteGroundTemperatureMonthlyWidget.hpp"

#include "ModelObjectItem.hpp"
#include "OSItemSelectorButtons.hpp"

#include "YearSettingsWidget.hpp"

#include "../shared_gui_components/OSQuantityEdit.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureBuildingSurface_Impl.hpp>
#include <openstudio/model/SiteGroundTemperatureShallow_Impl.hpp>

#include <openstudio/utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>

#define TEMP_EDIT_WIDTH 90

namespace openstudio {

// SiteGroundTemperatureMonthlyItem

SiteGroundTemperatureMonthlyItem::SiteGroundTemperatureMonthlyItem(const model::ModelObject& obj, const QString& displayName, QWidget* parent)
  : OSItem(modelObjectToItemId(obj, false), OSItemType::ListItem, parent), m_modelObject(obj) {
  setText(displayName);
}

model::ModelObject SiteGroundTemperatureMonthlyItem::modelObject() const {
  return m_modelObject;
}

bool SiteGroundTemperatureMonthlyItem::equal(const OSItem* other) const {
  if (const auto* o = qobject_cast<const SiteGroundTemperatureMonthlyItem*>(other)) {
    return m_modelObject.handle() == o->m_modelObject.handle();
  }
  return false;
}

// SiteGroundTemperatureMonthlyListView

SiteGroundTemperatureMonthlyListView::SiteGroundTemperatureMonthlyListView(const model::Model& model, QWidget* parent)
  : OSItemSelector(parent) {
  auto* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  // getUniqueModelObject is non-const (creates the object if absent)
  model::Model m = model;
  auto bs = m.getUniqueModelObject<model::SiteGroundTemperatureBuildingSurface>();
  auto sh = m.getUniqueModelObject<model::SiteGroundTemperatureShallow>();

  m_bsItem = new SiteGroundTemperatureMonthlyItem(bs, tr("Building Surface Ground Temperatures"), this);
  auto* shItem = new SiteGroundTemperatureMonthlyItem(sh, tr("Shallow Ground Temperatures"), this);

  connect(m_bsItem, &OSItem::itemClicked, this, &SiteGroundTemperatureMonthlyListView::onItemClicked);
  connect(shItem, &OSItem::itemClicked, this, &SiteGroundTemperatureMonthlyListView::onItemClicked);

  layout->addWidget(m_bsItem);
  layout->addWidget(shItem);
  layout->addStretch();

  QTimer::singleShot(0, this, &SiteGroundTemperatureMonthlyListView::selectFirst);
}

OSItem* SiteGroundTemperatureMonthlyListView::selectedItem() const {
  return m_selectedItem;
}

void SiteGroundTemperatureMonthlyListView::onItemClicked(OSItem* item) {
  if (m_selectedItem) {
    m_selectedItem->setSelected(false);
  }
  m_selectedItem = item;
  if (m_selectedItem) {
    m_selectedItem->setSelected(true);
  }
  emit itemSelected(item);
}

void SiteGroundTemperatureMonthlyListView::selectFirst() {
  if (m_bsItem) {
    onItemClicked(m_bsItem);
  }
}

// SiteGroundTemperatureMonthlyView

SiteGroundTemperatureMonthlyView::SiteGroundTemperatureMonthlyView(bool isIP, const model::Model& model, QWidget* parent)
  : ModelSubTabView(new SiteGroundTemperatureMonthlyListView(model, parent),
                    new SiteGroundTemperatureMonthlyInspectorView(isIP, model, parent), false, parent) {}

// SiteGroundTemperatureMonthlyInspectorView

SiteGroundTemperatureMonthlyInspectorView::SiteGroundTemperatureMonthlyInspectorView(bool isIP, const model::Model& model, QWidget* parent)
  : ModelObjectInspectorView(model, true, parent), m_isIP(isIP), m_hiddenWidgetIndex(0), m_monthlyWidgetIndex(0) {
  createWidgets();
}

SiteGroundTemperatureMonthlyInspectorView::~SiteGroundTemperatureMonthlyInspectorView() {
  detach();
}

void SiteGroundTemperatureMonthlyInspectorView::createWidgets() {
  auto* hiddenWidget = new QWidget();
  m_hiddenWidgetIndex = this->stackedWidget()->insertWidget(0, hiddenWidget);

  auto* monthlyWidget = new QWidget();
  m_monthlyWidgetIndex = this->stackedWidget()->addWidget(monthlyWidget);

  auto* mainLayout = new QVBoxLayout();
  mainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(20);
  monthlyWidget->setLayout(mainLayout);

  m_titleLabel = new QLabel();
  m_titleLabel->setObjectName("H2");
  mainLayout->addWidget(m_titleLabel);

  auto* gridLayout = new QGridLayout();
  gridLayout->setContentsMargins(0, 0, 0, 0);
  gridLayout->setSpacing(10);
  mainLayout->addLayout(gridLayout);

  // Column headers
  auto* monthHeader = new QLabel(tr("Month"));
  monthHeader->setObjectName("H2");
  gridLayout->addWidget(monthHeader, 0, 0);

  m_temperatureHeader = new QLabel(tr("Temperature"));
  m_temperatureHeader->setObjectName("H2");
  gridLayout->addWidget(m_temperatureHeader, 0, 1);

  // One row per month, built from the translated month list
  const QStringList monthNames = YearSettingsWidget::months();

  for (int i = 0; i < 12; ++i) {
    gridLayout->addWidget(new QLabel(monthNames[i]), i + 1, 0);
    m_edits[i] = new OSQuantityEdit2("C", "C", "F", m_isIP);
    connect(this, &SiteGroundTemperatureMonthlyInspectorView::toggleUnitsClicked, m_edits[i], &OSQuantityEdit2::onUnitSystemChange);
    m_edits[i]->setFixedWidth(TEMP_EDIT_WIDTH);
    gridLayout->addWidget(m_edits[i], i + 1, 1, Qt::AlignLeft);
  }

  mainLayout->addStretch();
}

void SiteGroundTemperatureMonthlyInspectorView::attachBuildingSurface(const model::SiteGroundTemperatureBuildingSurface& obj) {
  m_buildingSurface = obj;
  m_titleLabel->setText("Site:GroundTemperature:BuildingSurface");

  using BS = model::SiteGroundTemperatureBuildingSurface;

  struct MonthBinding
  {
    double (BS::*getter)() const;
    bool (BS::*setter)(double);
    void (BS::*resetter)();
    bool (BS::*defaulted)() const;
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {
      &BS::januaryGroundTemperature,
      &BS::setJanuaryGroundTemperature,
      &BS::resetJanuaryGroundTemperature,
      &BS::isJanuaryGroundTemperatureDefaulted,
    },
    {
      &BS::februaryGroundTemperature,
      &BS::setFebruaryGroundTemperature,
      &BS::resetFebruaryGroundTemperature,
      &BS::isFebruaryGroundTemperatureDefaulted,
    },
    {
      &BS::marchGroundTemperature,
      &BS::setMarchGroundTemperature,
      &BS::resetMarchGroundTemperature,
      &BS::isMarchGroundTemperatureDefaulted,
    },
    {
      &BS::aprilGroundTemperature,
      &BS::setAprilGroundTemperature,
      &BS::resetAprilGroundTemperature,
      &BS::isAprilGroundTemperatureDefaulted,
    },
    {
      &BS::mayGroundTemperature,
      &BS::setMayGroundTemperature,
      &BS::resetMayGroundTemperature,
      &BS::isMayGroundTemperatureDefaulted,
    },
    {
      &BS::juneGroundTemperature,
      &BS::setJuneGroundTemperature,
      &BS::resetJuneGroundTemperature,
      &BS::isJuneGroundTemperatureDefaulted,
    },
    {
      &BS::julyGroundTemperature,
      &BS::setJulyGroundTemperature,
      &BS::resetJulyGroundTemperature,
      &BS::isJulyGroundTemperatureDefaulted,
    },
    {
      &BS::augustGroundTemperature,
      &BS::setAugustGroundTemperature,
      &BS::resetAugustGroundTemperature,
      &BS::isAugustGroundTemperatureDefaulted,
    },
    {
      &BS::septemberGroundTemperature,
      &BS::setSeptemberGroundTemperature,
      &BS::resetSeptemberGroundTemperature,
      &BS::isSeptemberGroundTemperatureDefaulted,
    },
    {
      &BS::octoberGroundTemperature,
      &BS::setOctoberGroundTemperature,
      &BS::resetOctoberGroundTemperature,
      &BS::isOctoberGroundTemperatureDefaulted,
    },
    {
      &BS::novemberGroundTemperature,
      &BS::setNovemberGroundTemperature,
      &BS::resetNovemberGroundTemperature,
      &BS::isNovemberGroundTemperatureDefaulted,
    },
    {
      &BS::decemberGroundTemperature,
      &BS::setDecemberGroundTemperature,
      &BS::resetDecemberGroundTemperature,
      &BS::isDecemberGroundTemperatureDefaulted,
    },
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& m = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_buildingSurface, DoubleGetter([this, g = m.getter]() { return (m_buildingSurface.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = m.setter](double v) { return (m_buildingSurface.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = m.resetter]() { (m_buildingSurface.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = m.defaulted]() { return (m_buildingSurface.get_ptr()->*d)(); }));
  }

  this->stackedWidget()->setCurrentIndex(m_monthlyWidgetIndex);
}

void SiteGroundTemperatureMonthlyInspectorView::attachShallow(const model::SiteGroundTemperatureShallow& obj) {
  m_shallow = obj;
  m_titleLabel->setText("Site:GroundTemperature:Shallow");

  using SH = model::SiteGroundTemperatureShallow;

  struct MonthBinding
  {
    double (SH::*getter)() const;
    bool (SH::*setter)(double);
    void (SH::*resetter)();
    bool (SH::*defaulted)() const;
  };

  static const std::array<MonthBinding, 12> month_binders{{
    {
      &SH::januarySurfaceGroundTemperature,
      &SH::setJanuarySurfaceGroundTemperature,
      &SH::resetJanuarySurfaceGroundTemperature,
      &SH::isJanuarySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::februarySurfaceGroundTemperature,
      &SH::setFebruarySurfaceGroundTemperature,
      &SH::resetFebruarySurfaceGroundTemperature,
      &SH::isFebruarySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::marchSurfaceGroundTemperature,
      &SH::setMarchSurfaceGroundTemperature,
      &SH::resetMarchSurfaceGroundTemperature,
      &SH::isMarchSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::aprilSurfaceGroundTemperature,
      &SH::setAprilSurfaceGroundTemperature,
      &SH::resetAprilSurfaceGroundTemperature,
      &SH::isAprilSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::maySurfaceGroundTemperature,
      &SH::setMaySurfaceGroundTemperature,
      &SH::resetMaySurfaceGroundTemperature,
      &SH::isMaySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::juneSurfaceGroundTemperature,
      &SH::setJuneSurfaceGroundTemperature,
      &SH::resetJuneSurfaceGroundTemperature,
      &SH::isJuneSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::julySurfaceGroundTemperature,
      &SH::setJulySurfaceGroundTemperature,
      &SH::resetJulySurfaceGroundTemperature,
      &SH::isJulySurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::augustSurfaceGroundTemperature,
      &SH::setAugustSurfaceGroundTemperature,
      &SH::resetAugustSurfaceGroundTemperature,
      &SH::isAugustSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::septemberSurfaceGroundTemperature,
      &SH::setSeptemberSurfaceGroundTemperature,
      &SH::resetSeptemberSurfaceGroundTemperature,
      &SH::isSeptemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::octoberSurfaceGroundTemperature,
      &SH::setOctoberSurfaceGroundTemperature,
      &SH::resetOctoberSurfaceGroundTemperature,
      &SH::isOctoberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::novemberSurfaceGroundTemperature,
      &SH::setNovemberSurfaceGroundTemperature,
      &SH::resetNovemberSurfaceGroundTemperature,
      &SH::isNovemberSurfaceGroundTemperatureDefaulted,
    },
    {
      &SH::decemberSurfaceGroundTemperature,
      &SH::setDecemberSurfaceGroundTemperature,
      &SH::resetDecemberSurfaceGroundTemperature,
      &SH::isDecemberSurfaceGroundTemperatureDefaulted,
    },
  }};

  for (int i = 0; i < 12; ++i) {
    const auto& m = month_binders[i];
    m_edits[i]->bind(m_isIP, *m_shallow, DoubleGetter([this, g = m.getter]() { return (m_shallow.get_ptr()->*g)(); }),
                     boost::optional<DoubleSetter>([this, s = m.setter](double v) { return (m_shallow.get_ptr()->*s)(v); }),
                     boost::optional<NoFailAction>([this, r = m.resetter]() { (m_shallow.get_ptr()->*r)(); }), boost::none, boost::none,
                     boost::optional<BasicQuery>([this, d = m.defaulted]() { return (m_shallow.get_ptr()->*d)(); }));
  }

  this->stackedWidget()->setCurrentIndex(m_monthlyWidgetIndex);
}

void SiteGroundTemperatureMonthlyInspectorView::detach() {
  this->stackedWidget()->setCurrentIndex(m_hiddenWidgetIndex);

  for (auto* edit : m_edits) {
    if (edit) {
      edit->unbind();
    }
  }

  m_buildingSurface = boost::none;
  m_shallow = boost::none;
}

void SiteGroundTemperatureMonthlyInspectorView::onSelectItem(OSItem* item) {
  auto* monthlyItem = qobject_cast<SiteGroundTemperatureMonthlyItem*>(item);
  OS_ASSERT(monthlyItem);
  onSelectModelObject(monthlyItem->modelObject());
}

void SiteGroundTemperatureMonthlyInspectorView::onClearSelection() {
  ModelObjectInspectorView::onClearSelection();
  detach();
}

void SiteGroundTemperatureMonthlyInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {
  detach();
  if (auto obj = modelObject.optionalCast<model::SiteGroundTemperatureBuildingSurface>()) {
    attachBuildingSurface(*obj);
  } else if (auto obj = modelObject.optionalCast<model::SiteGroundTemperatureShallow>()) {
    attachShallow(*obj);
  }
}

void SiteGroundTemperatureMonthlyInspectorView::onUpdate() {
  // nothing to refresh
}

}  // namespace openstudio
