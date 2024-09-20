/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_EMSINSPECTORVIEW_HPP
#define OPENSTUDIO_EMSINSPECTORVIEW_HPP

#include <QWidget>

class QVBoxLayout;
class QScrollArea;

namespace openstudio {

namespace model {

class ModelObject;

}  // namespace model

class EMSInspectorView : public QWidget
{
  Q_OBJECT;

 public:
  enum class Type
  {
    SENSOR,
    ACTUATOR
  };

  explicit EMSInspectorView(QWidget* parent = nullptr, EMSInspectorView::Type type = Type::ACTUATOR);
  virtual ~EMSInspectorView() {}

  void layoutModelObject(const model::ModelObject& modelObject);

 private:
  QVBoxLayout* m_layout;
  Type m_type;
  QScrollArea* m_scrollArea;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_EMSINSPECTORVIEW_HPP
