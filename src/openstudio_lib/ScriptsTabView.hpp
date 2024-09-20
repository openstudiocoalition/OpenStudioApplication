/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCRIPTSTABVIEW_HPP
#define OPENSTUDIO_SCRIPTSTABVIEW_HPP

#include "SubTabView.hpp"

class QPushButton;

namespace openstudio {

class OSListView;

class SyncMeasuresDialog;

class ScriptsTabView : public MainTabView
{
  Q_OBJECT

 public:
  explicit ScriptsTabView(QWidget* t_parent);

  virtual ~ScriptsTabView() {}

  QWidget* mainContent;

  OSListView* workflowView;

 protected:
  virtual void showEvent(QShowEvent* e) override;

 private:
  QPushButton* m_updateMeasuresButton;

  boost::shared_ptr<SyncMeasuresDialog> m_syncMeasuresDialog;

 private slots:

  void openUpdateMeasuresDlg();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCRIPTSTABVIEW_HPP
