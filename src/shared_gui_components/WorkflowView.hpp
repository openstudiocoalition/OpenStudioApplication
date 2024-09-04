/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_WORKFLOWVIEW_HPP
#define SHAREDGUICOMPONENTS_WORKFLOWVIEW_HPP

#include "OSCollapsibleView.hpp"
#include "OSListView.hpp"
#include "HeaderViews.hpp"
#include <QAbstractButton>

class QLineEdit;
class QLabel;
class QPushButton;

namespace openstudio {

class MeasureType;

namespace measuretab {

class WorkflowSectionContentView;
class WorkflowStepButton;

}  // namespace measuretab

namespace measuretab {

// RectangularDropZone styles a rectangular drop zone
class RectangularDropZone : public QWidget
{
  Q_OBJECT

 public:
  RectangularDropZone();

  virtual ~RectangularDropZone() = default;

  void setAcceptedMimeType(const QString& type);

  QLabel* nameLabel;

 signals:

  void dataDropped(QDropEvent* event);

 protected:
  void paintEvent(QPaintEvent* event) override;

  void dropEvent(QDropEvent* event) override;

  void dragEnterEvent(QDragEnterEvent* event) override;

 private:
  QString m_acceptedMimeType;
};

// Accepts drop of a measure and emits signal which is connected by VariableGroupItemDelegate
class NewMeasureDropZone : public RectangularDropZone
{
  Q_OBJECT

 public:
  NewMeasureDropZone();

  virtual ~NewMeasureDropZone() = default;
};

// WorkflowSectionView displays a WorkflowSectionItem, e.g. all the Model or EnergyPlus Measures
class WorkflowSectionView : public OSCollapsibleView
{
  Q_OBJECT

 public:
  explicit WorkflowSectionView(const MeasureType& measureType);

  virtual ~WorkflowSectionView() = default;

  LightGradientHeader* header;

  WorkflowSectionContentView* content;
};

class WorkflowSectionContentView : public QWidget
{
  Q_OBJECT

 public:
  WorkflowSectionContentView();

  virtual ~WorkflowSectionContentView() = default;

  OSListView* workflowStepsView;

  NewMeasureDropZone* newMeasureDropZone;
};

// WorkflowStepView displays a WorkflowStepItem
class WorkflowStepView : public QWidget
{
  Q_OBJECT

 public:
  WorkflowStepView();

  virtual ~WorkflowStepView() = default;

  WorkflowStepButton* workflowStepButton;

  QPushButton* duplicateButton;

  QPushButton* removeButton;

  QPushButton* upButton;

  QPushButton* downButton;

 protected:
  void paintEvent(QPaintEvent*) override;
};

class WorkflowStepButton : public QAbstractButton
{
  Q_OBJECT

 public:
  WorkflowStepButton();

  virtual ~WorkflowStepButton() = default;

  QLabel* measureTypeBadge;

  QLabel* nameLabel;

  QLabel* cautionLabel;

  QLabel* errorLabel;

 public slots:

  void setHasEmphasis(bool hasEmphasis);

 protected:
  void paintEvent(QPaintEvent* event) override;
};

}  // namespace measuretab

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_WORKFLOWVIEW_HPP
