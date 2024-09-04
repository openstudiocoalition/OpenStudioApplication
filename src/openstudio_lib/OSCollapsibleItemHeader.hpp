/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSCOLLAPSIBLEITEMHEADER_HPP
#define OPENSTUDIO_OSCOLLAPSIBLEITEMHEADER_HPP

#include "OSItem.hpp"

class QLabel;
class QVBoxLayout;
class QPaintEvent;
class QMouseEvent;

namespace openstudio {

class OSCollapsibleItemHeader : public QWidget
{
  Q_OBJECT

 public:
  OSCollapsibleItemHeader(const std::string& text, const OSItemId& itemId, OSItemType type = OSItemType::CollapsibleListHeader,
                          QWidget* parent = nullptr);

  virtual ~OSCollapsibleItemHeader() = default;

  QString text() const;

  bool expanded() const;
  void setExpanded(bool isExpanded);

  bool selected() const;
  void setSelected(bool isSelected);

 signals:

  void clicked(OSCollapsibleItemHeader* header);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;
  QSize sizeHint() const override;

 private:
  void setImage(bool expanded);

  QLabel* m_arrowLabel;
  QLabel* m_textLabel;

  bool m_expanded;
  bool m_selected;
  bool m_mouseDown;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSCOLLAPSIBLEITEMHEADER_HPP
