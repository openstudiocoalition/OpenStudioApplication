/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "TIDItemModel.hpp"
#include <openstudio/utilities/core/Compare.hpp>
#include <openstudio/utilities/idd/IddEnums.hpp>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

class TIDItem
{
 public:
  explicit TIDItem(const QDomElement& element, TIDItem* parent = nullptr) : m_parentItem(parent) {

    QString tagName = element.tagName();

    if (istringEqual(tagName.toStdString(), "term")) {
      QString tidName = element.firstChildElement("name").text();

      m_itemData.push_back(tidName);

      int tid = element.firstChildElement("tid").text().toInt();

      m_itemData.push_back(tid);
    } else if (istringEqual(tagName.toStdString(), "taxonomy")) {
      m_itemData.push_back("Taxonomy");

      m_itemData.push_back(0);
    }

    QDomNodeList children = element.childNodes();

    for (int i = 0; i < children.length(); i++) {
      QDomNode childNode = children.at(i);

      if (childNode.isElement()) {
        QDomElement childElement = childNode.toElement();

        if (istringEqual(childElement.tagName().toStdString(), "term")) {
          auto* childItem = new TIDItem(childElement, this);

          m_childItems.append(childItem);
        }
      }
    }
  }

  ~TIDItem() {
    qDeleteAll(m_childItems);
  }

  TIDItem* child(int row) {
    return m_childItems.value(row);
  }

  int childCount() const {
    return m_childItems.count();
  }

  int columnCount() const {
    return m_itemData.count();
  }

  QVariant data(int column) const {
    return m_itemData.value(column);
  }

  int row() const {
    if (m_parentItem) {
      return m_parentItem->m_childItems.indexOf(const_cast<TIDItem*>(this));
    }

    return 0;
  }

  TIDItem* parent() {
    return m_parentItem;
  }

 private:
  QList<TIDItem*> m_childItems;

  QList<QVariant> m_itemData;

  TIDItem* m_parentItem;
};

TIDItemModel::TIDItemModel(const QDomDocument& document, QObject* parent) : QAbstractItemModel(parent), m_document(document) {
  m_rootItem = new TIDItem(m_document.documentElement());
}

TIDItemModel::~TIDItemModel() {
  delete m_rootItem;
}

QVariant TIDItemModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return {};
  }

  if (role != Qt::DisplayRole) {
    return {};
  }

  auto* item = static_cast<TIDItem*>(index.internalPointer());

  return item->data(index.column());
}

Qt::ItemFlags TIDItemModel::flags(const QModelIndex& index) const {
  if (!index.isValid()) {
    return {};
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex TIDItemModel::index(int row, int column, const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) {
    return {};
  }

  TIDItem* parentItem = nullptr;

  if (!parent.isValid()) {
    parentItem = m_rootItem;
  } else {
    parentItem = static_cast<TIDItem*>(parent.internalPointer());
  }

  TIDItem* childItem = parentItem->child(row);

  if (childItem) {
    return createIndex(row, column, childItem);
  } else {
    return {};
  }
}

QModelIndex TIDItemModel::parent(const QModelIndex& index) const {
  if (!index.isValid()) {
    return {};
  }

  auto* childItem = static_cast<TIDItem*>(index.internalPointer());

  TIDItem* parentItem = childItem->parent();

  if (parentItem == m_rootItem) {
    return {};
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int TIDItemModel::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0) {
    return 0;
  }

  TIDItem* parentItem = nullptr;

  if (!parent.isValid()) {
    parentItem = m_rootItem;
  } else {
    parentItem = static_cast<TIDItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

int TIDItemModel::columnCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return static_cast<TIDItem*>(parent.internalPointer())->columnCount();
  } else {
    return m_rootItem->columnCount();
  }
}

QModelIndex TIDItemModel::indexForTID(int tid) const {
  QModelIndexList list = match(index(0, 1), Qt::DisplayRole, tid);

  if (!list.empty()) {
    QModelIndex idx = list.first();

    return idx.sibling(idx.row(), 0);
  }

  return {};
}

const std::map<int, std::vector<IddObjectType>> TIDItemModel::createTidToOSTypeMap() {
  std::map<int, std::vector<IddObjectType>> map;

  // 127 Construction Assembly
  std::vector<IddObjectType> tid127_types;
  tid127_types.emplace_back(IddObjectType::OS_Construction);
  map[127] = tid127_types;

  // KSB: Should we have lower tids in the map like these?
  // If not then programmer will need to walk up the tid tree.

  // 152 Floor
  std::vector<IddObjectType> tid152_types;
  tid152_types.emplace_back(IddObjectType::OS_Construction);
  map[152] = tid152_types;

  // 134 Roof
  std::vector<IddObjectType> tid134_types;
  tid134_types.emplace_back(IddObjectType::OS_Construction);
  map[134] = tid134_types;

  // 129 Wall
  std::vector<IddObjectType> tid129_types;
  tid129_types.emplace_back(IddObjectType::OS_Construction);
  map[129] = tid129_types;

  return map;
}

const std::map<int, std::vector<IddObjectType>> TIDItemModel::tidToOSTypeMap = TIDItemModel::createTidToOSTypeMap();

}  // namespace openstudio
