/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_LOCALLIBRARYVIEW_HPP
#define SHAREDGUICOMPONENTS_LOCALLIBRARYVIEW_HPP

#include "OSDragableView.hpp"

#include "HeaderViews.hpp"

#include <QPoint>
#include <QWidget>

class QPushButton;
class QLabel;

namespace openstudio {

class MeasureBadge;
class OSViewSwitcher;

class LocalLibraryView : public QWidget
{
  Q_OBJECT

 public:
  explicit LocalLibraryView(QWidget* parent = nullptr);

  virtual ~LocalLibraryView() {}

  OSViewSwitcher* mainViewSwitcher;

  QPushButton* duplicateMeasureButton;

  QPushButton* addMeasureButton;

  QPushButton* myMeasuresFolderButton;

  QPushButton* checkForUpdateButton;

  QPushButton* addBCLMeasureButton;
};

class LibraryGroupItemHeader : public LightGradientHeader
{
  Q_OBJECT

 public:
  LibraryGroupItemHeader();

  virtual ~LibraryGroupItemHeader() {}

 public slots:

  void setCount(int count);

 private:
  QLabel* countLabel;
};

class LibrarySubGroupItemHeader : public LightHeader
{
  Q_OBJECT

 public:
  LibrarySubGroupItemHeader();

  virtual ~LibrarySubGroupItemHeader() {}

 public slots:

  void setCount(int count);

 private:
  QLabel* countLabel;
};

class LibraryItemView : public OSDragableView
{
  Q_OBJECT

 public:
  explicit LibraryItemView(QWidget* parent = nullptr);

  virtual ~LibraryItemView() {}

  QLabel* label;
  QLabel* m_measureTypeBadge;
  MeasureBadge* m_measureBadge;
  QLabel* errorLabel;

 public slots:

  void setHasEmphasis(bool hasEmphasis);

 protected:
  void paintEvent(QPaintEvent*) override;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_LOCALLIBRARYVIEW_HPP
