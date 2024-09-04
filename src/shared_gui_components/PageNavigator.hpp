/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_PAGENAVIGATOR_HPP
#define SHAREDGUICOMPONENTS_PAGENAVIGATOR_HPP

#include <QWidget>

class QAbstractButton;
class QButtonGroup;
class QColor;
class QHBoxLayout;

namespace openstudio {

class PageNavigator : public QWidget
{
  Q_OBJECT

 public:
  explicit PageNavigator(int numPages, int numPagesListed = 5, QWidget* parent = nullptr);
  virtual ~PageNavigator();
  void setNumPages(int numPages);
  void setNumPagesListed(int numPagesListed = 5);
  ///! Sets widget to display first page (pageIdx = 0)
  void firstPage();

 protected:
 private:
  void createLayout();
  ///! sets all the page numbers displayed by the buttons
  void setButtonPageNumbers(int lowestPageNum);
  void makeNumberedButtons();

  int m_numPagesListed;
  int m_numPages;
  QColor m_checkedTextColor;
  QColor m_uncheckedTextColor;
  QButtonGroup* m_buttonGroup;
  QHBoxLayout* m_buttonLayout;

 private slots:
  void on_buttonClicked(QAbstractButton* button);
  void on_firstPage(bool checked);
  void on_previousPage(bool checked);
  void on_nextPage(bool checked);
  void on_lastPage(bool checked);

 signals:
  void getComponentsByPage(int pageNum);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_PAGENAVIGATOR_HPP
