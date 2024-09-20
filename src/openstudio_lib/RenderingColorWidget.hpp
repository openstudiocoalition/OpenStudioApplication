/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RENDERINGCOLORWIDGET_HPP
#define OPENSTUDIO_RENDERINGCOLORWIDGET_HPP

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include "../shared_gui_components/FieldMethodTypedefs.hpp"

#include <openstudio/model/RenderingColor.hpp>

#include <QWidget>

class QPushButton;

namespace openstudio {

class RenderingColorWidget2
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit RenderingColorWidget2(QWidget* parent = nullptr);

  virtual ~RenderingColorWidget2() {}

  bool locked() const;

  void setLocked(bool locked);

  virtual void bind(model::ModelObject& modelObject, OptionalModelObjectGetter get, ModelObjectSetter set);

  virtual void unbind();

 private slots:

  void clear();

  void refresh();

  void renderColorButtonClicked();

  void getRenderingColor();

 private:
  void setRenderingColor();

  boost::optional<OptionalModelObjectGetter> m_get;
  boost::optional<ModelObjectSetter> m_set;
  boost::optional<model::ModelObject> m_modelObject;
  QPushButton* m_renderColorButton;
  boost::optional<model::RenderingColor> m_renderingColor;
};

class RenderingColorWidget
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit RenderingColorWidget(QWidget* parent = nullptr);

  virtual ~RenderingColorWidget() {}

  virtual void attach(const openstudio::model::RenderingColor& renderingColor);

  virtual void detach();

 private slots:

  void clear();

  void refresh();

  void renderColorButtonClicked();

 private:
  QWidget* m_renderColorWidget;
  QPushButton* m_renderColorButton;
  boost::optional<openstudio::model::RenderingColor> m_renderingColor;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_RENDERINGCOLORWIDGET_HPP
