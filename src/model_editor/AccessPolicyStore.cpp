/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <iostream>
#include <sstream>

#include <QXmlStreamReader>

#include "AccessPolicyStore.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/FilesystemHelpers.hpp>
#include <openstudio/utilities/idd/IddFileAndFactoryWrapper.hpp>

namespace openstudio {
namespace model {
AccessPolicyStore* AccessPolicyStore::s_instance = nullptr;

AccessPolicy::AccessPolicy() : m_numNormalFields(std::numeric_limits<unsigned>::max()), m_extensibleSize(std::numeric_limits<unsigned>::max()) {}

AccessPolicy::ACCESS_LEVEL AccessPolicy::getAccess(unsigned int index) const {
  if (m_numNormalFields == std::numeric_limits<unsigned>::max()) {
    OS_ASSERT(false);
  }
  if (m_extensibleSize == std::numeric_limits<unsigned>::max()) {
    OS_ASSERT(false);
  }

  if (index < m_numNormalFields) {
    auto i = m_accessMap.find(index);
    if (i != m_accessMap.end()) {
      return (*i).second;
    }
  } else {
    index -= m_numNormalFields;
    index = index % m_extensibleSize;
    auto i = m_extensibleAccessMap.find(index);
    if (i != m_extensibleAccessMap.end()) {
      return (*i).second;
    }
  }
  return FREE;
}

bool AccessPolicy::setAccess(unsigned int index, AccessPolicy::ACCESS_LEVEL accessLevel) {
  if (m_numNormalFields == std::numeric_limits<unsigned>::max()) {
    OS_ASSERT(false);
  }
  if (m_extensibleSize == std::numeric_limits<unsigned>::max()) {
    OS_ASSERT(false);
  }

  if (index < m_numNormalFields) {
    auto i = m_accessMap.find(index);
    if (i != m_accessMap.end()) {
      (*i).second = accessLevel;
      return true;
    }

    m_accessMap[index] = accessLevel;

  } else {
    index -= m_numNormalFields;
    index = index % m_extensibleSize;
    auto i = m_extensibleAccessMap.find(index);
    if (i != m_extensibleAccessMap.end()) {
      (*i).second = accessLevel;
      return true;
    }

    m_extensibleAccessMap[index] = accessLevel;
  }
  return false;
}

AccessPolicyStore::AccessPolicyStore() = default;

AccessPolicyStore::~AccessPolicyStore() {
  clear();
}

AccessPolicyStore& AccessPolicyStore::Instance() {
  if (s_instance == nullptr) {
    s_instance = new AccessPolicyStore();
  }
  return *s_instance;
}

bool AccessPolicyStore::loadFile(const QByteArray& data) {
  QXmlStreamReader reader(data);

  AccessPolicy* curPolicy = nullptr;
  IddObjectType curType("Catchall");
  IddFileAndFactoryWrapper factory;

  while (!reader.atEnd()) {
    reader.readNext();

    if (reader.isStartElement()) {
      const QString qName = reader.name().toString();

      if (qName.compare("policy", Qt::CaseInsensitive) == 0) {
        if (curPolicy != nullptr) {
          LOG(Debug, "parse error, new policy started before old one ended\n");
          return false;
        }

        bool foundType = false;
        for (const auto& attr : reader.attributes()) {
          if (attr.qualifiedName().compare("IddObjectType", Qt::CaseInsensitive) == 0) {
            QString val = attr.value().toString();
            try {
              curType = IddObjectType(val.toStdString());
            } catch (...) {
              LOG(Debug, "IddObjectType failed conversion:" << val.toStdString() << "\n");
              return false;
            }
            auto exists = Instance().m_policyMap.find(curType);
            if (exists != Instance().m_policyMap.end()) {
              LOG(Warn, "2 entries of same type found in policy xml. Later entries will obscure previous entries:" << val.toStdString() << "\n");
              delete exists->second;
              Instance().m_policyMap.erase(exists);
              OS_ASSERT(false);
            }
            curPolicy = new AccessPolicy();

            OptionalIddObject opObj = factory.getObject(curType);
            if (opObj) {
              curPolicy->m_numNormalFields = opObj->numFields();
              curPolicy->m_extensibleSize = opObj->properties().numExtensible;
            }

            Instance().m_policyMap[curType] = curPolicy;
            foundType = true;
            break;
          }
        }
        if (!foundType) {
          LOG(Debug, "NO IddObjectType!!!!\n");
          return false;
        }

      } else if (qName.compare("rule", Qt::CaseInsensitive) == 0) {
        if (curPolicy == nullptr) {
          LOG(Debug, "parse error, rule started before a policy is started");
          return false;
        }

        QString fieldName;
        QString accessRule;
        for (const auto& attr : reader.attributes()) {
          if (attr.qualifiedName().compare("IddField", Qt::CaseInsensitive) == 0) {
            fieldName = attr.value().toString();
          } else if (attr.qualifiedName().compare("access", Qt::CaseInsensitive) == 0) {
            accessRule = attr.value().toString();
          }
        }

        if (!fieldName.isEmpty() && !accessRule.isEmpty()) {
          AccessPolicy::ACCESS_LEVEL level = AccessPolicy::FREE;
          if (accessRule.compare("locked", Qt::CaseInsensitive) == 0) {
            level = AccessPolicy::LOCKED;
          } else if (accessRule.compare("hidden", Qt::CaseInsensitive) == 0) {
            level = AccessPolicy::HIDDEN;
          }

          OptionalIddObject opObj = factory.getObject(curType);
          if (!opObj) {
            LOG(Debug, "IddObject not found in factory!!!\n");
            continue;  // keep going
          }
          IddObject obj = *opObj;

          [[maybe_unused]] bool foundInFields = false;
          for (unsigned int i = 0, iend = obj.numFields(); i < iend; ++i) {
            openstudio::OptionalIddField f = obj.getField(i);
            QString fieldName2(f->name().c_str());
            if (fieldName.compare(fieldName2, Qt::CaseInsensitive) == 0) {
              curPolicy->m_accessMap[i] = level;
              foundInFields = true;
              break;
            }
          }
          curPolicy->m_numNormalFields = obj.numFields();
          curPolicy->m_extensibleSize = obj.properties().numExtensible;
          for (unsigned int i = obj.numFields(), iend = obj.properties().numExtensible + obj.numFields(); i < iend && !foundInFields; ++i) {
            openstudio::OptionalIddField f = obj.getField(i);
            QString fieldName2(f->name().c_str());
            if (fieldName.compare(fieldName2, Qt::CaseInsensitive) == 0) {
              curPolicy->m_extensibleAccessMap[i - obj.numFields()] = level;
              foundInFields = true;
              break;
            }
          }
        } else {
          LOG(Debug, "Parse error in <rule> need both IddField and Access attribute\n");
        }
      }

    } else if (reader.isEndElement()) {
      if (reader.name().compare("policy", Qt::CaseInsensitive) == 0) {
        curPolicy = nullptr;
      }
    }
  }

  if (reader.hasError()) {
    LOG(Debug, "xml parse error in AccessPolicyStore::loadFile: " << reader.errorString().toStdString() << "\n");
    OS_ASSERT(false);
    return false;
  }

  return true;
}

bool AccessPolicyStore::loadFile(openstudio::filesystem::ifstream& file) {
  if (!file.is_open()) {
    LOG(Debug, "file was not found\n");
    return false;
  }

  return loadFile(openstudio::filesystem::read(file));
}

bool AccessPolicyStore::loadFile(const openstudio::path& path) {
  openstudio::filesystem::ifstream file(path);
  return loadFile(file);
}

const AccessPolicy* AccessPolicyStore::getPolicy(const openstudio::IddObjectType& type) const {
  auto it = m_policyMap.find(type);
  if (it != m_policyMap.end()) {
    return it->second;
  }

  return nullptr;
}

void AccessPolicyStore::clear() {
  for (const auto& policyPair : m_policyMap) {
    // Not need to test if ptr has a value, delete nullptr has no effect
    delete policyPair.second;
  }
  m_policyMap.clear();
}

}  // namespace model
}  // namespace openstudio
