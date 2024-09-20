/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENTHEADER_HPP
#define SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENTHEADER_HPP

#include <QAbstractButton>

#include "Component.hpp"

class QLabel;
class QMouseEvent;
class QPaintEvent;
class QVBoxLayout;

namespace openstudio {

class PageNavigator;

class CollapsibleComponentHeader : public QAbstractButton
{
  Q_OBJECT

 public:
  CollapsibleComponentHeader(const std::string& text, int numResults, int numPages, QWidget* parent = nullptr);
  virtual ~CollapsibleComponentHeader() {}
  void setChecked(bool isChecked);
  void setText(const QString& text);
  void setNumResults(int numResults);
  void setNumPages(int numPages);
  void firstPage();

 protected:
  void paintEvent(QPaintEvent* event) override;
  QSize sizeHint() const override;

 private:
  void createLayout(const std::string& text, int numResults, int numPages);
  void setImage(bool isChecked);

  QLabel* m_text;
  QLabel* m_arrowLabel;
  PageNavigator* m_pageNavigator;
  QLabel* m_numLabel;

 signals:
  void getComponentsByPage(int pageNum);

 private slots:
  void on_getComponentsByPage(int pageIdx);
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_COLLAPSIBLECOMPONENTHEADER_HPP
