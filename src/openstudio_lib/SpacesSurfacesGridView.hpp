/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESSURFACESGRIDVIEW_HPP
#define OPENSTUDIO_SPACESSURFACESGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "SpacesSubtabGridView.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class SpacesSurfacesGridController;

class SpacesSurfacesGridView : public SpacesSubtabGridView
{
  Q_OBJECT

 public:
  SpacesSurfacesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SpacesSurfacesGridView() = default;

 private:
  REGISTER_LOGGER("openstudio.SpacesSurfacesGridView");
  void clearSelection();

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;
};

class SpacesSurfacesGridController : public OSGridController
{

  Q_OBJECT

 public:
  SpacesSurfacesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                               const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~SpacesSurfacesGridController() = default;

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

#endif  // OPENSTUDIO_SPACESSURFACESGRIDVIEW_HPP
