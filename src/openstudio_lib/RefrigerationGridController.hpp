/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_REFRIGERATIONGRIDCONTROLLER_HPP
#define OPENSTUDIO_REFRIGERATIONGRIDCONTROLLER_HPP

#include <openstudio/model/Model.hpp>

#include <QObject>
#include <QPointer>
#include <QSharedPointer>

namespace openstudio {

class RefrigerationGridView;

class RefrigerationGridController : public QObject
{
  Q_OBJECT

 public:
  RefrigerationGridController(bool isIP, const model::Model& model);

  virtual ~RefrigerationGridController();

  RefrigerationGridView* refrigerationGridView() const;

 private:
  QPointer<RefrigerationGridView> m_refrigerationGridView;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_REFRIGERATIONGRIDCONTROLLER_HPP
