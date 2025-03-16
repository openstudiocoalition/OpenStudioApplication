/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "BuildingComponentDialogCentralWidget.hpp"
#include "BuildingComponentDialog.hpp"
#include "CollapsibleComponent.hpp"
#include "CollapsibleComponentHeader.hpp"
#include "CollapsibleComponentList.hpp"
#include "Component.hpp"
#include "ComponentList.hpp"
#include "BaseApp.hpp"
#include "MeasureManager.hpp"

#include <cstddef>
#include <openstudio/measure/OSArgument.hpp>
#include <openstudio/utilities/bcl/BCL.hpp>
#include <openstudio/utilities/bcl/LocalBCL.hpp>
#include <openstudio/utilities/bcl/RemoteBCL.hpp>
#include <openstudio/utilities/core/Assert.hpp>

#include "../model_editor/Application.hpp"
#include "../openstudio_lib/OSAppBase.hpp"
#include "../openstudio_lib/OSDocument.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QElapsedTimer>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

namespace openstudio {

BuildingComponentDialogCentralWidget::BuildingComponentDialogCentralWidget(QWidget* parent)
  : QWidget(parent),
    m_tid(0),
    m_collapsibleComponentList(nullptr),
    m_componentList(nullptr),  // TODO cruft to be removed
    m_progressBar(nullptr),
    m_pageIdx(0),
    m_showNewComponents(false),
    m_remoteBCL(nullptr),
    m_timer(nullptr),
    m_timeoutSeconds(120) {
  init();
}

BuildingComponentDialogCentralWidget::BuildingComponentDialogCentralWidget(int tid, QWidget* parent)
  : QWidget(parent),
    m_tid(tid),
    m_collapsibleComponentList(nullptr),
    m_componentList(nullptr),  // TODO cruft to be removed
    m_progressBar(nullptr),
    m_pageIdx(0),
    m_showNewComponents(false),
    m_remoteBCL(nullptr),
    m_timer(nullptr),
    m_timeoutSeconds(120) {
  init();
}

BuildingComponentDialogCentralWidget::~BuildingComponentDialogCentralWidget() {
  m_timer->stop();
  clearPendingDownloads(false);
  if (m_remoteBCL) {
    m_remoteBCL->waitForComponentDownload();
  }
}

void BuildingComponentDialogCentralWidget::init() {
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &BuildingComponentDialogCentralWidget::downloadNextComponent);
  m_timer->start(500);

  createLayout();
}

void BuildingComponentDialogCentralWidget::createLayout() {

  auto* label = new QLabel("Sort by:");
  label->hide();  // TODO remove this hack when we have sorts to do

  auto* comboBox = new QComboBox(this);
  comboBox->hide();  // TODO remove this hack when we have sorts to do

  connect(comboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this,
          &BuildingComponentDialogCentralWidget::comboBoxIndexChanged);

  auto* upperPushButton = new QPushButton("Check All");
  connect(upperPushButton, &QPushButton::clicked, this, &BuildingComponentDialogCentralWidget::upperPushButtonClicked);

  auto* upperLayout = new QHBoxLayout();
  upperLayout->addWidget(label);
  upperLayout->addWidget(comboBox);
  upperLayout->addStretch();
  upperLayout->addWidget(upperPushButton);

  m_collapsibleComponentList = new CollapsibleComponentList();

  connect(m_collapsibleComponentList, &CollapsibleComponentList::headerClicked, this, &BuildingComponentDialogCentralWidget::headerClicked);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::headerClicked, this, &BuildingComponentDialogCentralWidget::on_headerClicked);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::componentClicked, this, &BuildingComponentDialogCentralWidget::componentClicked);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::componentClicked, this, &BuildingComponentDialogCentralWidget::on_componentClicked);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::collapsibleComponentClicked, this,
          &BuildingComponentDialogCentralWidget::collapsibleComponentClicked);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::collapsibleComponentClicked, this,
          &BuildingComponentDialogCentralWidget::on_collapsibleComponentClicked);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::getComponentsByPage, this,
          &BuildingComponentDialogCentralWidget::getComponentsByPage);

  connect(m_collapsibleComponentList, &CollapsibleComponentList::getComponentsByPage, this,
          &BuildingComponentDialogCentralWidget::on_getComponentsByPage);

  //*******************************************************************
  // Hack code to be removed (TODO)
  m_componentList = new ComponentList();  // TODO refactor and remove

  CollapsibleComponentHeader* collapsibleComponentHeader = nullptr;
  collapsibleComponentHeader = new CollapsibleComponentHeader("Constructions", 100, 5);

  CollapsibleComponent* collapsibleComponent = nullptr;
  collapsibleComponent = new CollapsibleComponent(collapsibleComponentHeader, m_componentList);

  m_collapsibleComponentList->addCollapsibleComponent(collapsibleComponent);
  //*******************************************************************

  m_progressBar = new ProgressBarWithError(this);
  m_progressBar->setVisible(false);

  auto* lowerPushButton = new QPushButton("Download");
  connect(lowerPushButton, &QPushButton::clicked, this, &BuildingComponentDialogCentralWidget::lowerPushButtonClicked);

  auto* lowerLayout = new QHBoxLayout();
  lowerLayout->addStretch();
  lowerLayout->addWidget(m_progressBar);
  lowerLayout->addWidget(lowerPushButton);

  auto* mainLayout = new QVBoxLayout();
  mainLayout->addLayout(upperLayout);

  mainLayout->addWidget(m_collapsibleComponentList, 0, Qt::AlignTop);
  mainLayout->addLayout(lowerLayout);
  setLayout(mainLayout);
}

int BuildingComponentDialogCentralWidget::tid() const {
  return m_tid;
}

int BuildingComponentDialogCentralWidget::pageIdx() const {
  return m_pageIdx;
}

void BuildingComponentDialogCentralWidget::setTid() {
  requestComponents(m_filterType, m_tid, m_pageIdx, m_searchString);
}

std::vector<openstudio::BCLSearchResult> BuildingComponentDialogCentralWidget::fetchAndSortResponses(const std::string& filterType, int tid, const QString& searchString) {
  m_allResponses.clear();
  RemoteBCL remoteBCL;
  std::vector<BCLSearchResult> responses;
  int totalPages = 1;
  int currentPage = 0;

  // Collect all responses from all pages
  do {
    std::vector<BCLSearchResult> pageResponses;
    if (filterType == "components") {
      pageResponses = remoteBCL.searchComponentLibrary(searchString.toStdString(), tid, currentPage);
    } else if (filterType == "measures") {
      pageResponses = remoteBCL.searchMeasureLibrary(searchString.toStdString(), tid, currentPage);
    }
    responses.insert(responses.end(), pageResponses.begin(), pageResponses.end());
    totalPages = remoteBCL.numResultPages();
  } while (++currentPage < totalPages);

  // Sort responses alphabetically by name
  std::sort(responses.begin(), responses.end(), [](const BCLSearchResult& a, const BCLSearchResult& b) {
    return a.name() < b.name();
  });

  return responses;
}

// Note: don't call this directly if the "wait" screen is desired
void BuildingComponentDialogCentralWidget::setTid(const std::string& filterType, int tid, int pageIdx, const QString& title,
                                                  const QString& searchString) {

  if (m_tid != tid || m_searchString != searchString || m_filterType != filterType) {
    m_collapsibleComponentList->firstPage();
    m_allResponses = fetchAndSortResponses(filterType, tid, searchString);
  }

  m_searchString = searchString;
  m_filterType = filterType;
  m_tid = tid;

  // Clear existing components
  std::vector<Component*> components = m_componentList->components();  // TODO replace with code above
  for (auto& comp : components) {
    delete comp;
  }

  // Paginate responses
  int itemsPerPage = 10;  // Assuming 10 items per page
  size_t startIdx = pageIdx * itemsPerPage;
  size_t endIdx = std::min(startIdx + itemsPerPage, m_allResponses.size());
  std::vector<BCLSearchResult> paginatedResponses(m_allResponses.begin() + startIdx, m_allResponses.begin() + endIdx);

  for (const auto& response : paginatedResponses) {
    auto* component = new Component(response);

    // TODO replace with a componentList owned by m_collapsibleComponentList
    m_componentList->addComponent(component);
  }

  // the parent taxonomy
  m_collapsibleComponentList->setText(title);

  // the total number of results
  int lastTotalResults = m_allResponses.size();
  m_collapsibleComponentList->setNumResults(lastTotalResults);

  // the number of pages of results
  int numResultPages = (lastTotalResults / itemsPerPage) + 1;
  m_collapsibleComponentList->setNumPages(numResultPages);

  // make sure the header is expanded
  if (m_collapsibleComponentList->checkedCollapsibleComponent()) {
    m_collapsibleComponentList->checkedCollapsibleComponent()->setExpanded(true);
  }

  // select the first component
  if (m_componentList->firstComponent()) {
    m_componentList->firstComponent()->setChecked(true);
  } else {
    emit noComponents();
  }

  emit componentsReady();
}

///! Slots

void BuildingComponentDialogCentralWidget::upperPushButtonClicked() {
  for (Component* component : m_collapsibleComponentList->components()) {
    if (component->checkBoxEnabled()) {
      component->setCheckBoxChecked(true);
    }
  }
}

void BuildingComponentDialogCentralWidget::lowerPushButtonClicked() {
  for (Component* component : m_collapsibleComponentList->components()) {
    if (component->checkBoxChecked() && component->checkBoxEnabled()) {

      std::string uid = component->uid();

      component->setCheckBoxEnabled(false);
      component->msg()->setHidden(true);

      m_pendingDownloads.emplace(uid, m_filterType);
    }
  }

  m_totalPendingDownloads = m_pendingDownloads.size();
}

void BuildingComponentDialogCentralWidget::comboBoxIndexChanged(const QString& text) {}

void BuildingComponentDialogCentralWidget::componentDownloadComplete(const std::string& uid, const boost::optional<BCLComponent>& component) {
  if (component) {
    // remove outdated components
    OSAppBase::instance()->currentDocument()->removeOutdatedLocalComponents(component->uid(), component->versionId());
  } else {
    // error downloading component
    downloadFailed(uid);
  }

  m_remoteBCL.reset();
  m_downloadTimer.invalidate();
  m_currentDownload.reset();
}

void BuildingComponentDialogCentralWidget::measureDownloadComplete(const std::string& uid, const boost::optional<BCLMeasure>& measure) {
  if (measure) {
    // remove outdated measures
    OSAppBase::instance()->currentDocument()->removeOutdatedLocalMeasures(measure->uid(), measure->versionId());
  } else {
    // error downloading measure
    downloadFailed(uid);
  }

  auto* app = dynamic_cast<BaseApp*>(Application::instance().application());
  if (app) {
    if (measure) {
      try {
        app->measureManager().getArguments(*measure);
      } catch (const std::exception&) {
      }
    }
    // DLM: handled in OSDocument::on_closeMeasuresBclDlg
    // app->measureManager().updateMeasuresLists();
  }

  m_remoteBCL.reset();
  m_downloadTimer.invalidate();
  m_currentDownload.reset();
}

Component* BuildingComponentDialogCentralWidget::checkedComponent() const {
  return m_collapsibleComponentList->checkedComponent();
}

bool BuildingComponentDialogCentralWidget::showNewComponents() const {
  return m_showNewComponents;
}

void BuildingComponentDialogCentralWidget::setShowNewComponents(bool showNewComponents) {
  m_showNewComponents = showNewComponents;
}

///! SLOTS

void BuildingComponentDialogCentralWidget::on_headerClicked(bool checked) {}

void BuildingComponentDialogCentralWidget::on_componentClicked(bool checked) {}

void BuildingComponentDialogCentralWidget::on_collapsibleComponentClicked(bool checked) {}

void BuildingComponentDialogCentralWidget::on_getComponentsByPage(int pageIdx) {
  m_pageIdx = pageIdx;
  setTid();
}

void BuildingComponentDialogCentralWidget::downloadNextComponent() {

  if (m_remoteBCL) {
    // currently doing a download
    if (m_downloadTimer.isValid()) {
      // RemoteBCL does not always call the call back for us if we timeout
      if (m_downloadTimer.elapsed() > static_cast<qint64>(1000) * m_timeoutSeconds) {
        m_timer->stop();

        m_remoteBCL->waitForComponentDownload();

        // if m_remoteBCL is not empty then call back did not happen and this was a failure
        if (m_currentDownload) {
          downloadFailed(m_currentDownload->first);
        }
        m_remoteBCL.reset();
        m_downloadTimer.invalidate();
        m_currentDownload.reset();

        if (!RemoteBCL::isOnline()) {
          // if we have gone offline cancel the remaining pending downloads
          clearPendingDownloads(true);
        }

        m_timer->start(500);
      }
    }
    return;
  }

  if (m_pendingDownloads.empty()) {
    // show done progress
    m_progressBar->setValue(0);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0);
    m_progressBar->setVisible(false);
  } else {
    m_currentDownload = m_pendingDownloads.front();
    m_pendingDownloads.pop();

    // show busy progress: we add 1 to both Value and Maximum to immediately indicate to the user that we started doing *something*
    m_progressBar->setValue(m_totalPendingDownloads - m_pendingDownloads.size());
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(m_totalPendingDownloads + 1);
    m_progressBar->setVisible(true);

    if (m_currentDownload->second == "components") {
      m_remoteBCL = std::make_shared<RemoteBCL>();
      m_remoteBCL->setTimeOutSeconds(m_timeoutSeconds);

      // Connect to Nano Signal
      m_remoteBCL->componentDownloaded
        .connect<BuildingComponentDialogCentralWidget, &BuildingComponentDialogCentralWidget::componentDownloadComplete>(
          const_cast<BuildingComponentDialogCentralWidget*>(this));

      if (m_remoteBCL->downloadComponent(m_currentDownload->first)) {
        m_downloadTimer.start();
      } else {
        m_remoteBCL.reset();
        m_currentDownload.reset();
      }

    } else if (m_currentDownload->second == "measures") {
      m_remoteBCL = std::make_shared<RemoteBCL>();
      m_remoteBCL->setTimeOutSeconds(m_timeoutSeconds);

      // Connect to Nano Signal
      m_remoteBCL->measureDownloaded.connect<BuildingComponentDialogCentralWidget, &BuildingComponentDialogCentralWidget::measureDownloadComplete>(
        const_cast<BuildingComponentDialogCentralWidget*>(this));

      if (m_remoteBCL->downloadMeasure(m_currentDownload->first)) {
        m_downloadTimer.start();
      } else {
        m_remoteBCL.reset();
        m_currentDownload.reset();
      }
    }
  }
}

void BuildingComponentDialogCentralWidget::clearPendingDownloads(bool failed) {
  while (!m_pendingDownloads.empty()) {
    if (failed) {
      const auto& uidType = m_pendingDownloads.front();
      downloadFailed(uidType.first);
    }
    m_pendingDownloads.pop();
  }
}

void BuildingComponentDialogCentralWidget::downloadFailed(const std::string& uid) {
  // find measure in list by uid and re-enable
  std::vector<Component*> components = m_collapsibleComponentList->components();
  auto it = std::find_if(components.begin(), components.end(), [&uid](auto* comp) { return comp->uid() == uid; });
  if (it != components.end()) {
    (*it)->setCheckBoxEnabled(true);
  }
}

}  // namespace openstudio
