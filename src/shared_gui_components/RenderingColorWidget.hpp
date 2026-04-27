/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_RENDERINGCOLORWIDGET_HPP
#define OPENSTUDIO_RENDERINGCOLORWIDGET_HPP

#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <openstudio/model/RenderingColor.hpp>

#include <QWidget>

class QPushButton;

namespace openstudio {

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
