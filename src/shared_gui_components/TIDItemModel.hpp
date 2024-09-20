/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_TIDITEMMODEL_HPP
#define SHAREDGUICOMPONENTS_TIDITEMMODEL_HPP

#include <QAbstractItemModel>
#include <QDomDocument>
#include <map>

namespace openstudio {
struct IddObjectType;
class TIDItem;

class TIDItemModel : public QAbstractItemModel
{
  Q_OBJECT

 public:
  explicit TIDItemModel(const QDomDocument& document, QObject* parent = nullptr);

  virtual ~TIDItemModel();

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

  QModelIndex parent(const QModelIndex& index) const override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QModelIndex indexForTID(int tid) const;

  static const std::map<int, std::vector<IddObjectType>> tidToOSTypeMap;

 private:
  QDomDocument m_document;

  TIDItem* m_rootItem;

  static const std::map<int, std::vector<IddObjectType>> createTidToOSTypeMap();
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_TIDITEMMODEL_HPP
