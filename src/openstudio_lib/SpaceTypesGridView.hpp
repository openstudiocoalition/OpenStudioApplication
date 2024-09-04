/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SPACETYPESGRIDVIEW_HPP
#define OPENSTUDIO_SPACETYPESGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

#include <QWidget>

class QComboBox;
class QLabel;

namespace openstudio {

class ModelSubTabView;
class SpaceTypesGridController;

class SpaceTypesGridView : public QWidget
{
  Q_OBJECT

 public:
  SpaceTypesGridView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~SpaceTypesGridView() {}

  std::set<model::ModelObject> selectedObjects() const;

  void enableFilter();

  void disableFilter();

  QComboBox* m_filters = nullptr;

 private:
  REGISTER_LOGGER("openstudio.SpaceTypesGridView");

  bool m_isIP;

  SpaceTypesGridController* m_gridController = nullptr;

  QLabel* m_filterLabel = nullptr;

 signals:

  void toggleUnitsClicked(bool displayIP);

  void dropZoneItemClicked(OSItem* item);

  void selectionCleared();
};

class SpaceTypesGridController : public OSGridController
{

  Q_OBJECT

 public:
  SpaceTypesGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                           const std::vector<model::ModelObject>& modelObjects);

  virtual ~SpaceTypesGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 private:
  SpaceTypesGridView* spaceTypesGridView();

 public slots:

  virtual void onCategorySelected(int index) override;

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;

  void filterChanged(const QString& text);
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SPACETYPESGRIDVIEW_HPP
