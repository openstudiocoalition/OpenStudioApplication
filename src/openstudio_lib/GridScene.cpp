/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "GridScene.hpp"

namespace openstudio {

GridScene::GridScene(QObject* parent) : QGraphicsScene(parent) {}

QRectF GridScene::getCell(int xindex, int yindex) {
  int xcord;
  int ycord;
  int width;
  int height;

  xcord = xindex * 100;
  ycord = yindex * 100;
  width = 100;
  height = 100;

  return QRectF(xcord, ycord, width, height);
}

QRectF GridScene::getCell(QPointF point) {
  float xcord;
  float ycord;
  int width;
  int height;

  xcord = ((int)(point.x()) / 100) * 100.0;
  ycord = ((int)(point.y()) / 100) * 100.0;

  width = 100;
  height = 100;

  return QRectF(xcord, ycord, width, height);
}

}  // namespace openstudio
