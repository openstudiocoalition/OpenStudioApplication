/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSINSPECTORVIEW_HPP
#define OPENSTUDIO_OSINSPECTORVIEW_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/optional.hpp>
#include <QWidget>

class QLabel;
class QStackedWidget;

namespace openstudio {

class OSItem;

class OSInspectorView : public QWidget
{
  Q_OBJECT

 public:
  explicit OSInspectorView(bool addScrollArea, QWidget* parent = nullptr);

  virtual ~OSInspectorView() {}

 signals:

  void dropZoneItemClicked(OSItem* item);

 public slots:

  void clearSelection();

  void selectItem(OSItem* item);

 protected:
  virtual void onSelectItem(OSItem* item) = 0;

  virtual void onClearSelection() = 0;

  virtual void onUpdate() = 0;

  QStackedWidget* stackedWidget() const;

 protected slots:

  void update();

 private:
  QStackedWidget* m_stackedWidget;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSINSPECTORVIEW_HPP
