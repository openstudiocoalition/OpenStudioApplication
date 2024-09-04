/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_REFRIGERATIONGRIDVIEW_HPP
#define OPENSTUDIO_REFRIGERATIONGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

#include <QWidget>

namespace openstudio {

class RefrigerationGridView : public QWidget
{
  Q_OBJECT

 public:
  RefrigerationGridView(bool isIP, const model::Model& model, QWidget* parent = nullptr);

  virtual ~RefrigerationGridView() {}

 private:
  bool m_isIP;

 signals:

  void toggleUnitsClicked(bool displayIP);

  void dropZoneItemClicked(OSItem* item);
};

class RefrigerationCaseGridController : public OSGridController
{

  Q_OBJECT

 public:
  RefrigerationCaseGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                                  const std::vector<model::ModelObject>& modelObjects);

  virtual ~RefrigerationCaseGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& t_category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;
};

class RefrigerationWalkInGridController : public OSGridController
{

  Q_OBJECT

 public:
  RefrigerationWalkInGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                                    const std::vector<model::ModelObject>& modelObjects);

  virtual ~RefrigerationWalkInGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& t_category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_REFRIGERATIONGRIDVIEW_HPP
