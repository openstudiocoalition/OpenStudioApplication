/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_REFRIGERATIONSCENE_HPP
#define OPENSTUDIO_REFRIGERATIONSCENE_HPP

#include <QGraphicsScene>

namespace openstudio {

class RefrigerationScene : public QGraphicsScene
{
  Q_OBJECT

 public:
  RefrigerationScene();

  virtual ~RefrigerationScene() {}

 public slots:

  void layout();
};

}  // namespace openstudio

#endif  // OPENSTUDIO_REFRIGERATIONSCENE_HPP
