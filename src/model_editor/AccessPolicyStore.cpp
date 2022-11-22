/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2022, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#include <iostream>
#include <sstream>

#include <QXmlDefaultHandler>
#include <QXmlReader>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QXmlAttributes>

#include "AccessPolicyStore.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/FilesystemHelpers.hpp>
#include <openstudio/utilities/idd/IddFileAndFactoryWrapper.hpp>

// TODO: We will have to replace QXmlDefaultHandler at some point, ignore for now
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace openstudio {
namespace model {
AccessPolicyStore* AccessPolicyStore::s_instance = nullptr;

//XML Parser
class AccessParser : public QXmlDefaultHandler
{
 public:
  AccessParser();

 protected:
  virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts) override;

  virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName) override;

  virtual bool error(const QXmlParseException& e) override;
  virtual bool fatalError(const QXmlParseException& e) override;

 private:
  AccessPolicy* m_curPolicy = nullptr;
  IddObjectType m_curType;
  IddFileAndFactoryWrapper m_factory;
  REGISTER_LOGGER("AccessParser");
};

AccessParser::AccessParser() : m_curType("Catchall") {}

bool AccessParser::error(const QXmlParseException& e) {
  std::stringstream s;
  s << "Error line:" << e.lineNumber() << "\ncolumn:" << e.columnNumber() << "\n" << e.message().toStdString() << "\n";
  LOG(Debug, s.str());
  return false;
}
bool AccessParser::fatalError(const QXmlParseException& e) {
  LOG(Debug, "Error line:" << e.lineNumber() << "\ncolumn:" << e.columnNumber() << "\n" << e.message().toStdString() << "\n");
  return false;
}

bool AccessParser::startElement(const QString& /*namespaceURI*/, const QString& /*localName*/, const QString& qName, const QXmlAttributes& atts) {
  if (qName.compare("policy", Qt::CaseInsensitive) == 0) {
    if (m_curPolicy != nullptr) {
      LOG(Debug, "parse error, new policy started before old one ended\n");
      return false;
    }

    for (int i = 0, iend = atts.length(); i < iend; ++i) {
      QString name = atts.qName(i);
      if (name.compare("IddObjectType", Qt::CaseInsensitive) == 0) {
        QString val = atts.value(i);
        try {
          m_curType = IddObjectType(val.toStdString());
        } catch (...) {
          LOG(Debug, "IddObjectType failed conversion:" << val.toStdString() << "\n");
          return false;  //Bad IddObjectType
        }
        auto exists = AccessPolicyStore::Instance().m_policyMap.find(m_curType);
        if (exists != AccessPolicyStore::Instance().m_policyMap.end()) {
          LOG(Warn, "2 entries of same type found in policy xml. Later entries will obscure previous entires:" << val.toStdString() << "\n");
          delete exists->second;
          AccessPolicyStore::Instance().m_policyMap.erase(exists);
          // DLM: return false?
          OS_ASSERT(false);
        }
        m_curPolicy = new AccessPolicy();

        OptionalIddObject opObj = m_factory.getObject(m_curType);
        if (opObj) {
          // initialize here in case there are no rules
          m_curPolicy->m_numNormalFields = opObj->numFields();
          m_curPolicy->m_extensibleSize = opObj->properties().numExtensible;
        }

        AccessPolicyStore::Instance().m_policyMap[m_curType] = m_curPolicy;
        return true;  //I don't care about any other attributes!
      }
    }
    return false;  //NO IddObjectType!!!!
  }

  if (qName.compare("rule", Qt::CaseInsensitive) == 0) {
    if (m_curPolicy == nullptr) {
      LOG(Debug, "parse error, rule started before a policy is started");
      return false;
    }
    QString fieldName;
    QString accessRule;
    for (int i = 0, iend = atts.length(); i < iend; ++i) {
      QString name = atts.qName(i);
      if (name.compare("IddField", Qt::CaseInsensitive) == 0) {
        fieldName = atts.value(i);
      } else if (name.compare("access", Qt::CaseInsensitive) == 0) {
        accessRule = atts.value(i);
      }
    }
    if (!fieldName.isEmpty() && !accessRule.isEmpty()) {
      AccessPolicy::ACCESS_LEVEL level = AccessPolicy::FREE;
      if (accessRule.compare("locked", Qt::CaseInsensitive) == 0) {
        level = AccessPolicy::LOCKED;
      } else if (accessRule.compare("hidden", Qt::CaseInsensitive) == 0) {
        level = AccessPolicy::HIDDEN;
      }

      OptionalIddObject opObj = m_factory.getObject(m_curType);
      if (!opObj) {
        LOG(Debug, "IddObject not found in factory!!!\n");
        return true;  //keep going
      }
      IddObject obj = *opObj;

      [[maybe_unused]] bool foundInFields = false;
      for (unsigned int i = 0, iend = obj.numFields(); i < iend; ++i) {
        openstudio::OptionalIddField f = obj.getField(i);
        QString fieldName2(f->name().c_str());
        if (fieldName.compare(fieldName2, Qt::CaseInsensitive) == 0) {
          m_curPolicy->m_accessMap[i] = level;
          foundInFields = true;
          break;
        }
      }
      m_curPolicy->m_numNormalFields = obj.numFields();
      m_curPolicy->m_extensibleSize = obj.properties().numExtensible;
      for (unsigned int i = obj.numFields(), iend = obj.properties().numExtensible + obj.numFields(); i < iend && !foundInFields; ++i) {
        openstudio::OptionalIddField f = obj.getField(i);
        QString fieldName2(f->name().c_str());
        if (fieldName.compare(fieldName2, Qt::CaseInsensitive) == 0) {
          m_curPolicy->m_extensibleAccessMap[i - obj.numFields()] = level;
          foundInFields = true;
          break;
        }
      }

      // TODO: should we return foundInFields here?
      return true;
    } else {
      LOG(Debug, "Parse error in <rule> need both IddField and Access attribute\n");
      return true;
    }
  }
  return true;
}

bool AccessParser::endElement(const QString& /*namespaceURI*/, const QString& /*localName*/, const QString& qName) {
  if (qName.compare("policy", Qt::CaseInsensitive) == 0) {
    m_curPolicy = nullptr;
  }
  return true;
}

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
  QXmlSimpleReader xmlReader;
  AccessParser ap;
  xmlReader.setContentHandler(&ap);

  QXmlInputSource source;
  source.setData(data);
  //LER:: add error handler
  if (!xmlReader.parse(source)) {
    LOG(Debug, "xml parse error in AccessPolicyStore::loadFile\n");
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
  for (auto& policyPair : m_policyMap) {
    // Not need to test if ptr has a value, delete nullptr has no effect
    delete policyPair.second;
  }
  m_policyMap.clear();
}

}  // namespace model
}  // namespace openstudio

#if defined(_MSC_VER)
#  pragma warning(pop)
#elif (defined(__GNUC__))
#  pragma GCC diagnostic pop
#endif
