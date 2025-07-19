/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_BUILDINGCOMPONENTDIALOGCENTRALWIDGET_HPP
#define SHAREDGUICOMPONENTS_BUILDINGCOMPONENTDIALOGCENTRALWIDGET_HPP

#include <QElapsedTimer>
#include <QWidget>

#include <optional>
#include <queue>
#include <vector>

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/utilities/bcl/BCLComponent.hpp>
#include <openstudio/utilities/bcl/BCLMeasure.hpp>

#include "../shared_gui_components/ProgressBarWithError.hpp"

#include <boost/optional.hpp>

class QTimer;

namespace openstudio {

class Component;
class ComponentList;
class CollapsibleComponentList;
class RemoteBCL;

class BuildingComponentDialogCentralWidget
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit BuildingComponentDialogCentralWidget(QWidget* parent = nullptr);
  explicit BuildingComponentDialogCentralWidget(int tid, QWidget* parent = nullptr);
  virtual ~BuildingComponentDialogCentralWidget();
  int tid() const;
  void setTid(const std::string& filterType, int tid, int pageIdx, const QString& title, const QString& searchString = "");
  Component* checkedComponent() const;
  int pageIdx() const;
  bool showNewComponents() const;
  void setShowNewComponents(bool showNewComponents);

 protected:
 private:
  void createLayout();
  void init();
  void setTid();
  void componentDownloadComplete(const std::string& uid, const boost::optional<BCLComponent>& component);
  void measureDownloadComplete(const std::string& uid, const boost::optional<BCLMeasure>& measure);

  int m_tid;
  CollapsibleComponentList* m_collapsibleComponentList;
  ComponentList* m_componentList;  // TODO cruft to be removed
  ProgressBarWithError* m_progressBar;
  std::queue<std::pair<std::string, std::string>> m_pendingDownloads;
  std::string m_filterType;
  int m_pageIdx;
  QString m_searchString;
  bool m_showNewComponents;
  std::shared_ptr<RemoteBCL> m_remoteBCL;
  QTimer* m_timer;
  QElapsedTimer m_downloadTimer;
  std::optional<std::pair<std::string, std::string>> m_currentDownload;
  unsigned m_totalPendingDownloads = 0;
  unsigned m_timeoutSeconds;

 signals:
  void headerClicked(bool checked);
  void componentClicked(bool checked);
  void collapsibleComponentClicked(bool checked);
  void getComponentsByPage(int pageNum);
  void componentsReady();
  void noComponents();
  void requestComponents(const std::string& filterType, int tids, int pageIdx, const QString& searchString);

 private slots:
  void upperPushButtonClicked();
  void lowerPushButtonClicked();
  void comboBoxIndexChanged(const QString& text);
  void on_headerClicked(bool checked);
  void on_componentClicked(bool checked);
  void on_collapsibleComponentClicked(bool checked);
  void on_getComponentsByPage(int pageIdx);
  void downloadNextComponent();
  void clearPendingDownloads(bool failed);
  void downloadFailed(const std::string& uid);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_BUILDINGCOMPONENTDIALOGCENTRALWIDGET_HPP
