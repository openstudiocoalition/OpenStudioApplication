/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELOBJECTTREEWIDGET_HPP
#define OPENSTUDIO_MODELOBJECTTREEWIDGET_HPP

#include "OSItemSelector.hpp"
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <openstudio/model/Model.hpp>
#include "../model_editor/QMetaTypes.hpp"

class QTreeWidget;

class QVBoxLayout;

namespace openstudio {

class ModelObjectTreeWidget
  : public OSItemSelector
  , public Nano::Observer
{
  Q_OBJECT

 public:
  explicit ModelObjectTreeWidget(const model::Model& model, QWidget* parent = nullptr);

  virtual ~ModelObjectTreeWidget() {}

  virtual OSItem* selectedItem() const override;

  QTreeWidget* treeWidget() const;

  QVBoxLayout* vLayout() const;

  openstudio::model::Model model() const;

 protected:
  virtual void onObjectAdded(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType,
                             const openstudio::UUID& handle) = 0;

  virtual void onObjectRemoved(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType,
                               const openstudio::UUID& handle) = 0;

  void refresh();

 private slots:

  void objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& iddObjectType,
                   const openstudio::UUID& handle);

  void objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& iddObjectType,
                     const openstudio::UUID& handle);

 private:
  QTreeWidget* m_treeWidget;

  QVBoxLayout* m_vLayout;

  openstudio::model::Model m_model;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELOBJECTTREEWIDGET_HPP
