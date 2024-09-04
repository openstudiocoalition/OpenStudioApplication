/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCRIPTSTABCONTROLLER_HPP
#define OPENSTUDIO_SCRIPTSTABCONTROLLER_HPP

#include <openstudio/utilities/core/Path.hpp>

#include "MainTabController.hpp"
#include "ScriptsTabView.hpp"
#include <QPointer>

namespace openstudio {

namespace measuretab {

class WorkflowController;
class WorkflowSectionItemDelegate;
}  // namespace measuretab

class ScriptsTabController : public MainTabController
{
  Q_OBJECT

 public:
  ScriptsTabController();

  virtual ~ScriptsTabController();

 private:
  QPointer<ScriptsTabView> scriptsTabView;

 private:
  REGISTER_LOGGER("openstudio.measuretab.MeasuresTabController");

  QSharedPointer<measuretab::WorkflowController> m_workflowController;
  QSharedPointer<measuretab::WorkflowSectionItemDelegate> m_workflowSectionItemDelegate;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCRIPTSTABCONTROLLER_HPP
