/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_WORKFLOWCONTROLLER_HPP
#define SHAREDGUICOMPONENTS_WORKFLOWCONTROLLER_HPP

#include "OSListController.hpp"
#include "OSListView.hpp"
#include "BaseApp.hpp"
#include <openstudio/utilities/bcl/BCLMeasure.hpp>
#include <openstudio/utilities/core/Logger.hpp>
#include <openstudio/utilities/filetypes/WorkflowStep.hpp>
#include <QObject>
#include <QSharedPointer>
#include <map>

namespace openstudio {

namespace measuretab {

class WorkflowSectionItem;
class WorkflowStepController;
class MeasureStepItem;
class MeasureStepController;

}  // namespace measuretab

namespace measure {

class OSArgument;

}

namespace measuretab {

// the classes here are used in the measures tab of the App

// WorkflowController controls a list of WorkflowSectionItems
// Each WorkflowSectionItem represents a set of measures grouped by type, e.g. all the model measures
class WorkflowController : public OSListController
{
  Q_OBJECT

 public:
  explicit WorkflowController(BaseApp* t_baseApp);

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

 private:
  void addItem(QSharedPointer<OSListItem> item);

  std::vector<QSharedPointer<WorkflowSectionItem>> m_workflowSectionItems;
};

// WorkflowSectionItemDelegate views a WorkflowSectionItem and returns a WorkflowSectionView
class WorkflowSectionItemDelegate : public OSItemDelegate
{
  Q_OBJECT

 public:
  explicit WorkflowSectionItemDelegate();

  QWidget* view(QSharedPointer<OSListItem> dataSource) override;

 private:
};

// Each WorkflowSectionItem represents a set of measures grouped by type, e.g. all the model measures
// DLM: in the future this can also be used to show other types of jobs, e.g. ModelToIdf
class WorkflowSectionItem : public OSListItem
{
  Q_OBJECT

 public:
  WorkflowSectionItem(const MeasureType& measureType, const QString& label, BaseApp* t_baseApp);

  QString label() const;

  boost::optional<MeasureType> measureType() const;

  QSharedPointer<WorkflowStepController> workflowStepController() const;

 private:
  QString m_label;
  boost::optional<MeasureType> m_measureType;

  QSharedPointer<WorkflowStepController> m_workflowStepController;
};

// WorkflowStepController controls a list of WorkflowStepItems
class WorkflowStepController : public OSListController
{
  Q_OBJECT

 public:
  explicit WorkflowStepController(BaseApp* t_baseApp);
};

// MeasureStepController controls a list of MeasureStepItems
class MeasureStepController : public WorkflowStepController
{
  Q_OBJECT

 public:
  MeasureStepController(MeasureType measureType, BaseApp* t_baseApp);

  QSharedPointer<OSListItem> itemAt(int i) override;

  int count() override;

  MeasureType measureType() const;
  MeasureLanguage measureLanguage() const;

  std::vector<MeasureStep> measureSteps() const;

  void removeItemForStep(MeasureStep step);

  void moveUp(MeasureStep step);

  void moveDown(MeasureStep step);

 public slots:

  void addItemForDroppedMeasure(QDropEvent* event);

 private:
  void addItem(QSharedPointer<OSListItem> item);

  MeasureType m_measureType;
  MeasureLanguage m_measureLanguage;

  BaseApp* m_app;

  std::vector<QSharedPointer<MeasureStepItem>> m_measureStepItems;
};

// MeasureStepItemDelegate views a MeasureStepItem and returns a MeasureStepView
class MeasureStepItemDelegate : public OSItemDelegate
{
  Q_OBJECT

 public:
  explicit MeasureStepItemDelegate();

  QWidget* view(QSharedPointer<OSListItem> dataSource) override;

 private:
};

// Each MeasureStepItem represents a MeasureStep
class MeasureStepItem : public OSListItem
{
  Q_OBJECT

 public:
  MeasureStepItem(MeasureType measureType, MeasureStep step, BaseApp* t_baseApp);

  QString name() const;

  //QString displayName() const;

  MeasureType measureType() const;

  MeasureLanguage measureLanguage() const;

  MeasureStep measureStep() const;

  QString description() const;

  QString modelerDescription() const;

  QString scriptFileName() const;

  OptionalBCLMeasure bclMeasure() const;

  // arguments includes the full list of arguments calculated for the given model along with any values specified in the OSW
  std::vector<measure::OSArgument> arguments() const;

  bool hasIncompleteArguments() const;

  std::vector<measure::OSArgument> incompleteArguments() const;

 public slots:

  void remove();

  void moveUp();

  void moveDown();

  void setName(const QString& name);

  //void setDisplayName(const QString & displayName);

  void setDescription(const QString& description);

  void setArgument(const measure::OSArgument& argument);

  void setSelected(bool isSelected) override;

 signals:

  void nameChanged(const QString& name);

  //void displayNameChanged(const QString & displayName);

  void descriptionChanged();

  void argumentsChanged(bool isIncomplete);

 private:
  MeasureType m_measureType;
  MeasureLanguage m_measureLanguage;
  MeasureStep m_step;
  BaseApp* m_app;
};

}  // namespace measuretab

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_WORKFLOWCONTROLLER_HPP
