/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2021, OpenStudio Coalition and other contributors. All rights reserved.
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

#include <boost/optional.hpp>

class QProgressBar;
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
  BuildingComponentDialogCentralWidget(QWidget* parent = nullptr);
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
  QProgressBar* m_progressBar;
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
