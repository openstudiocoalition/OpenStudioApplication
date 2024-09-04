/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESSUBSURFACESGRIDVIEW_HPP
#define OPENSTUDIO_SPACESSUBSURFACESGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "SpacesSubtabGridView.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class SpacesSubsurfacesGridController;

class SpacesSubsurfacesGridView : public SpacesSubtabGridView
{
  Q_OBJECT

 public:
  SpacesSubsurfacesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SpacesSubsurfacesGridView() = default;

 protected:
  virtual void removeObject(openstudio::model::ModelObject modelObject) override;

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;

 private:
  void clearSelection();

  REGISTER_LOGGER("openstudio.SpacesSubsurfacesGridView");
};

class SpacesSubsurfacesGridController : public OSGridController
{

  Q_OBJECT

 public:
  SpacesSubsurfacesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                                  const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~SpacesSubsurfacesGridController() = default;

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

#endif  // OPENSTUDIO_SPACESSUBSURFACESGRIDVIEW_HPP
