/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_GEOMETRYTABVIEW_HPP
#define OPENSTUDIO_GEOMETRYTABVIEW_HPP

#include "SubTabView.hpp"

namespace openstudio {

class GeometryTabView : public MainTabView
{
  Q_OBJECT

 public:
  GeometryTabView(const model::Model& model, const QString& tabLabel, QWidget* parent = nullptr);
  virtual ~GeometryTabView() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_GEOMETRYTABVIEW_HPP
