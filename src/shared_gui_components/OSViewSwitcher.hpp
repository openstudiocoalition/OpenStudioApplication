/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSVIEWSWITCHER_HPP
#define SHAREDGUICOMPONENTS_OSVIEWSWITCHER_HPP

#include <QSharedPointer>
#include <QWidget>

class QStackedWidget;

namespace openstudio {

class OSViewSwitcher : public QWidget
{
  Q_OBJECT

 public:
  explicit OSViewSwitcher(QWidget* parent = nullptr);

  virtual ~OSViewSwitcher();

  void setView(QWidget* view);

  QWidget* view() const;

  void clear();

 protected:
  void paintEvent(QPaintEvent*) override;

 private:
  QWidget* m_view;

  QStackedWidget* m_stack;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSVIEWSWITCHER_HPP
