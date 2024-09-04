/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACETYPESVIEW_HPP
#define OPENSTUDIO_SPACETYPESVIEW_HPP

#include "ModelSubTabView.hpp"
#include "ModelObjectInspectorView.hpp"

#include <openstudio/model/Model.hpp>

#include <openstudio/utilities/idf/WorkspaceObject_Impl.hpp>

class QLabel;

namespace openstudio {

class OSItem;

class SpaceTypesView : public ModelSubTabView
{
  Q_OBJECT

 public:
  SpaceTypesView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~SpaceTypesView() {}
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACETYPESVIEW_HPP
