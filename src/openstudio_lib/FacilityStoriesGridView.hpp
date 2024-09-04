/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_FACILITYSTORIESGRIDVIEW_HPP
#define OPENSTUDIO_FACILITYSTORIESGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "GridViewSubTab.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

class QLineEdit;

namespace openstudio {

class FacilityStoriesGridController;

class FacilityStoriesGridView : public GridViewSubTab
{
  Q_OBJECT

 public:
  FacilityStoriesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = 0);

  virtual ~FacilityStoriesGridView() = default;

 private:
  REGISTER_LOGGER("openstudio.FacilityStoriesGridView");

  virtual void addObject(const IddObjectType& iddObjectType) override;

  virtual void purgeObjects(const IddObjectType& iddObjectType) override;

  void filterChanged();

  QLineEdit* m_lessThanFilter = nullptr;

  QLineEdit* m_greaterThanFilter = nullptr;

  void clearSelection();

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;

 private slots:

  void greaterThanFilterChanged();

  void lessThanFilterChanged();
};

class FacilityStoriesGridController : public OSGridController
{

  Q_OBJECT

 public:
  FacilityStoriesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                                const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~FacilityStoriesGridController() = default;

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

#endif  // OPENSTUDIO_FACILITYSTORIESGRIDVIEW_HPP
