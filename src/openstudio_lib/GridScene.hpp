/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GRIDSCENE_HPP
#define OPENSTUDIO_GRIDSCENE_HPP

#include <QGraphicsScene>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement
#include <openstudio/model/ModelObject.hpp>
#include "OSItem.hpp"

namespace openstudio {

namespace model {
class HVACComponent;
}

class GridScene
  : public QGraphicsScene
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit GridScene(QObject* parent = nullptr);

  virtual ~GridScene() {}

 signals:

  void modelObjectSelected(model::OptionalModelObject&, bool readOnly);

  void removeModelObjectClicked(model::ModelObject&);

  void hvacComponentDropped(OSItemId itemid, model::HVACComponent& node);

  void hvacComponentDropped(OSItemId itemid);

  void innerNodeClicked(model::ModelObject&);

 protected:
  QRectF getCell(int xindex, int yindex);

  QRectF getCell(QPointF point);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GRIDSCENE_HPP
