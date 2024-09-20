/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_OSCATEGORYPLACEHOLDER_HPP
#define OPENSTUDIO_OSCATEGORYPLACEHOLDER_HPP

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;

namespace openstudio {

class OSCategoryPlaceholder : public QWidget
{
  Q_OBJECT

 public:
  explicit OSCategoryPlaceholder(const std::string& text, QWidget* parent = nullptr);

  virtual ~OSCategoryPlaceholder() = default;

  QString text() const;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QLabel* m_textLabel;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_OSCATEGORYPLACEHOLDER_HPP
