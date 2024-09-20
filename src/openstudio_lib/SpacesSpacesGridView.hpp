/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACESSPACESGRIDVIEW_HPP
#define OPENSTUDIO_SPACESSPACESGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "SpacesSubtabGridView.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

namespace openstudio {

class SpacesSpacesGridController;

class SpacesSpacesGridView : public SpacesSubtabGridView
{
  Q_OBJECT

 public:
  SpacesSpacesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = 0);

  virtual ~SpacesSpacesGridView() = default;

 private:
  REGISTER_LOGGER("openstudio.SpacesSpacesGridView");

  void clearSelection();

  // Overrides SpacesSubtabGrid view to trigger appropriate filtering
  virtual bool hasSubRows() override {
    return false;
  };

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;
};

class SpacesSpacesGridController : public OSGridController
{

  Q_OBJECT

 public:
  SpacesSpacesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                             const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~SpacesSpacesGridController() = default;

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

#endif  // OPENSTUDIO_SPACESSPACESGRIDVIEW_HPP
