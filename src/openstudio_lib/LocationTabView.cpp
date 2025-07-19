/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LocationTabView.hpp"

#include "DesignDayGridView.hpp"
#include "ModelObjectListView.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "OSDropZone.hpp"
#include "OSItemSelectorButtons.hpp"
#include "SchedulesTabController.hpp"

#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSGridView.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "../shared_gui_components/OSSwitch.hpp"

#include "../openstudio_app/OpenStudioApp.hpp"

#include <openstudio/model/ClimateZones.hpp>
#include <openstudio/model/ClimateZones_Impl.hpp>
#include <openstudio/model/DesignDay.hpp>
#include <openstudio/model/DesignDay_Impl.hpp>
#include <openstudio/model/Model_Impl.hpp>
#include <openstudio/model/RunPeriod.hpp>
#include <openstudio/model/RunPeriodControlDaylightSavingTime.hpp>
#include <openstudio/model/RunPeriodControlDaylightSavingTime_Impl.hpp>
#include <openstudio/model/RunPeriod_Impl.hpp>
#include <openstudio/model/Site_Impl.hpp>
#include <openstudio/model/SizingPeriod.hpp>
#include <openstudio/model/SizingPeriod_Impl.hpp>
#include <openstudio/model/WeatherFile.hpp>
#include <openstudio/model/WeatherFileConditionType.hpp>
#include <openstudio/model/WeatherFileDays.hpp>
#include <openstudio/model/WeatherFile_Impl.hpp>
#include <openstudio/model/YearDescription_Impl.hpp>

#include "../model_editor/Utilities.hpp"

#include <openstudio/energyplus/ReverseTranslator.hpp>

//#include "../runmanager/lib/ConfigOptions.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/filetypes/EpwFile.hpp>
#include <openstudio/utilities/idf/IdfFile.hpp>

#include <openstudio/utilities/idd/IddEnums.hxx>

#include <boost/smart_ptr.hpp>

#include <QBoxLayout>
#include <QButtonGroup>
#include <QComboBox>
#include <QDateTime>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>
#include <QSettings>
#include <QSizePolicy>
#include <QCoreApplication>
#include <QObject>
#include <QPushButton>
#include <span>

static constexpr auto NAME("Name: ");
static constexpr auto LATITUDE("Latitude: ");
static constexpr auto LONGITUDE("Longitude: ");
static constexpr auto ELEVATION("Elevation: ");
static constexpr auto TIME_ZONE("Time Zone: ");
static constexpr auto NUM_DESIGN_DAYS("Number of Design Days: ");
static constexpr auto SETWEATHERFILE("Set Weather File");
static constexpr auto CHANGEWEATHERFILE("Change Weather File");

namespace openstudio {

SortableDesignDay::SortableDesignDay(const openstudio::model::DesignDay& designDay) : m_designDay(designDay) {
  QRegExp regex("^.*Ann.*([\\d\\.]+)[\\s]?%.*$", Qt::CaseInsensitive);
  if (regex.exactMatch(toQString(designDay.nameString())) && regex.captureCount() == 1) {
    m_permil = qstringToPermil(regex.capturedTexts()[1]);
    if (m_permil > 500) {
      m_type = "Heating";
    } else {
      m_type = "Cooling";
    }
  }
}

int SortableDesignDay::qstringToPermil(const QString& str) {
  return (int)(str.toDouble() * 10.0);
}

QString SortableDesignDay::permilToQString(int permil) {
  return QString::number((double)permil / 10.0, 'f', 1);
}

QString SortableDesignDay::key(const QString& type, int sortablePermil) {
  return type + permilToQString(sortablePermil);
}

QString SortableDesignDay::type() const {
  return m_type;
}

int SortableDesignDay::permil() const {
  return m_permil;
}

int SortableDesignDay::sortablePermil() const {
  return ((m_permil < 500) ? m_permil : 1000 - m_permil);
}

LocationTabView::LocationTabView(const model::Model& model, const QString& modelTempDir, QWidget* parent)
  : MainTabView(tr("Site"), MainTabView::SUB_TAB, parent) {}

LocationView::LocationView(bool isIP, const model::Model& model, const QString& modelTempDir)
  : m_model(model),
    m_site(m_model.getUniqueModelObject<model::Site>()),
    m_yearDescription(m_model.getUniqueModelObject<model::YearDescription>()),
    m_modelTempDir(modelTempDir),
    m_isIP(isIP) {
  OS_ASSERT(m_site);

  loadQSettings();

  auto climateZones = m_model.getUniqueModelObject<model::ClimateZones>();

  // ***** Main Layout *****
  auto* mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);

  // ***** Scroll *****
  auto* scrollLayout = new QVBoxLayout();
  scrollLayout->setContentsMargins(0, 0, 0, 0);
  scrollLayout->setSpacing(0);

  auto* scrollWidget = new QWidget();
  scrollWidget->setObjectName("ScrollWidget");
  scrollWidget->setStyleSheet("QWidget#ScrollWidget { background: transparent; }");
  scrollWidget->setLayout(scrollLayout);

  auto* scrollArea = new QScrollArea();
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->setFrameStyle(QFrame::NoFrame);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setWidget(scrollWidget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setBackgroundRole(QPalette::NoRole);
  mainLayout->addWidget(scrollArea);

  // ***** Upper Horizontal Layout *****
  auto* upperHorizontalLayout = new QHBoxLayout();
  upperHorizontalLayout->setContentsMargins(0, 0, 0, 0);
  upperHorizontalLayout->setSpacing(0);

  // ***** Left Vertical Layout *****
  auto* leftVLayout = new QVBoxLayout();
  leftVLayout->setContentsMargins(7, 2, 7, 7);
  leftVLayout->setSpacing(0);

  // ***** Weather File GridLayout *****
  auto* weatherFileGridLayout = new QGridLayout();
  weatherFileGridLayout->setContentsMargins(7, 3, 7, 7);
  weatherFileGridLayout->setSpacing(7);

  // ***** Measure Tags GridLayout *****
  auto* siteInfoGridLayout = new QGridLayout();
  siteInfoGridLayout->setContentsMargins(7, 7, 7, 7);
  siteInfoGridLayout->setSpacing(7);

  // ***** Measure Tags GridLayout *****
  auto* measureTagsGridLayout = new QGridLayout();
  measureTagsGridLayout->setContentsMargins(7, 7, 7, 7);
  measureTagsGridLayout->setSpacing(7);

  // ***** Schedules Layout *****
  auto* schedulesLayout = new QVBoxLayout();
  schedulesLayout->setContentsMargins(0, 0, 0, 0);
  schedulesLayout->setSpacing(0);

  // ***** Vertical Line *****
  auto* vLine = new QFrame();
  vLine->setFrameShape(QFrame::VLine);
  vLine->setFrameShadow(QFrame::Sunken);

  // ***** Small Horizontal Line *****
  auto* smallHLine = new QFrame();
  smallHLine->setFrameShape(QFrame::HLine);
  smallHLine->setFrameShadow(QFrame::Sunken);

  // ***** Main Horizontal Line *****
  auto* mainHLine = new QFrame();
  mainHLine->setFrameShape(QFrame::HLine);
  mainHLine->setFrameShadow(QFrame::Sunken);

  // ***** Weather File *****
  auto* label = new QLabel(tr("Weather File"));
  label->setObjectName("H2");

  m_weatherFileBtn = new QPushButton(this);
  m_weatherFileBtn->setFlat(true);
  m_weatherFileBtn->setObjectName("StandardGrayButton");
  connect(m_weatherFileBtn, &QPushButton::clicked, this, &LocationView::onWeatherFileBtnClicked);

  auto* hLayout = new QHBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->setSpacing(7);

  hLayout->addWidget(label, 0, Qt::AlignLeft);
  hLayout->addWidget(m_weatherFileBtn, 0, Qt::AlignLeft);
  hLayout->addStretch();

  leftVLayout->addLayout(hLayout);

  // ***** Site Info *****
  int i = 0;

  label = new QLabel(tr("Name: "));

  m_siteName = new QLineEdit();
  connect(m_siteName, &QLineEdit::textEdited, this, &LocationView::onSiteNameChanged);

  hLayout = new QHBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->setSpacing(7);

  hLayout->addWidget(label, 0, Qt::AlignLeft);
  hLayout->addWidget(m_siteName, 0, Qt::AlignLeft);
  hLayout->addStretch();

  weatherFileGridLayout->addLayout(hLayout, i++, 0);

  m_latitudeLbl = new QLabel(tr(LATITUDE));
  weatherFileGridLayout->addWidget(m_latitudeLbl, i++, 0);

  m_longitudeLbl = new QLabel(tr(LONGITUDE));
  weatherFileGridLayout->addWidget(m_longitudeLbl, i++, 0);

  m_timeZoneLbl = new QLabel(tr(TIME_ZONE));
  weatherFileGridLayout->addWidget(m_timeZoneLbl, i++, 0);

  // ***** Weather File Download Location *****
  label = new QLabel(tr("Download weather files at <a href=\"http://www.energyplus.net/weather\">www.energyplus.net/weather</a>"));
  label->setOpenExternalLinks(true);
  weatherFileGridLayout->addWidget(label, i++, 0);

  // ***** Add Weather File GridLayout *****
  weatherFileGridLayout->setColumnStretch(i, 10);
  leftVLayout->addLayout(weatherFileGridLayout);
  leftVLayout->addStretch();

  // Site Information
  {
    label = new QLabel(tr("Site Information:"));
    label->setObjectName("H2");
    leftVLayout->addWidget(label);

    i = 0;

    {
      label = new QLabel(tr("Keep Site Location Information"));
      label->setToolTip(tr("If enabled, this will write the Site:Location object that will keep the Elevation change for example."));

      m_keepSiteLocationInfo = new OSSwitch2();

      m_keepSiteLocationInfo->bind(*m_site, BoolGetter([this] { return m_site->keepSiteLocationInformation(); }),
                                   boost::optional<BoolSetter>([this](bool b) {
                                     bool result = m_site->setKeepSiteLocationInformation(b);

                                     if (result) {

                                       // force the style to update
                                       m_elevation->clearCachedText();

                                       if (b) {
                                         // set elevation if turning on
                                         if (m_site->isElevationDefaulted()) {
                                           m_site->setElevation(m_weatherFileElevation);
                                         } else {
                                           m_site->setElevation(m_site->elevation());
                                         }
                                       } else {
                                         // reset elevation if turning off
                                         if (std::abs(m_weatherFileElevation) > 0.01) {
                                           m_site->setElevation(m_weatherFileElevation);
                                         } else {
                                           m_site->resetElevation();
                                         }
                                       }
                                     }
                                     return result;
                                   }),
                                   boost::optional<NoFailAction>([this] { m_site->resetKeepSiteLocationInformation(); }),
                                   boost::optional<BasicQuery>([this] { return m_site->isKeepSiteLocationInformationDefaulted(); }));

      siteInfoGridLayout->addWidget(label, i, 0);
      siteInfoGridLayout->addWidget(m_keepSiteLocationInfo, i++, 1);
    }
    {
      label = new QLabel(tr(ELEVATION));
      label->setToolTip(tr("Elevation affects the wind speed at the site, and is defaulted to the Weather File's elevation"));

      m_elevation = new OSQuantityEdit2("m", "m", "ft", m_isIP);
      connect(this, &LocationView::toggleUnitsClicked, m_elevation, &OSQuantityEdit2::onUnitSystemChange);

      // Bind is delayed until after update() is called, so that the weatherFileElevation is set correctly.

      m_elevation->setFixedWidth(200);

      siteInfoGridLayout->addWidget(label, i, 0);
      siteInfoGridLayout->addWidget(m_elevation, i++, 1);
    }
    // Terrain
    {
      label = new QLabel(tr("Terrain"));
      label->setToolTip(tr("Terrain affects the wind speed at the site."));

      m_terrain = new OSComboBox2();
      m_terrain->bind<std::string>(*m_site, static_cast<std::string (*)(const std::string&)>(&openstudio::toString), &model::Site::validTerrainValues,
                                   std::bind(&model::Site::terrain, m_site.get_ptr()),
                                   std::bind(&model::Site::setTerrain, m_site.get_ptr(), std::placeholders::_1),
                                   boost::optional<NoFailAction>(std::bind(&model::Site::resetTerrain, m_site.get_ptr())),
                                   boost::optional<BasicQuery>(std::bind(&model::Site::isTerrainDefaulted, m_site.get_ptr())));

      m_terrain->setFixedWidth(200);

      siteInfoGridLayout->addWidget(label, i, 0);
      siteInfoGridLayout->addWidget(m_terrain, i++, 1);
    }

    // ***** Site Info GridLayout *****
    siteInfoGridLayout->setColumnStretch(++i, 10);
    leftVLayout->addLayout(siteInfoGridLayout);
    leftVLayout->addStretch();
  }

  // ***** Climate Zones *****
  label = new QLabel(tr("Measure Tags (Optional):"));
  label->setObjectName("H2");
  leftVLayout->addWidget(label);

  label = new QLabel(tr("ASHRAE Climate Zone"));
  label->setObjectName("StandardsInfo");

  m_ashraeClimateZone = new QComboBox();
  m_ashraeClimateZone->setFixedWidth(200);

  i = 0;

  measureTagsGridLayout->addWidget(label, i, 0);
  measureTagsGridLayout->addWidget(m_ashraeClimateZone, i++, 1);

  m_ashraeClimateZone->addItem("");
  std::vector<std::string> ashraeClimateZoneValues =
    model::ClimateZones::validClimateZoneValues(model::ClimateZones::ashraeInstitutionName(), model::ClimateZones::ashraeDefaultYear());
  for (const std::string& climateZone : ashraeClimateZoneValues) {
    m_ashraeClimateZone->addItem(toQString(climateZone));
  }

  model::ClimateZone ashraeClimateZone =
    climateZones.getClimateZone(model::ClimateZones::ashraeInstitutionName(), model::ClimateZones::ashraeDefaultYear());
  if (ashraeClimateZone.empty()) {
    ashraeClimateZone = climateZones.appendClimateZone(model::ClimateZones::ashraeInstitutionName(), model::ClimateZones::ashraeDefaultYear(), "");
  }
  //ashraeClimateZone.setType(model::ClimateZones::ashraeInstitutionName(), model::ClimateZones::ashraeDocumentName(), model::ClimateZones::ashraeDefaultYear());

  std::string ashraeClimateZoneValue = ashraeClimateZone.value();
  auto idx = m_ashraeClimateZone->findText(toQString(ashraeClimateZoneValue));
  OS_ASSERT(idx != -1);
  m_ashraeClimateZone->setCurrentIndex(idx);

  connect(m_ashraeClimateZone, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this,
          &LocationView::onASHRAEClimateZoneChanged);

  label = new QLabel(tr("CEC Climate Zone"));
  label->setObjectName("StandardsInfo");

  m_cecClimateZone = new QComboBox();
  m_cecClimateZone->setFixedWidth(200);

  measureTagsGridLayout->addWidget(label, i, 0);
  measureTagsGridLayout->addWidget(m_cecClimateZone, i++, 1);

  m_cecClimateZone->addItem("");
  std::vector<std::string> cecClimateZoneValues =
    model::ClimateZones::validClimateZoneValues(model::ClimateZones::cecInstitutionName(), model::ClimateZones::cecDefaultYear());
  for (const std::string& climateZone : cecClimateZoneValues) {
    m_cecClimateZone->addItem(toQString(climateZone));
  }

  model::ClimateZone cecClimateZone = climateZones.getClimateZone(model::ClimateZones::cecInstitutionName(), model::ClimateZones::cecDefaultYear());
  if (cecClimateZone.empty()) {
    cecClimateZone = climateZones.appendClimateZone(model::ClimateZones::cecInstitutionName(), model::ClimateZones::cecDefaultYear(), "");
  }
  //cecClimateZone.setType(model::ClimateZones::cecInstitutionName(), model::ClimateZones::cecDocumentName(), model::ClimateZones::cecDefaultYear());

  std::string cecClimateZoneValue = cecClimateZone.value();
  idx = m_cecClimateZone->findText(toQString(cecClimateZoneValue));
  OS_ASSERT(idx != -1);
  m_cecClimateZone->setCurrentIndex(idx);

  connect(m_cecClimateZone, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this,
          &LocationView::onCECClimateZoneChanged);

  // ***** Measure Tags GridLayout *****
  measureTagsGridLayout->setColumnStretch(i, 10);
  leftVLayout->addLayout(measureTagsGridLayout);
  leftVLayout->addStretch();

  // ***** Schedules *****
  m_yearSettingsWidget = new YearSettingsWidget(m_model);
  schedulesLayout->addWidget(m_yearSettingsWidget);

  connect(m_yearSettingsWidget, &YearSettingsWidget::calendarYearSelected, this, &LocationView::setCalendarYear);

  connect(m_yearSettingsWidget, &YearSettingsWidget::firstDayofYearSelected, this, &LocationView::setFirstDayofYear);

  connect(m_yearSettingsWidget, &YearSettingsWidget::daylightSavingTimeClicked, this, &LocationView::setDaylightSavingsTime);

  connect(m_yearSettingsWidget, &YearSettingsWidget::dstStartDayOfWeekAndMonthChanged, this, &LocationView::setDstStartDayOfWeekAndMonth);

  connect(m_yearSettingsWidget, &YearSettingsWidget::dstStartDateChanged, this, &LocationView::setDstStartDate);

  connect(m_yearSettingsWidget, &YearSettingsWidget::dstEndDayOfWeekAndMonthChanged, this, &LocationView::setDstEndDayOfWeekAndMonth);

  connect(m_yearSettingsWidget, &YearSettingsWidget::dstEndDateChanged, this, &LocationView::setDstEndDate);

  // ***** Add Vertical Layout *****
  upperHorizontalLayout->addLayout(leftVLayout);

  // ***** Add Vertical Line *****
  upperHorizontalLayout->addWidget(vLine);

  // ***** Add Schedules Layout *****
  upperHorizontalLayout->addLayout(schedulesLayout);

  // ***** Add Upper Horizontal Layout *****
  scrollLayout->addLayout(upperHorizontalLayout);

  // ***** Add Main Horizontal Line *****
  scrollLayout->addWidget(mainHLine);

  // ***** Design Days *****
  label = new QLabel(tr("Design Days"));
  label->setObjectName("H2");

  auto* btn = new QPushButton(tr("Import From DDY"), this);
  btn->setFlat(true);
  btn->setObjectName("StandardGrayButton");
  connect(btn, &QPushButton::clicked, this, &LocationView::onDesignDayBtnClicked);

  hLayout = new QHBoxLayout();
  hLayout->setContentsMargins(7, 7, 0, 7);
  hLayout->setSpacing(7);

  hLayout->addWidget(label, 0, Qt::AlignLeft);
  hLayout->addWidget(btn, 0, Qt::AlignLeft);
  hLayout->addStretch();

  scrollLayout->addLayout(hLayout);

  m_designDaysGridView = new DesignDayGridView(m_isIP, model, this->parentWidget());
  connect(this, &LocationView::toggleUnitsClicked, this, &LocationView::toggleUnits);
  connect(this, &LocationView::toggleUnitsClicked, m_designDaysGridView, &DesignDayGridView::toggleUnitsClicked);

  scrollLayout->addWidget(m_designDaysGridView, 1, Qt::AlignTop);

  // ***** Item Selector Buttons *****
  m_itemSelectorButtons = new OSItemSelectorButtons();
  m_itemSelectorButtons->hideDropZone();
  mainLayout->addWidget(m_itemSelectorButtons, 0, Qt::AlignBottom);

  connect(m_itemSelectorButtons, &OSItemSelectorButtons::addClicked, m_designDaysGridView, &DesignDayGridView::onAddClicked);
  connect(m_itemSelectorButtons, &OSItemSelectorButtons::copyClicked, m_designDaysGridView, &DesignDayGridView::onCopyClicked);
  connect(m_itemSelectorButtons, &OSItemSelectorButtons::removeClicked, m_designDaysGridView, &DesignDayGridView::onRemoveClicked);
  connect(m_itemSelectorButtons, &OSItemSelectorButtons::purgeClicked, m_designDaysGridView, &DesignDayGridView::onPurgeClicked);

  update();
  {
    m_elevation->bind(m_isIP, *m_site, DoubleGetter([this] { return m_site->elevation(); }), boost::optional<DoubleSetter>([this](double d) {
                        // turn keep site info on
                        m_site->setKeepSiteLocationInformation(true);
                        return m_site->setElevation(d);
                      }),
                      boost::optional<NoFailAction>([this] {
                        // turn keep site info off
                        m_site->setKeepSiteLocationInformation(false);

                        // force the style to update
                        m_elevation->clearCachedText();

                        if (std::abs(m_weatherFileElevation) > 0.01) {
                          m_site->setElevation(m_weatherFileElevation);
                        } else {
                          m_site->resetElevation();
                        }
                      }),
                      boost::none,                          // autosize
                      boost::none,                          // autocalculate
                      boost::optional<BasicQuery>([this] {  //
                        return (m_site->isElevationDefaulted() || !m_site->keepSiteLocationInformation());
                      }));
  }

  onSelectItem();
}

LocationView::~LocationView() {
  // m_terrain->unbind();  // NOTE: I don't think this is necessary
  saveQSettings();
}

bool LocationView::calendarYearChecked() {
  if (m_yearSettingsWidget != nullptr) {
    return m_yearSettingsWidget->calendarYearChecked();
  }

  return false;
}

std::set<model::ModelObject> LocationView::selectedObjects() const {
  return m_designDaysGridView->selectedObjects();
}

void LocationView::onSelectModelObject(const openstudio::model::ModelObject& modelObject) {}

void LocationView::onUpdate() {}

void LocationView::refresh() {}

void LocationView::toggleUnits(bool isIP) {
  m_isIP = isIP;
}

void LocationView::onSiteNameChanged(const QString& text) {
  auto temp = m_site->setName(text.toStdString());
  if (!temp) {
    m_siteName->setText("");
  } else if (QString(temp.get().c_str()) != text) {
    m_siteName->setText(temp.get().c_str());
  }
}

void LocationView::loadQSettings() {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  m_lastEpwPathOpened = settings.value("m_lastEpwPathOpened").toString();
  m_lastDdyPathOpened = settings.value("m_lastDdyPathOpened").toString();
}

void LocationView::saveQSettings() const {
  QString organizationName = QCoreApplication::organizationName();
  QString applicationName = QCoreApplication::applicationName();
  QSettings settings(organizationName, applicationName);
  settings.setValue("m_lastEpwPathOpened", m_lastEpwPathOpened);
  settings.setValue("m_lastDdyPathOpened", m_lastDdyPathOpened);
}

void LocationView::update() {
  boost::optional<model::WeatherFile> weatherFile = m_model.getOptionalUniqueModelObject<model::WeatherFile>();
  if (weatherFile) {

    auto fileExists = QFile(m_lastEpwPathOpened).exists();
    if (!fileExists) {

      boost::optional<openstudio::path> epwPath = weatherFile->path();
      if (epwPath) {
        // If there is a path, and a file at that path, our job is done
        fileExists = QFile(epwPath->string().c_str()).exists();
        if (!fileExists) {
          // Construct the absolute path as dictated by the osm location, and check for the file
          openstudio::OSAppBase* app = OSAppBase::instance();
          if ((app != nullptr) && app->currentDocument()) {

            QString savePath = app->currentDocument()->savePath();
            QString filePath;

            if (savePath.contains(".osm")) {
              savePath.chop(4);
              filePath = savePath + "/files/";
              filePath += (epwPath.get().filename()).string().c_str();
            }

            fileExists = QFile(filePath).exists();
          }

          if (!fileExists) {
            // check the temp dir, this is required when launching the os app with a filename since app->currentDocument() is not set when constructing this tab
            QString filePath = m_modelTempDir + "/resources/files/";
            filePath += (epwPath.get().filename()).string().c_str();

            fileExists = QFile(filePath).exists();
          }
        }
      }
    }

    if (fileExists) {
      m_weatherFileBtn->setText(tr("Change Weather File"));
      m_weatherFileElevation = weatherFile->elevation();
      setSiteInfo();
    } else {
      m_weatherFileBtn->setText(tr("Set Weather File"));
      clearSiteInfo();
    }
  } else {
    m_weatherFileBtn->setText(tr("Set Weather File"));
    clearSiteInfo();
  }
}

void LocationView::setSiteInfo() {
  QString info = tr(LATITUDE);
  QString temp;

  if (m_site->name() && !m_site->name()->empty()) {
    m_siteName->setText(m_site->name().get().c_str());
  } else {
    m_siteName->setText("");
  }

  temp.setNum(m_site->latitude());
  info += temp;
  m_latitudeLbl->setText(info);

  info = tr(LONGITUDE);
  temp.setNum(m_site->longitude());
  info += temp;
  m_longitudeLbl->setText(info);

  info = tr(TIME_ZONE);
  temp.setNum(m_site->timeZone());
  info += temp;
  m_timeZoneLbl->setText(info);
}

void LocationView::clearSiteInfo() {
  m_siteName->setText("");

  m_latitudeLbl->setText(tr(LATITUDE));

  m_longitudeLbl->setText(tr(LONGITUDE));

  m_timeZoneLbl->setText(tr(TIME_ZONE));

  m_weatherFileElevation = 0.0;
}

// ***** SLOTS *****
void LocationView::onWeatherFileBtnClicked() {
  QString fileTypes(tr("EPW Files (*.epw);; All Files (*.*)"));

  QString lastPath = m_lastEpwPathOpened;
  if (lastPath.isEmpty() && m_lastDdyPathOpened.isEmpty()) {
    //openstudio::runmanager::ConfigOptions co(true);
    //lastPath = toQString(co.getDefaultEPWLocation().native());
  } else if (lastPath.isEmpty()) {
    QString path = m_lastDdyPathOpened;
    lastPath = path.replace(".ddy", ".epw");
  }

  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Weather File"), lastPath, fileTypes);
  if (!fileName.isEmpty()) {

    openstudio::path epwPath = toPath(fileName);
    openstudio::path newPath = toPath(m_modelTempDir) / toPath("resources/files") / epwPath.filename();
    openstudio::path previousEPWPath;

    StringStreamLogSink ss;
    ss.setChannelRegex(boost::regex(".*EpwFile.*"));
    ss.setLogLevel(Error);

    try {

      boost::optional<openstudio::model::WeatherFile> weatherFile = m_model.getOptionalUniqueModelObject<model::WeatherFile>();

      if (weatherFile) {
        boost::optional<openstudio::path> temp = weatherFile->path();
        if (temp) {
          openstudio::path previousEPWName = temp->filename();
          //std::string ex = toString(previousEPWName.extension());
          if (!previousEPWName.empty() && previousEPWName.extension() == toPath(".epw")) {
            previousEPWPath = toPath(m_modelTempDir) / toPath("resources/files") / previousEPWName;
          }
        }
      }

      // duplicate code in OSDocument::fixWeatherFilePath

      openstudio::filesystem::copy_file(epwPath, newPath, openstudio::filesystem::copy_options::overwrite_existing);

      // this can throw
      EpwFile epwFile(newPath);

      double totalDays = (epwFile.endDate() - epwFile.startDate()).totalDays() + 1;
      if (totalDays > 366) {
        LOG_FREE(Error, "openstudio.EpwFile", "Cannot accept weather file with more than 366 days of data");
        throw openstudio::Exception("Cannot accept weather file with more than 366 days of data");
      }

      weatherFile = openstudio::model::WeatherFile::setWeatherFile(m_model, epwFile);
      OS_ASSERT(weatherFile);
      weatherFile->makeUrlRelative(toPath(m_modelTempDir) / toPath("resources/files"));

      m_model.workflowJSON().setWeatherFile(newPath.filename());

      if (!previousEPWPath.empty()) {
        if (previousEPWPath.filename() != newPath.filename()) {
          if (openstudio::filesystem::exists(previousEPWPath)) {
            openstudio::filesystem::remove_all(previousEPWPath);
          }
        }
      }

      // set run period based on weather file
      openstudio::model::RunPeriod runPeriod = m_model.getUniqueModelObject<openstudio::model::RunPeriod>();
      runPeriod.setBeginMonth(epwFile.startDate().monthOfYear().value());
      runPeriod.setBeginDayOfMonth(epwFile.startDate().dayOfMonth());
      runPeriod.setEndMonth(epwFile.endDate().monthOfYear().value());
      runPeriod.setEndDayOfMonth(epwFile.endDate().dayOfMonth());

      // set the calendar year or start day of week
      openstudio::model::YearDescription yearDescription = m_model.getUniqueModelObject<openstudio::model::YearDescription>();
      boost::optional<int> startDateActualYear = epwFile.startDateActualYear();
      if (startDateActualYear) {
        yearDescription.resetDayofWeekforStartDay();
        yearDescription.setCalendarYear(*startDateActualYear);
      } else {
        yearDescription.resetCalendarYear();
        yearDescription.setDayofWeekforStartDay(epwFile.startDayOfWeek().valueName());
      }

      // update site info
      m_site->setName(weatherFile->city());
      m_site->setLatitude(weatherFile->latitude());
      m_site->setLongitude(weatherFile->longitude());
      m_weatherFileElevation = weatherFile->elevation();
      m_site->setKeepSiteLocationInformation(false);
      m_site->setElevation(weatherFile->elevation());
      m_site->resetTerrain();
      m_site->setTimeZone(weatherFile->timeZone());

      m_lastEpwPathOpened = QFileInfo(fileName).absoluteFilePath();

      update();

    } catch (...) {

      openstudio::filesystem::remove_all(newPath);

      QMessageBox box(QMessageBox::Warning, tr("Failed To Set Weather File"), tr("Failed To Set Weather File To ") + fileName, QMessageBox::Ok);
      box.setDetailedText(toQString(ss.string()));
      box.exec();

      boost::optional<model::WeatherFile> weatherFile = m_model.weatherFile();
      if (weatherFile) {
        boost::optional<openstudio::path> weatherFilePath = weatherFile->path();
        if (weatherFilePath) {
          if (!previousEPWPath.empty()) {
            if ((previousEPWPath.filename() != weatherFilePath->filename()) || (!openstudio::filesystem::exists(previousEPWPath))) {
              weatherFile->remove();
            }
          }
        }
      }

      update();
    }
  }
}

/**
 * @brief Displays a dialog for selecting design days from a given list.
 *
 * This function creates and displays a modal dialog that allows the user to select specific design days
 * from a provided list of all available design days which are
 *  heatingPercentages "99.6%", "99%"
  *  and coolingPercentages "2%", "1%", "0.4%"
 *
 * . The dialog includes options for selecting heating
 * and cooling design days based on predefined percentages. The user can choose to import all design days,
 * select specific ones, or cancel the operation.
 *
 * @param allDesignDays A vector containing all available design days.
 * @return A vector of selected design days if the user confirms the selection, or an empty vector if the user cancels.
 */
std::vector<model::DesignDay> LocationView::showDesignDaySelectionDialog(const std::vector<openstudio::model::DesignDay>& allDesignDays) {

  std::vector<model::DesignDay> result;

  // parse out the design day names into SortableDesignDays and figure out the column and row names
  std::vector<SortableDesignDay> sortableDesignDays;
  std::set<QString> designDayTypes;      // rows
  std::set<int> sortedDesignDayPermils;  // columns

  // key is designDayType + sortedDesignDayPermil, value is names of dds
  // each cell in the table has a unique key
  std::map<QString, std::vector<openstudio::model::DesignDay>> designDayMap;
  size_t numUnknownType = 0;
  for (const auto& dd : allDesignDays) {
    SortableDesignDay sdd(dd);

    // skip Design Days with unknown type
    if (sdd.type().isEmpty()) {
      ++numUnknownType;
      continue;
    }

    sortableDesignDays.push_back(sdd);
    designDayTypes.insert(sdd.type());
    sortedDesignDayPermils.insert(sdd.sortablePermil());
    QString key = SortableDesignDay::key(sdd.type(), sdd.sortablePermil());
    if (!designDayMap.contains(key)) {
      designDayMap[key] = std::vector<openstudio::model::DesignDay>();
    }
    designDayMap[key].push_back(dd);
  }

  // main dialog
  QDialog dialog(this, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
  dialog.setWindowTitle(QCoreApplication::translate("LocationView", "Import Design Days"));
  dialog.setMinimumWidth(450);
  dialog.setModal(true);
  dialog.setStyleSheet("background: #E6E6E6;");

  auto* layout = new QVBoxLayout(&dialog);

  // grid view for the design day types and permils to import
  auto* gridLayout = new QGridLayout();

  // first row is for headers
  int row = 0;

  auto msg = tr("There are <span style=\"font-weight:bold;\">%1</span> Design Days available for import").arg(QString::number(allDesignDays.size()));
  if (numUnknownType > 0) {
    msg += tr(", %1 of which are unknown type").arg(QString::number(numUnknownType));
  }

  auto* numInfo = new QLabel(msg);
  gridLayout->addWidget(numInfo, row, 0, 1, -1, Qt::AlignCenter);

  ++row;
  int column = 1;
  for (const auto& sddp : sortedDesignDayPermils) {
    auto* header = new QLabel(SortableDesignDay::permilToQString(sddp) + "%");
    header->setStyleSheet("font-weight: bold;");
    gridLayout->addWidget(header, row, column++, Qt::AlignCenter);
  }

  // one row for each design day type
  ++row;
  QVector<QRadioButton*> allRadioButtons;
  for (const auto& ddt : designDayTypes) {
    column = 0;
    bool checkedFirst = false;
    auto* rowHeader = new QLabel();
    if (ddt == "Heating") {
      rowHeader->setText(tr("Heating"));
      rowHeader->setStyleSheet("font-weight: bold; color: #EF1C21;");
    } else if (ddt == "Cooling") {
      rowHeader->setText(tr("Cooling"));
      rowHeader->setStyleSheet("font-weight: bold; color: #0071BD;");
    } else {
      rowHeader->setText(ddt);
    }
    gridLayout->addWidget(rowHeader, row, column++, Qt::AlignCenter);

    auto* buttonGroup = new QButtonGroup(gridLayout);
    for (const auto& sddp : sortedDesignDayPermils) {
      QString key = SortableDesignDay::key(ddt, sddp);
      auto* radioButton = new QRadioButton();
      allRadioButtons.append(radioButton);
      if (!designDayMap.contains(key)) {
        radioButton->setEnabled(false);
        radioButton->setCheckable(false);
        radioButton->setToolTip(QString::number(0) + " " + tr("Design Days"));
        radioButton->setProperty("designDayKey", "");
      } else {
        radioButton->setEnabled(true);
        radioButton->setCheckable(true);
        if (!checkedFirst) {
          radioButton->setChecked(true);
          checkedFirst = true;
        }
        radioButton->setToolTip(QString::number(designDayMap[key].size()) + " " + tr("Design Days"));
        radioButton->setProperty("designDayKey", key);
      }
      buttonGroup->addButton(radioButton);
      gridLayout->addWidget(radioButton, row, column++, Qt::AlignCenter);
    }
    ++row;
  }
  layout->addLayout(gridLayout);
  int columnCount = gridLayout->columnCount();
  int rowCount = gridLayout->rowCount();

  // ok button only imports the checked design days
  auto* okButton = new QPushButton(tr("OK"), &dialog);
  connect(okButton, &QPushButton::clicked, [&dialog, &result, &allRadioButtons, &designDayMap]() {
    for (const auto& rb : allRadioButtons) {
      if (rb->isChecked()) {
        QString key = rb->property("designDayKey").toString();
        if (!key.isEmpty() && designDayMap.contains(key)) {
          for (const auto& dd : designDayMap[key]) {
            result.push_back(dd);
          }
        }
      }
    }
    dialog.accept();
  });

  // cancel button imports nothing
  auto* cancelButton = new QPushButton(tr("Cancel"), &dialog);
  connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

  // import all imports everything
  auto* importAllButton = new QPushButton(tr("Import all"), &dialog);
  connect(importAllButton, &QPushButton::clicked, [&dialog, &result, &allDesignDays]() {
    result = allDesignDays;
    dialog.accept();
  });

  // add all the buttons in a button box
  auto* buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
  buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
  buttonBox->addButton(importAllButton, QDialogButtonBox::YesRole);
  layout->addWidget(buttonBox);

  // Execute the dialog and wait for user interaction
  dialog.exec();
  return result;
}

void LocationView::onDesignDayBtnClicked() {
  QString fileTypes("Files (*.ddy)");

  QString lastPath = m_lastDdyPathOpened;
  if (lastPath.isEmpty() && m_lastEpwPathOpened.isEmpty()) {
    //openstudio::runmanager::ConfigOptions co(true);
    //lastPath = toQString(co.getDefaultEPWLocation().native());
  } else if (lastPath.isEmpty()) {
    QString path = m_lastEpwPathOpened;
    lastPath = path.replace(".epw", ".ddy");
  }

  QString fileName = QFileDialog::getOpenFileName(this, tr("Open DDY File"), lastPath, fileTypes);
  if (!fileName.isEmpty()) {

    boost::optional<IdfFile> ddyIdfFile = openstudio::IdfFile::load(toPath(fileName));
    if (ddyIdfFile) {

      openstudio::Workspace ddyWorkspace(StrictnessLevel::None, IddFileType::EnergyPlus);

      for (const IdfObject& idfObject : ddyIdfFile->objects()) {
        IddObjectType iddObjectType = idfObject.iddObject().type();
        if ((iddObjectType == IddObjectType::SizingPeriod_DesignDay) || (iddObjectType == IddObjectType::SizingPeriod_WeatherFileDays)
            || (iddObjectType == IddObjectType::SizingPeriod_WeatherFileConditionType)) {
          ddyWorkspace.addObject(idfObject);
        }
      }

      openstudio::energyplus::ReverseTranslator reverseTranslator;
      model::Model ddyModel = reverseTranslator.translateWorkspace(ddyWorkspace);

      // Use a heuristic based on the ddy files provided by EnergyPlus
      // Filter out the days that are not helpful.
      if (!ddyModel.objects().empty()) {

        // Evan note: do not remove existing design days
        //for (model::SizingPeriod sizingPeriod : m_model.getModelObjects<model::SizingPeriod>()){
        //  sizingPeriod.remove();
        //}

        //m_model.insertObjects(ddyModel.objects());

        std::vector<openstudio::model::DesignDay> designDaysToInsert =
          showDesignDaySelectionDialog(ddyModel.getConcreteModelObjects<model::DesignDay>());

        // Remove design days from ddyModel that are not in designDaysToInsert
        for (auto& designDay : ddyModel.getConcreteModelObjects<model::DesignDay>()) {
          if (std::find(designDaysToInsert.begin(), designDaysToInsert.end(), designDay) == designDaysToInsert.end()) {
            designDay.remove();
          }
        }

        m_model.insertObjects(ddyModel.objects());

        m_lastDdyPathOpened = QFileInfo(fileName).absoluteFilePath();
      }
    }
  }
}

void LocationView::checkNumDesignDays() {
  if (m_model.getModelObjects<model::SizingPeriod>().empty()) {
    QMessageBox box(QMessageBox::Warning, tr("No Design Days in DDY File"),
                    tr("This DDY file does not contain any valid design days.  Check the DDY file itself for errors or omissions."), QMessageBox::Ok);
    box.exec();
  }
}

void LocationView::onASHRAEClimateZoneChanged(const QString& climateZone) {
  auto climateZones = m_model.getUniqueModelObject<model::ClimateZones>();

  model::ClimateZone ashraeClimateZone =
    climateZones.getClimateZone(model::ClimateZones::ashraeInstitutionName(), model::ClimateZones::ashraeDefaultYear());
  ashraeClimateZone.setValue(toString(climateZone));
}

void LocationView::onCECClimateZoneChanged(const QString& climateZone) {
  auto climateZones = m_model.getUniqueModelObject<model::ClimateZones>();
  model::ClimateZone cecClimateZone = climateZones.getClimateZone(model::ClimateZones::cecInstitutionName(), model::ClimateZones::cecDefaultYear());
  cecClimateZone.setValue(toString(climateZone));
}

void LocationView::setCalendarYear(int year) {
  m_yearDescription->setCalendarYear(year);

  emit calendarYearSelectionChanged();
}

void LocationView::setFirstDayofYear(const QString& firstDayofYear) {
  m_yearDescription->resetCalendarYear();

  auto idx = YearSettingsWidget::validDayofWeekforStartDay().indexOf(firstDayofYear);
  m_yearDescription->setDayofWeekforStartDay(model::YearDescription::validDayofWeekforStartDayValues()[idx]);

  emit calendarYearSelectionChanged();
}

void LocationView::setDaylightSavingsTime(bool enabled) {
  if (enabled) {
    m_model.getUniqueModelObject<model::RunPeriodControlDaylightSavingTime>();
  } else {
    if (boost::optional<model::RunPeriodControlDaylightSavingTime> dst =
          m_model.getOptionalUniqueModelObject<model::RunPeriodControlDaylightSavingTime>()) {
      dst->remove();
    }
  }
}

void LocationView::setDstStartDayOfWeekAndMonth(int newWeek, int newDay, int newMonth) {
  auto dst = m_model.getUniqueModelObject<model::RunPeriodControlDaylightSavingTime>();

  dst.setStartDate(NthDayOfWeekInMonth(newWeek), DayOfWeek(newDay), MonthOfYear(newMonth));
}

void LocationView::setDstStartDate(const QDate& newdate) {
  auto dst = m_model.getUniqueModelObject<model::RunPeriodControlDaylightSavingTime>();

  dst.setStartDate(MonthOfYear(newdate.month()), newdate.day());
}

void LocationView::setDstEndDayOfWeekAndMonth(int newWeek, int newDay, int newMonth) {
  auto dst = m_model.getUniqueModelObject<model::RunPeriodControlDaylightSavingTime>();

  dst.setEndDate(NthDayOfWeekInMonth(newWeek), DayOfWeek(newDay), MonthOfYear(newMonth));
}

void LocationView::setDstEndDate(const QDate& newdate) {
  auto dst = m_model.getUniqueModelObject<model::RunPeriodControlDaylightSavingTime>();

  dst.setEndDate(MonthOfYear(newdate.month()), newdate.day());
}

void LocationView::onSelectItem() {
  m_itemSelectorButtons->enableCopyButton();
  m_itemSelectorButtons->enableRemoveButton();
  m_itemSelectorButtons->enablePurgeButton();
}

void LocationView::onClearSelection() {
  m_itemSelectorButtons->disableCopyButton();
  m_itemSelectorButtons->disableRemoveButton();
  m_itemSelectorButtons->disablePurgeButton();
}

}  // namespace openstudio
