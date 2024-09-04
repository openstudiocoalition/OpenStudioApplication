/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONSTABVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONSTABVIEW_HPP

#include "SubTabView.hpp"

namespace openstudio {

class ConstructionsTabView : public MainTabView
{
  Q_OBJECT

 public:
  ConstructionsTabView(const model::Model& model, const QString& tabLabel, QWidget* parent = nullptr);
  virtual ~ConstructionsTabView() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONSTABVIEW_HPP
