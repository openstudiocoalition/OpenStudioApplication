/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_PLANARSURFACEWIDGET_HPP
#define OPENSTUDIO_PLANARSURFACEWIDGET_HPP

#include <openstudio/model/PlanarSurface.hpp>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <QWidget>

namespace openstudio {

class PlanarSurfaceWidget
  : public QWidget
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit PlanarSurfaceWidget(bool isIP, QWidget* parent = nullptr);

  virtual ~PlanarSurfaceWidget() {}

  virtual void attach(const openstudio::model::PlanarSurface& planarSurface);

  virtual void detach();

 private slots:

  void clear();

  void refresh();

 private:
  void setUnits(bool displayIP);

  boost::optional<openstudio::model::PlanarSurface> m_planarSurface;

  QString m_units;

 public slots:

  void toggleUnits(bool displayIP);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_PLANARSURFACEWIDGET_HPP
