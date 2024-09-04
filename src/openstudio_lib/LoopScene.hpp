/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_LOOPSCENE_HPP
#define OPENSTUDIO_LOOPSCENE_HPP

#include <QPainter>
#include <QRectF>
#include <boost/smart_ptr.hpp>
#include <openstudio/model/Loop.hpp>
#include "OSItem.hpp"
#include "GridScene.hpp"
#include "../model_editor/QMetaTypes.hpp"

namespace openstudio {

namespace model {
class Splitter;

class Mixer;

class Node;

class HVACComponent;

class ModelObject;
}  // namespace model

class HorizontalBranchItem;

class VerticalBranchItem;

class HorizontalBranchGroupItem;

class DemandSideItem;

class SupplySideItem;

class OASystemItem;

class LoopScene : public GridScene
{

  Q_OBJECT

 public:
  explicit LoopScene(model::Loop loop, QObject* parent = nullptr);

  virtual ~LoopScene() {}

  model::Loop loop();

 public slots:

  void addedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                            const openstudio::UUID& uuid);

  void removedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                              const openstudio::UUID& uuid);

  void layout();

  //signals:
  //
  //void modelObjectSelected( model::OptionalModelObject &, bool readOnly );

  //void removeModelObjectClicked( model::ModelObject & );

  //void hvacComponentDropped(OSItemId itemid, model::HVACComponent & node);

  //void innerNodeClicked( model::ModelObject & );

 private:
  DemandSideItem* createDemandSide();

  SupplySideItem* createSupplySide();

  void initDefault();

  model::Loop m_loop;

  bool m_dirty;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_LOOPSCENE_HPP
