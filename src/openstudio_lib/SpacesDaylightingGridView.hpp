/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESDAYLIGHTINGGRIDVIEW_HPP
#define OPENSTUDIO_SPACESDAYLIGHTINGGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "SpacesSubtabGridView.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class SpacesDaylightingGridController;

class SpacesDaylightingGridView : public SpacesSubtabGridView
{
  Q_OBJECT

 public:
  SpacesDaylightingGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SpacesDaylightingGridView() {}

 private:
  REGISTER_LOGGER("openstudio.SpacesDaylightingGridView");
};

class SpacesDaylightingGridController : public OSGridController
{

  Q_OBJECT

 public:
  SpacesDaylightingGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                                  const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~SpacesDaylightingGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onCategorySelected(int index) override;

  virtual void onItemDropped(const OSItemId& itemId) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACESDAYLIGHTINGGRIDVIEW_HPP
