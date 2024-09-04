/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_DEFAULTCONSTRUCTIONSETSVIEW_HPP
#define OPENSTUDIO_DEFAULTCONSTRUCTIONSETSVIEW_HPP

#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

class QStackedWidget;

namespace openstudio {

class DefaultConstructionSetsView : public ModelSubTabView
{
  Q_OBJECT

 public:
  DefaultConstructionSetsView(const openstudio::model::Model& model, const QString& tabLabel, bool hasSubTabs, QWidget* parent = nullptr);

  virtual ~DefaultConstructionSetsView() {}

 private:
};

}  // namespace openstudio

#endif  // OPENSTUDIO_DEFAULTCONSTRUCTIONSETSVIEW_HPP
