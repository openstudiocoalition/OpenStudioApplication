/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSWIDGETHOLDER_HPP
#define SHAREDGUICOMPONENTS_OSWIDGETHOLDER_HPP

#include <QWidget>

namespace openstudio {

class GridCellLocation;
class GridCellInfo;
class OSCellWrapper;

// An OSWidgetHolder holds an OSWidget like OSLineEdit
class OSWidgetHolder : public QWidget
{
  Q_OBJECT

 public:
  OSWidgetHolder(OSCellWrapper* cellWrapper, QWidget* widget, bool isEven);

  virtual ~OSWidgetHolder();

  QWidget* widget() const;

  void setCellProperties(const GridCellLocation& location, const GridCellInfo& info);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QWidget* m_widget;
  bool m_isEven;

 signals:

  void inFocus(bool inFocus, bool hasData);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSWIDGETHOLDER_HPP
