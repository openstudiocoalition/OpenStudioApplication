/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONSCENE_HPP
#define OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONSCENE_HPP

#include <QGraphicsScene>

namespace openstudio {

class ELCDScene : public QGraphicsScene
{
  Q_OBJECT

 public:
  ELCDScene();

  virtual ~ELCDScene() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ELECTRICLOADCENTERDISTRIBUTIONSCENE_HPP
