  /*
  This source is part of the libosmscout library
  Copyright (C) 2009  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/TypeConfig.h>

#include <osmscout/system/Assert.h>

#include <osmscout/ost/Parser.h>
#include <osmscout/ost/Scanner.h>

#include <osmscout/util/File.h>
#include <osmscout/util/String.h>

#include <iostream>
namespace osmscout {

  TagCondition::~TagCondition()
  {
    // no code
  }

  TagNotCondition::TagNotCondition(TagCondition* condition)
  : condition(condition)
  {
    // no code
  }

  bool TagNotCondition::Evaluate(const std::map<TagId,std::string>& tagMap) const
  {
    return !condition->Evaluate(tagMap);
  }

  TagBoolCondition::TagBoolCondition(Type type)
  : type(type)
  {
    // no code
  }

  void TagBoolCondition::AddCondition(TagCondition* condition)
  {
    conditions.push_back(condition);
  }

  bool TagBoolCondition::Evaluate(const std::map<TagId,std::string>& tagMap) const
  {
    switch (type) {
    case boolAnd:
      for (std::list<TagConditionRef>::const_iterator condition=conditions.begin();
           condition!=conditions.end();
           ++condition) {
        if (!(*condition)->Evaluate(tagMap)) {
          return false;
        }
      }

      return true;
    case boolOr:
      for (std::list<TagConditionRef>::const_iterator condition=conditions.begin();
           condition!=conditions.end();
           ++condition) {
        if ((*condition)->Evaluate(tagMap)) {
          return true;
        }
      }

      return false;
    default:
      assert(false);

      return false;
    }
  }

  TagExistsCondition::TagExistsCondition(TagId tag)
  : tag(tag)
  {
    // no code
  }

  bool TagExistsCondition::Evaluate(const std::map<TagId,std::string>& tagMap) const
  {
    return tagMap.find(tag)!=tagMap.end();
  }

  TagBinaryCondition::TagBinaryCondition(TagId tag,
                                         BinaryOperator binaryOperator,
                                         const std::string& tagValue)
  : tag(tag),
    binaryOperator(binaryOperator),
    valueType(string),
    tagStringValue(tagValue),
    tagSizeValue(0)
  {
    // no code
  }

  TagBinaryCondition::TagBinaryCondition(TagId tag,
                                         BinaryOperator binaryOperator,
                                         const size_t& tagValue)
  : tag(tag),
    binaryOperator(binaryOperator),
    valueType(sizet),
    tagSizeValue(tagValue)
  {
    // no code
  }

  bool TagBinaryCondition::Evaluate(const std::map<TagId,std::string>& tagMap) const
  {
    std::map<TagId,std::string>::const_iterator t;

    t=tagMap.find(tag);

    if (t==tagMap.end()) {
      return false;
    }

    if (valueType==string) {
      switch (binaryOperator) {
      case  operatorLess:
        return t->second<tagStringValue;
      case  operatorLessEqual:
        return t->second<=tagStringValue;
      case  operatorEqual:
        return t->second==tagStringValue;
      case operatorNotEqual:
        return t->second!=tagStringValue;
      case operatorGreaterEqual:
        return t->second>=tagStringValue;
      case  operatorGreater:
        return t->second>tagStringValue;
      default:
        assert(false);

        return false;
      }
    }
    else if (valueType==sizet) {
      size_t value;

      if (!StringToNumber(t->second,
                          value)) {
        return false;
      }

      switch (binaryOperator) {
      case  operatorLess:
        return value<tagSizeValue;
      case  operatorLessEqual:
        return value<=tagSizeValue;
      case  operatorEqual:
        return value==tagSizeValue;
      case operatorNotEqual:
        return value!=tagSizeValue;
      case operatorGreaterEqual:
        return value>=tagSizeValue;
      case  operatorGreater:
        return value>tagSizeValue;
      default:
        assert(false);

        return false;
      }
    }
    else {
      assert(false);

      return false;
    }
  }

  TagIsInCondition::TagIsInCondition(TagId tag)
  : tag(tag)
  {
    // no code
  }

  void TagIsInCondition::AddTagValue(const std::string& tagValue)
  {
    tagValues.insert(tagValue);
  }

  bool TagIsInCondition::Evaluate(const std::map<TagId,std::string>& tagMap) const
  {
    std::map<TagId,std::string>::const_iterator t;

    t=tagMap.find(tag);

    if (t==tagMap.end()) {
      return false;
    }

    return tagValues.find(t->second)!=tagValues.end();
  }

  TagInfo::TagInfo()
   : id(0),
     internalOnly(true)
  {
  }

  TagInfo::TagInfo(const std::string& name,
                   bool internalOnly)
   : id(0),
     name(name),
     internalOnly(internalOnly)
  {
    // no code
  }

  TagInfo& TagInfo::SetId(TagId id)
  {
    this->id=id;

    return *this;
  }

  TagInfo& TagInfo::SetToExternal()
  {
    internalOnly=false;

    return *this;
  }

  FeatureValue::FeatureValue()
  {
    // no code
  }

  FeatureValue::~FeatureValue()
  {
    // no code
  }

  Feature::Feature()
  {
    // no code
  }

  Feature::~Feature()
  {
    // no code
  }

  FeatureInstance::FeatureInstance(const FeatureRef& feature,
                                   size_t offset)
  : feature(feature),
    offset(offset)
  {
    assert(feature.Valid());
  }

  const char* const NameFeature::NAME = "Name";

  void NameFeature::Initialize(TypeConfig& /*typeConfig*/)
  {
    // no code
  }

  std::string NameFeature::GetName() const
  {
    return NAME;
  }

  size_t NameFeature::GetValueSize() const
  {
    return sizeof(NameFeatureValue);
  }

  void NameFeature::AllocateValue(void* buffer)
  {
    new (buffer) NameFeatureValue();
  }

  void NameFeature::Parse(Progress& /*progress*/,
                          const TypeConfig& typeConfig,
                          const ObjectOSMRef& /*object*/,
                          const TypeInfo& /*type*/,
                          size_t idx,
                          const std::map<TagId,std::string>& tags,
                          FeatureValueBuffer& buffer) const
  {
    std::string name;
    uint32_t    namePriority=0;

    for (std::map<TagId,std::string>::const_iterator tag=tags.begin();
         tag!=tags.end();
         ++tag) {
      uint32_t ntPrio;
      bool     isNameTag=typeConfig.IsNameTag(tag->first,ntPrio);

      if (isNameTag &&
          (name.empty() || ntPrio>namePriority)) {
        name=tag->second;
        namePriority=ntPrio;
      }
    }

    if (!name.empty()) {
      buffer.AllocateValue(idx);

      NameFeatureValue* value=dynamic_cast<NameFeatureValue*>(buffer.GetValue(idx));

      value->SetName(name);
    }
  }

  bool NameFeature::Read(FileScanner& scanner,
                         FeatureValue* value)
  {
    std::string name;

    if (!scanner.Read(name)) {
      return false;
    }

    NameFeatureValue* nameFeature=static_cast<NameFeatureValue*>(value);

    nameFeature->SetName(name);

    return true;
  }

  bool NameFeature::Write(FileWriter& writer,
                          FeatureValue* value)
  {
    NameFeatureValue* v=dynamic_cast<NameFeatureValue*>(value);

    return writer.Write(v->GetName());
  }

  const char* const NameAltFeature::NAME = "NameAlt";

  void NameAltFeature::Initialize(TypeConfig& /*typeConfig*/)
  {
    // no code
  }

  std::string NameAltFeature::GetName() const
  {
    return NAME;
  }

  size_t NameAltFeature::GetValueSize() const
  {
    return sizeof(NameAltFeatureValue);
  }

  void NameAltFeature::AllocateValue(void* buffer)
  {
    new (buffer) NameAltFeatureValue();
  }

  void NameAltFeature::Parse(Progress& /*progress*/,
                             const TypeConfig& typeConfig,
                             const ObjectOSMRef& /*object*/,
                             const TypeInfo& /*type*/,
                             size_t idx,
                             const std::map<TagId,std::string>& tags,
                             FeatureValueBuffer& buffer) const
  {
    std::string nameAlt;
    uint32_t    nameAltPriority=0;

    for (std::map<TagId,std::string>::const_iterator tag=tags.begin();
         tag!=tags.end();
         ++tag) {
      uint32_t natPrio;
      bool     isNameAltTag=typeConfig.IsNameAltTag(tag->first,natPrio);

      if (isNameAltTag &&
          (nameAlt.empty() || natPrio>nameAltPriority)) {
        nameAlt=tag->second;
        nameAltPriority=natPrio;
      }
    }

    if (!nameAlt.empty()) {
      buffer.AllocateValue(idx);

      NameAltFeatureValue* value=dynamic_cast<NameAltFeatureValue*>(buffer.GetValue(idx));

      value->SetNameAlt(nameAlt);
    }
  }

  bool NameAltFeature::Read(FileScanner& scanner,
                            FeatureValue* value)
  {
    std::string altName;

    if (!scanner.Read(altName)) {
      return false;
    }

    NameAltFeatureValue* nameAltFeature=static_cast<NameAltFeatureValue*>(value);

    nameAltFeature->SetNameAlt(altName);

    return true;
  }

  bool NameAltFeature::Write(FileWriter& writer,
                             FeatureValue* value)
  {
    NameAltFeatureValue* v=dynamic_cast<NameAltFeatureValue*>(value);

    return writer.Write(v->GetNameAlt());
  }

  const char* const RefFeature::NAME = "Ref";

  void RefFeature::Initialize(TypeConfig& typeConfig)
  {
    tagRef=typeConfig.RegisterTagForInternalUse("ref");
  }

  std::string RefFeature::GetName() const
  {
    return NAME;
  }

  size_t RefFeature::GetValueSize() const
  {
    return sizeof(RefFeatureValue);
  }

  void RefFeature::AllocateValue(void* buffer)
  {
    new (buffer) RefFeatureValue();
  }

  void RefFeature::Parse(Progress& /*progress*/,
                         const TypeConfig& /*typeConfig*/,
                         const ObjectOSMRef& /*object*/,
                         const TypeInfo& /*type*/,
                         size_t idx,
                         const std::map<TagId,std::string>& tags,
                         FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator ref=tags.find(tagRef);

    if (ref!=tags.end() &&
        !ref->second.empty()) {
      buffer.AllocateValue(idx);

      RefFeatureValue* value=dynamic_cast<RefFeatureValue*>(buffer.GetValue(idx));

      value->SetRef(ref->second);
    }
  }

  bool RefFeature::Read(FileScanner& scanner,
                        FeatureValue* value)
  {
    std::string ref;

    if (!scanner.Read(ref)) {
      return false;
    }

    RefFeatureValue* refFeature=static_cast<RefFeatureValue*>(value);

    refFeature->SetRef(ref);

    return true;
  }

  bool RefFeature::Write(FileWriter& writer,
                         FeatureValue* value)
  {
    RefFeatureValue* v=dynamic_cast<RefFeatureValue*>(value);

    return writer.Write(v->GetRef());
  }

  const char* const AddressFeature::NAME = "Address";

  void AddressFeature::Initialize(TypeConfig& typeConfig)
  {
    tagAddrHouseNr=typeConfig.RegisterTagForInternalUse("addr:housenumber");
    tagAddrStreet=typeConfig.RegisterTagForInternalUse("addr:street");
  }

  std::string AddressFeature::GetName() const
  {
    return NAME;
  }

  size_t AddressFeature::GetValueSize() const
  {
    return sizeof(AddressFeatureValue);
  }

  void AddressFeature::AllocateValue(void* buffer)
  {
    new (buffer) AddressFeatureValue();
  }

  void AddressFeature::Parse(Progress& /*progress*/,
                             const TypeConfig& /*typeConfig*/,
                             const ObjectOSMRef& /*object*/,
                             const TypeInfo& /*type*/,
                             size_t idx,
                             const std::map<TagId,std::string>& tags,
                             FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator street;
    std::map<TagId,std::string>::const_iterator houseNr;

    street=tags.find(tagAddrStreet);

    if (street!=tags.end()) {
      houseNr=tags.find(tagAddrHouseNr);
    }
    else {
      houseNr=tags.end();
    }

    if (street!=tags.end() &&
        !street->second.empty() &&
        houseNr!=tags.end() &&
        !houseNr->second.empty()) {
      buffer.AllocateValue(idx);

      AddressFeatureValue* value=dynamic_cast<AddressFeatureValue*>(buffer.GetValue(idx));

      value->SetAddress(street->second,
                        houseNr->second);
    }
  }

  bool AddressFeature::Read(FileScanner& scanner,
                            FeatureValue* value)
  {
    std::string location;
    std::string address;

    if (!scanner.Read(location) ||
        !scanner.Read(address)) {
      return false;
    }

    AddressFeatureValue* addressFeature=static_cast<AddressFeatureValue*>(value);

    addressFeature->SetAddress(location,
                               address);

    return true;
  }

  bool AddressFeature::Write(FileWriter& writer,
                             FeatureValue* value)
  {
    AddressFeatureValue* v=dynamic_cast<AddressFeatureValue*>(value);

    return writer.Write(v->GetLocation()) && writer.Write(v->GetAddress());
  }

  const char* const AccessFeature::NAME = "Access";

  void AccessFeature::Initialize(TypeConfig& typeConfig)
  {
    tagOneway=typeConfig.RegisterTagForInternalUse("oneway");
    tagJunction=typeConfig.RegisterTagForInternalUse("junction");

    tagAccess=typeConfig.RegisterTagForInternalUse("access");
    tagAccessForward=typeConfig.RegisterTagForInternalUse("access:foward");
    tagAccessBackward=typeConfig.RegisterTagForInternalUse("access:backward");

    tagAccessFoot=typeConfig.RegisterTagForInternalUse("access:foot");
    tagAccessFootForward=typeConfig.RegisterTagForInternalUse("access:foot:foward");
    tagAccessFootBackward=typeConfig.RegisterTagForInternalUse("access:foot:backward");

    tagAccessBicycle=typeConfig.RegisterTagForInternalUse("access:bicycle");
    tagAccessBicycleForward=typeConfig.RegisterTagForInternalUse("access:bicycle:foward");
    tagAccessBicycleBackward=typeConfig.RegisterTagForInternalUse("access:bicycle:backward");

    tagAccessMotorVehicle=typeConfig.RegisterTagForInternalUse("access:motor_vehicle");
    tagAccessMotorVehicleForward=typeConfig.RegisterTagForInternalUse("access:motor_vehicle:foward");
    tagAccessMotorVehicleBackward=typeConfig.RegisterTagForInternalUse("access:motor_vehicle:backward");

    tagAccessMotorcar=typeConfig.RegisterTagForInternalUse("access:motorcar");
    tagAccessMotorcarForward=typeConfig.RegisterTagForInternalUse("access:motorcar:foward");
    tagAccessMotorcarBackward=typeConfig.RegisterTagForInternalUse("access:motorcar:backward");
  }

  std::string AccessFeature::GetName() const
  {
    return NAME;
  }

  size_t AccessFeature::GetValueSize() const
  {
    return sizeof(AccessFeatureValue);
  }

  void AccessFeature::AllocateValue(void* buffer)
  {
    new (buffer) AccessFeatureValue();
  }

  void AccessFeature::Parse(Progress& /*progress*/,
                            const TypeConfig& /*typeConfig*/,
                            const ObjectOSMRef& /*object*/,
                            const TypeInfo& type,
                            size_t idx,
                            const std::map<TagId,std::string>& tags,
                            FeatureValueBuffer& buffer) const
  {
    uint8_t access=0;

    if (type.CanRouteFoot()) {
      access|=(AccessFeatureValue::footForward|AccessFeatureValue::footBackward);
    }

    if (type.CanRouteBicycle()) {
      access|=(AccessFeatureValue::bicycleForward|AccessFeatureValue::bicycleBackward);
    }

    if (type.CanRouteCar()) {
      access|=(AccessFeatureValue::carForward|AccessFeatureValue::carBackward);
    }

    uint8_t defaultAccess=access;

    // Flag access

    std::map<TagId,std::string>::const_iterator accessValue=tags.find(tagAccess);

    if (accessValue!=tags.end()) {
      access=0;

      if (!(accessValue->second=="no")) {
        access=(AccessFeatureValue::footForward|AccessFeatureValue::footBackward|AccessFeatureValue::bicycleForward|AccessFeatureValue::bicycleBackward|AccessFeatureValue::carForward|AccessFeatureValue::carBackward);
      }
    }

    // Flag access:forward/access:backward

    std::map<TagId,std::string>::const_iterator accessForwardValue=tags.find(tagAccessForward);
    std::map<TagId,std::string>::const_iterator accessBackwardValue=tags.find(tagAccessBackward);

    if (accessForwardValue!=tags.end()) {
      access&=~(AccessFeatureValue::footForward|AccessFeatureValue::bicycleForward|AccessFeatureValue::carForward);

      if (!(accessForwardValue->second=="no")) {
        access|=(AccessFeatureValue::footForward|AccessFeatureValue::bicycleForward|AccessFeatureValue::carForward);
      }
    }
    else if (accessBackwardValue!=tags.end()) {
      access&=~(AccessFeatureValue::footBackward|AccessFeatureValue::bicycleBackward|AccessFeatureValue::carBackward);

      if (!(accessBackwardValue->second=="no")) {
        access|=(AccessFeatureValue::footBackward|AccessFeatureValue::bicycleBackward|AccessFeatureValue::carBackward);
      }
    }

    // Flags access:foot, access:bicycle, access:motor_vehicle, access:motorcar

    std::map<TagId,std::string>::const_iterator accessFootValue=tags.find(tagAccessFoot);
    std::map<TagId,std::string>::const_iterator accessBicycleValue=tags.find(tagAccessBicycle);
    std::map<TagId,std::string>::const_iterator accessMotorVehicleValue=tags.find(tagAccessMotorVehicle);
    std::map<TagId,std::string>::const_iterator accessMotorcarValue=tags.find(tagAccessMotorcar);

    if (accessFootValue!=tags.end()) {
      access&=~(AccessFeatureValue::footForward|AccessFeatureValue::footBackward);
      if (!(accessFootValue->second=="no")) {
        access|=(AccessFeatureValue::footForward|AccessFeatureValue::footBackward);
      }
    }
    else if (accessBicycleValue!=tags.end()) {
      access&=~(AccessFeatureValue::bicycleForward|AccessFeatureValue::bicycleBackward);

      if (!(accessBicycleValue->second=="no")) {
        if (!access & AccessFeatureValue::onewayBackward) {
          access|=(AccessFeatureValue::bicycleForward);
        }
        if (!access & AccessFeatureValue::onewayForward) {
          access|=(AccessFeatureValue::bicycleBackward);
        }
      }
    }
    else if (accessMotorVehicleValue!=tags.end()) {
      access&=~(AccessFeatureValue::carForward|AccessFeatureValue::carBackward);

      if (!(accessMotorVehicleValue->second=="no")) {
        if (!access & AccessFeatureValue::onewayBackward) {
          access|=(AccessFeatureValue::carForward);
        }
        if (!access & AccessFeatureValue::onewayForward) {
          access|=(AccessFeatureValue::carBackward);
        }
      }
    }
    else if (accessMotorcarValue!=tags.end()) {
      access&=~(AccessFeatureValue::carForward|AccessFeatureValue::carBackward);

      if (!(accessMotorcarValue->second=="no")) {
        if (!access & AccessFeatureValue::onewayBackward) {
          access|=(AccessFeatureValue::carForward);
        }
        if (!access & AccessFeatureValue::onewayForward) {
          access|=(AccessFeatureValue::carBackward);
        }
      }
    }

    // Flags access:foot::forward/access:foot::backward,
    //       access:bicycle::forward/access:bicycle::backward,
    //       access:motor_vehicle::forward/access:motor_vehicle::backward,
    //       access:motorcar::forward/access:motorcar::backward

    std::map<TagId,std::string>::const_iterator accessFootForwardValue=tags.find(tagAccessFootForward);
    std::map<TagId,std::string>::const_iterator accessFootBackwardValue=tags.find(tagAccessFootBackward);
    std::map<TagId,std::string>::const_iterator accessBicycleForwardValue=tags.find(tagAccessBicycleForward);
    std::map<TagId,std::string>::const_iterator accessBicycleBackwardValue=tags.find(tagAccessBicycleBackward);
    std::map<TagId,std::string>::const_iterator accessMotorVehicleForwardValue=tags.find(tagAccessMotorVehicleForward);
    std::map<TagId,std::string>::const_iterator accessMotorVehicleBackwardValue=tags.find(tagAccessMotorVehicleBackward);
    std::map<TagId,std::string>::const_iterator accessMotorcarForwardValue=tags.find(tagAccessMotorcarForward);
    std::map<TagId,std::string>::const_iterator accessMotorcarBackwardValue=tags.find(tagAccessMotorcarBackward);

    if (accessFootForwardValue!=tags.end()) {
      ParseAccessFlag(accessFootForwardValue->second,
                      access,
                      AccessFeatureValue::footForward);
    }

    if (accessFootBackwardValue!=tags.end()) {
      ParseAccessFlag(accessFootBackwardValue->second,
                      access,
                      AccessFeatureValue::footBackward);
    }

    if (accessBicycleForwardValue!=tags.end()) {
      ParseAccessFlag(accessBicycleForwardValue->second,
                      access,
                      AccessFeatureValue::bicycleForward);
    }

    if (accessBicycleBackwardValue!=tags.end()) {
      ParseAccessFlag(accessBicycleBackwardValue->second,
                      access,
                      AccessFeatureValue::bicycleBackward);
    }

    if (accessMotorVehicleForwardValue!=tags.end()) {
      ParseAccessFlag(accessMotorVehicleForwardValue->second,
                      access,
                      AccessFeatureValue::carForward);
    }

    if (accessMotorVehicleBackwardValue!=tags.end()) {
      ParseAccessFlag(accessMotorVehicleBackwardValue->second,
                      access,
                      AccessFeatureValue::carBackward);
    }

    if (accessMotorcarForwardValue!=tags.end()) {
      ParseAccessFlag(accessMotorcarForwardValue->second,
                      access,
                      AccessFeatureValue::carForward);
    }

    if (accessMotorcarBackwardValue!=tags.end()) {
      ParseAccessFlag(accessMotorcarBackwardValue->second,
                      access,
                      AccessFeatureValue::carBackward);
    }

    std::map<TagId,std::string>::const_iterator onewayValue=tags.find(tagOneway);
    std::map<TagId,std::string>::const_iterator junctionValue=tags.find(tagJunction);

    if (onewayValue!=tags.end()) {
      if (onewayValue->second=="-1") {
        access&=~(AccessFeatureValue::bicycleForward|AccessFeatureValue::carForward|AccessFeatureValue::onewayForward);
        access|=AccessFeatureValue::onewayBackward;
      }
      else if (!(onewayValue->second=="no" || onewayValue->second=="false" || onewayValue->second=="0")) {
        access&=~(AccessFeatureValue::bicycleBackward|AccessFeatureValue::carBackward|AccessFeatureValue::onewayBackward);
        access|=AccessFeatureValue::onewayForward;
      }
    }
    else if (junctionValue!=tags.end()
             && junctionValue->second=="roundabout") {
      access&=~(AccessFeatureValue::bicycleBackward|AccessFeatureValue::carBackward|AccessFeatureValue::onewayBackward);
      access|=(AccessFeatureValue::bicycleForward|AccessFeatureValue::carForward|AccessFeatureValue::onewayForward);
    }

    if (access!=defaultAccess) {
      buffer.AllocateValue(idx);

      AccessFeatureValue* value=dynamic_cast<AccessFeatureValue*>(buffer.GetValue(idx));

      value->SetAccess(access);
    }
  }

  bool AccessFeature::Read(FileScanner& scanner,
                           FeatureValue* value)
  {
    uint8_t access;

    if (!scanner.Read(access)) {
      return false;
    }

    AccessFeatureValue* accessFeature=static_cast<AccessFeatureValue*>(value);

    accessFeature->SetAccess(access);

    return true;
  }

  bool AccessFeature::Write(FileWriter& writer,
                            FeatureValue* value)
  {
    AccessFeatureValue* v=dynamic_cast<AccessFeatureValue*>(value);

    return writer.Write(v->GetAccess());
  }

  const char* const LayerFeature::NAME = "Layer";

  void LayerFeature::Initialize(TypeConfig& typeConfig)
  {
    tagLayer=typeConfig.RegisterTagForInternalUse("layer");
  }

  std::string LayerFeature::GetName() const
  {
    return NAME;
  }

  size_t LayerFeature::GetValueSize() const
  {
    return sizeof(LayerFeatureValue);
  }

  void LayerFeature::AllocateValue(void* buffer)
  {
    new (buffer) LayerFeatureValue();
  }

  void LayerFeature::Parse(Progress& progress,
                           const TypeConfig& /*typeConfig*/,
                           const ObjectOSMRef& object,
                           const TypeInfo& /*type*/,
                           size_t idx,
                           const std::map<TagId,std::string>& tags,
                           FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator layer=tags.find(tagLayer);

    if (layer!=tags.end()) {
      int8_t layerValue;

      if (StringToNumber(layer->second,layerValue)) {
        if (layerValue!=0) {
          buffer.AllocateValue(idx);

          LayerFeatureValue* value=dynamic_cast<LayerFeatureValue*>(buffer.GetValue(idx));

          value->SetLayer(layerValue);
        }
      }
      else {
        progress.Warning(std::string("Layer tag value '")+layer->second+"' for "+object.GetName()+" is not numeric!");
      }
    }
  }

  bool LayerFeature::Read(FileScanner& scanner,
                          FeatureValue* value)
  {
    int8_t layer;

    if (!scanner.Read(layer)) {
      return false;
    }

    LayerFeatureValue* layerFeature=static_cast<LayerFeatureValue*>(value);

    layerFeature->SetLayer(layer);

    return true;
  }

  bool LayerFeature::Write(FileWriter& writer,
                           FeatureValue* value)
  {
    LayerFeatureValue* v=dynamic_cast<LayerFeatureValue*>(value);

    return writer.Write(v->GetLayer());
  }

  const char* const WidthFeature::NAME = "Width";

  void WidthFeature::Initialize(TypeConfig& typeConfig)
  {
    tagWidth=typeConfig.RegisterTagForInternalUse("width");
  }

  std::string WidthFeature::GetName() const
  {
    return NAME;
  }

  size_t WidthFeature::GetValueSize() const
  {
    return sizeof(WidthFeatureValue);
  }

  void WidthFeature::AllocateValue(void* buffer)
  {
    new (buffer) WidthFeatureValue();
  }

  void WidthFeature::Parse(Progress& progress,
                           const TypeConfig& /*typeConfig*/,
                           const ObjectOSMRef& object,
                           const TypeInfo& /*type*/,
                           size_t idx,
                           const std::map<TagId,std::string>& tags,
                           FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator width=tags.find(tagWidth);

    if (width==tags.end()) {
      return;
    }

    std::string widthString=width->second;
    double      w;
    size_t      pos=0;
    size_t      count=0;

    // We expect that float values use '.' as separator, but many values use ',' instead.
    // Try try fix this if string looks reasonable
    for (size_t i=0; i<widthString.length() && count<=1; i++) {
      if (widthString[i]==',') {
        pos=i;
        count++;
      }
    }

    if (count==1) {
      widthString[pos]='.';
    }

    // Some width tagvalues add an 'm' to hint that the unit is meter, remove it.
    if (widthString.length()>=2) {
      if (widthString[widthString.length()-1]=='m' &&
          ((widthString[widthString.length()-2]>='0' &&
            widthString[widthString.length()-2]<='9') ||
            widthString[widthString.length()-2]<=' ')) {
        widthString.erase(widthString.length()-1);
      }

      // Trim possible trailing spaces
      while (widthString.length()>0 &&
             widthString[widthString.length()-1]==' ') {
        widthString.erase(widthString.length()-1);
      }
    }

    if (!StringToNumber(widthString,w)) {
      progress.Warning(std::string("Width tag value '")+width->second+"' for "+object.GetName()+" is no double!");
    }
    else if (w<0 && w>255.5) {
      progress.Warning(std::string("Width tag value '")+width->second+"' for "+object.GetName()+" value is too small or too big!");
    }
    else {
      buffer.AllocateValue(idx);

      WidthFeatureValue* value=dynamic_cast<WidthFeatureValue*>(buffer.GetValue(idx));

      value->SetWidth((uint8_t)floor(w+0.5));
    }

  }

  bool WidthFeature::Read(FileScanner& scanner,
                         FeatureValue* value)
  {
    uint8_t width;

    if (!scanner.Read(width)) {
      return false;
    }

    WidthFeatureValue* widthFeature=static_cast<WidthFeatureValue*>(value);

    widthFeature->SetWidth(width);

    return true;
  }

  bool WidthFeature::Write(FileWriter& writer,
                           FeatureValue* value)
  {
    WidthFeatureValue* v=dynamic_cast<WidthFeatureValue*>(value);

    return writer.Write(v->GetWidth());
  }

  const char* const MaxSpeedFeature::NAME = "MaxSpeed";

  void MaxSpeedFeature::Initialize(TypeConfig& typeConfig)
  {
    tagMaxSpeed=typeConfig.RegisterTagForInternalUse("maxspeed");
  }

  std::string MaxSpeedFeature::GetName() const
  {
    return NAME;
  }

  size_t MaxSpeedFeature::GetValueSize() const
  {
    return sizeof(MaxSpeedFeatureValue);
  }

  void MaxSpeedFeature::AllocateValue(void* buffer)
  {
    new (buffer) MaxSpeedFeatureValue();
  }

  void MaxSpeedFeature::Parse(Progress& progress,
                              const TypeConfig& /*typeConfig*/,
                              const ObjectOSMRef& object,
                              const TypeInfo& /*type*/,
                              size_t idx,
                              const std::map<TagId,std::string>& tags,
                              FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator maxSpeed=tags.find(tagMaxSpeed);

    if (maxSpeed==tags.end()) {
      return;
    }

    std::string valueString=maxSpeed->second;
    size_t      valueNumeric;
    bool        isMph=false;

    if (valueString=="signals") {
      return;
    }

    if (valueString=="none") {
      return;
    }

    // "walk" should not be used, but we provide an estimation anyway,
    // since it is likely still better than the default
    if (valueString=="walk") {
      buffer.AllocateValue(idx);

      MaxSpeedFeatureValue* value=dynamic_cast<MaxSpeedFeatureValue*>(buffer.GetValue(idx));

      value->SetMaxSpeed(10);

      return;
    }

    size_t pos;

    pos=valueString.rfind("mph");
    if (pos!=std::string::npos) {
      valueString.erase(pos);
      isMph=true;
    }

    while (valueString.length()>0 && valueString[valueString.length()-1]==' ') {
      valueString.erase(valueString.length()-1);
    }

    if (!StringToNumber(valueString,valueNumeric)) {
      progress.Warning(std::string("Max speed tag value '")+maxSpeed->second+"' for "+object.GetName()+" is not numeric!");
      return;
    }

    buffer.AllocateValue(idx);

    MaxSpeedFeatureValue* value=dynamic_cast<MaxSpeedFeatureValue*>(buffer.GetValue(idx));

    if (isMph) {
      if (valueNumeric>std::numeric_limits<uint8_t>::max()/1.609+0.5) {

        value->SetMaxSpeed(std::numeric_limits<uint8_t>::max());
      }
      else {
        value->SetMaxSpeed((uint8_t)(valueNumeric*1.609+0.5));
      }
    }
    else {
      if (valueNumeric>std::numeric_limits<uint8_t>::max()) {
        value->SetMaxSpeed(std::numeric_limits<uint8_t>::max());
      }
      else {
        value->SetMaxSpeed(valueNumeric);
      }
    }
  }

  bool MaxSpeedFeature::Read(FileScanner& scanner,
                             FeatureValue* value)
  {
    uint8_t maxSpeed;

    if (!scanner.Read(maxSpeed)) {
      return false;
    }

    MaxSpeedFeatureValue* maxSpeedFeature=static_cast<MaxSpeedFeatureValue*>(value);

    maxSpeedFeature->SetMaxSpeed(maxSpeed);

    return true;
  }

  bool MaxSpeedFeature::Write(FileWriter& writer,
                              FeatureValue* value)
  {
    MaxSpeedFeatureValue* v=dynamic_cast<MaxSpeedFeatureValue*>(value);

    return writer.Write(v->GetMaxSpeed());
  }

  const char* const GradeFeature::NAME = "Grade";

  void GradeFeature::Initialize(TypeConfig& typeConfig)
  {
    tagSurface=typeConfig.RegisterTagForInternalUse("surface");
    tagTrackType=typeConfig.RegisterTagForInternalUse("tracktype");
  }

  std::string GradeFeature::GetName() const
  {
    return NAME;
  }

  size_t GradeFeature::GetValueSize() const
  {
    return sizeof(GradeFeatureValue);
  }

  void GradeFeature::AllocateValue(void* buffer)
  {
    new (buffer) GradeFeatureValue();
  }

  void GradeFeature::Parse(Progress& progress,
                           const TypeConfig& typeConfig,
                           const ObjectOSMRef& object,
                           const TypeInfo& /*type*/,
                           size_t idx,
                           const std::map<TagId,std::string>& tags,
                           FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator tracktype=tags.find(tagTrackType);

    if (tracktype!=tags.end()) {
      if (tracktype->second=="grade1") {
        buffer.AllocateValue(idx);

        GradeFeatureValue* value=dynamic_cast<GradeFeatureValue*>(buffer.GetValue(idx));

        value->SetGrade(1);

        return;
      }
      else if (tracktype->second=="grade2") {
        buffer.AllocateValue(idx);

        GradeFeatureValue* value=dynamic_cast<GradeFeatureValue*>(buffer.GetValue(idx));

        value->SetGrade(2);

        return;
      }
      else if (tracktype->second=="grade3") {
        buffer.AllocateValue(idx);

        GradeFeatureValue* value=dynamic_cast<GradeFeatureValue*>(buffer.GetValue(idx));

        value->SetGrade(3);

        return;
      }
      else if (tracktype->second=="grade4") {
        buffer.AllocateValue(idx);

        GradeFeatureValue* value=dynamic_cast<GradeFeatureValue*>(buffer.GetValue(idx));

        value->SetGrade(4);

        return;
      }
      else if (tracktype->second=="grade5") {
        buffer.AllocateValue(idx);

        GradeFeatureValue* value=dynamic_cast<GradeFeatureValue*>(buffer.GetValue(idx));

        value->SetGrade(5);

        return;
      }
      else {
        progress.Warning(std::string("Unsupported tracktype value '")+tracktype->second+"' for "+object.GetName());
      }
    }

    std::map<TagId,std::string>::const_iterator surface=tags.find(tagSurface);

    if (surface!=tags.end()) {
      size_t grade;

      if (typeConfig.GetGradeForSurface(surface->second,
                                        grade)) {
        buffer.AllocateValue(idx);

        GradeFeatureValue* value=dynamic_cast<GradeFeatureValue*>(buffer.GetValue(idx));

        value->SetGrade((uint8_t)grade);
      }
      else {
        progress.Warning(std::string("Unknown surface type '")+surface->second+"' for "+object.GetName()+"!");
      }
    }
  }

  bool GradeFeature::Read(FileScanner& scanner,
                          FeatureValue* value)
  {
    uint8_t grade;

    if (!scanner.Read(grade)) {
      return false;
    }

    GradeFeatureValue* gradeFeature=static_cast<GradeFeatureValue*>(value);

    gradeFeature->SetGrade(grade);

    return true;
  }

  bool GradeFeature::Write(FileWriter& writer,
                           FeatureValue* value)
  {
    GradeFeatureValue* v=dynamic_cast<GradeFeatureValue*>(value);

    return writer.Write(v->GetGrade());
  }

  const char* const BridgeFeature::NAME = "Bridge";

  void BridgeFeature::Initialize(TypeConfig& typeConfig)
  {
    tagBridge=typeConfig.RegisterTagForInternalUse("bridge");
  }

  std::string BridgeFeature::GetName() const
  {
    return NAME;
  }

  size_t BridgeFeature::GetValueSize() const
  {
    return 0;
  }

  void BridgeFeature::AllocateValue(void* /*buffer*/)
  {
    assert(true);
  }

  void BridgeFeature::Parse(Progress& /*progress*/,
                            const TypeConfig& /*typeConfig*/,
                            const ObjectOSMRef& /*object*/,
                            const TypeInfo& /*type*/,
                            size_t idx,
                            const std::map<TagId,std::string>& tags,
                            FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator bridge=tags.find(tagBridge);

    if (bridge!=tags.end() &&
        !(bridge->second=="no" ||
          bridge->second=="false" ||
          bridge->second=="0")) {
      buffer.AllocateValue(idx);
    }
  }

  bool BridgeFeature::Read(FileScanner& /*scanner*/,
                           FeatureValue* /*value*/)
  {
    return true;
  }

  bool BridgeFeature::Write(FileWriter& /*writer*/,
                            FeatureValue* /*value*/)
  {
    return true;
  }

  const char* const TunnelFeature::NAME = "Tunnel";

  void TunnelFeature::Initialize(TypeConfig& typeConfig)
  {
    tagTunnel=typeConfig.RegisterTagForInternalUse("tunnel");
  }

  std::string TunnelFeature::GetName() const
  {
    return NAME;
  }

  size_t TunnelFeature::GetValueSize() const
  {
    return 0;
  }

  void TunnelFeature::AllocateValue(void* /*buffer*/)
  {
    assert(true);
  }

  void TunnelFeature::Parse(Progress& /*progress*/,
                            const TypeConfig& /*typeConfig*/,
                            const ObjectOSMRef& /*object*/,
                            const TypeInfo& /*type*/,
                            size_t idx,
                            const std::map<TagId,std::string>& tags,
                            FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator tunnel=tags.find(tagTunnel);

    if (tunnel!=tags.end() &&
        !(tunnel->second=="no" ||
          tunnel->second=="false" ||
          tunnel->second=="0")) {
      buffer.AllocateValue(idx);
    }
  }

  bool TunnelFeature::Read(FileScanner& /*scanner*/,
                           FeatureValue* /*value*/)
  {
    return true;
  }

  bool TunnelFeature::Write(FileWriter& /*writer*/,
                            FeatureValue* /*value*/)
  {
    return true;
  }

  const char* const RoundaboutFeature::NAME = "Roundabout";

  void RoundaboutFeature::Initialize(TypeConfig& typeConfig)
  {
    tagJunction=typeConfig.RegisterTagForInternalUse("junction");
  }

  std::string RoundaboutFeature::GetName() const
  {
    return NAME;
  }

  size_t RoundaboutFeature::GetValueSize() const
  {
    return 0;
  }

  void RoundaboutFeature::AllocateValue(void* /*buffer*/)
  {
    assert(true);
  }

  void RoundaboutFeature::Parse(Progress& /*progress*/,
                                const TypeConfig& /*typeConfig*/,
                                const ObjectOSMRef& /*object*/,
                                const TypeInfo& /*type*/,
                                size_t idx,
                                const std::map<TagId,std::string>& tags,
                                FeatureValueBuffer& buffer) const
  {
    std::map<TagId,std::string>::const_iterator junction=tags.find(tagJunction);

    if (junction!=tags.end() &&
        junction->second=="roundabout") {
      buffer.AllocateValue(idx);
    }
  }

  bool RoundaboutFeature::Read(FileScanner& /*scanner*/,
                               FeatureValue* /*value*/)
  {
    return true;
  }

  bool RoundaboutFeature::Write(FileWriter& /*writer*/,
                                FeatureValue* /*value*/)
  {
    return true;
  }

  FeatureValueBuffer::FeatureValueBuffer()
  : featureBits(NULL),
    featureValueBuffer(NULL)
  {
    // no code
  }

  FeatureValueBuffer::~FeatureValueBuffer()
  {
    DeleteData();
  }

  void FeatureValueBuffer::SetType(const TypeInfoRef& type)
  {
    assert(type.Valid());

    DeleteData();

    this->type=type;

    AllocateData();
  }

  void FeatureValueBuffer::DeleteData()
  {
    if (type.Valid()) {
      if (featureValueBuffer!=NULL) {
        for (size_t i=0; i<type->GetFeatureCount(); i++) {
          if (HasValue(i)) {
            FreeValue(i);
          }
        }

        delete [] featureValueBuffer;
      }

      if (featureBits!=NULL) {
        delete [] featureBits;
      }
    }
  }

  void FeatureValueBuffer::AllocateData()
  {
    featureBits=new uint8_t[type->GetFeatureBytes()];

    for (size_t i=0; i<type->GetFeatureBytes(); i++) {
      featureBits[i]=0;
    }

    featureValueBuffer=static_cast<char*>(::operator new(type->GetFeatureValueBufferSize()));
  }

  bool FeatureValueBuffer::HasValue(size_t idx) const
  {
    assert(type.Valid());
    assert(idx<type->GetFeatureCount());

    return featureBits[idx/8] & (1 << idx%8);
  }

  FeatureValue* FeatureValueBuffer::GetValue(size_t idx) const
  {
    return static_cast<FeatureValue*>(static_cast<void*>(&featureValueBuffer[type->GetFeature(idx).GetOffset()]));
  }

  void FeatureValueBuffer::AllocateValue(size_t idx)
  {
    assert(type.Valid());
    assert(idx<type->GetFeatureCount());
    assert(!HasValue(idx));

    size_t byteIdx=idx/8;

    featureBits[byteIdx]=featureBits[byteIdx] | (1 << idx%8);

    if (type->GetFeature(idx).GetFeature()->HasValue()) {
      FeatureValue* value=GetValue(idx);

      type->GetFeature(idx).GetFeature()->AllocateValue(value);
    }
  }

  void FeatureValueBuffer::FreeValue(size_t idx)
  {
    assert(type.Valid());
    assert(idx<type->GetFeatureCount());
    assert(HasValue(idx));

    size_t byteIdx=idx/8;

    featureBits[byteIdx]=featureBits[byteIdx] & ~(1 << idx%8);

    if (type->GetFeature(idx).GetFeature()->HasValue()) {
      FeatureValue* value=GetValue(idx);

      value->~FeatureValue();
    }
  }

  void FeatureValueBuffer::Parse(Progress& progress,
                                 const TypeConfig& typeConfig,
                                 const ObjectOSMRef& object,
                                 const std::map<TagId,std::string>& tags)
  {
    size_t featureIdx=0;
    for (auto feature : type->GetFeatures()) {
      feature.GetFeature()->Parse(progress,
                                  typeConfig,
                                  object,
                                  *type,
                                  featureIdx,
                                  tags,
                                  *this);

      featureIdx++;
    }
  }

  bool FeatureValueBuffer::Read(FileScanner& scanner)
  {
    for (size_t i=0; i<type->GetFeatureBytes(); i++) {
      if (!scanner.Read(featureBits[i])) {
        return false;
      }
    }

    size_t idx=0;
    for (auto feature : type->GetFeatures()) {
      if (HasValue(idx) &&
          type->GetFeature(idx).GetFeature()->HasValue()) {
        FeatureValue* value=GetValue(idx);

        type->GetFeature(idx).GetFeature()->AllocateValue(value);

        if (!feature.GetFeature()->Read(scanner,
                                        value)) {
          return false;
        }
      }

      idx++;
    }

    return true;
  }

  bool FeatureValueBuffer::Write(FileWriter& writer) const
  {
    for (size_t i=0; i<type->GetFeatureBytes(); i++) {
      if (!writer.Write(featureBits[i])) {
        return false;
      }
    }

    size_t idx=0;
    for (auto feature : type->GetFeatures()) {
      if (HasValue(idx) &&
          type->GetFeature(idx).GetFeature()->HasValue()) {
        if (!feature.GetFeature()->Write(writer,
                                         GetValue(idx))) {
          return false;
        }
      }

      idx++;
    }

    return !writer.HasError();
  }

  TypeInfo::TypeInfo()
   : id(0),
     canBeNode(false),
     canBeWay(false),
     canBeArea(false),
     canBeRelation(false),
     canRouteFoot(false),
     canRouteBicycle(false),
     canRouteCar(false),
     indexAsLocation(false),
     indexAsRegion(false),
     indexAsPOI(false),
     optimizeLowZoom(false),
     multipolygon(false),
     pinWay(false),
     ignoreSeaLand(false),
     ignore(false)
  {
    // no code
  }

  /**
   * We forbid copying of TypeInfo instances
   *
   * @param other
   */
  TypeInfo::TypeInfo(const TypeInfo& /*other*/)
  {
    // no code
  }

  TypeInfo::~TypeInfo()
  {
    // no code
  }

  TypeInfo& TypeInfo::SetId(TypeId id)
  {
    this->id=id;

    return *this;
  }

  TypeInfo& TypeInfo::SetType(const std::string& name)
  {
    this->name=name;

    return *this;
  }

  TypeInfo& TypeInfo::AddCondition(unsigned char types,
                                   TagCondition* condition)
  {
    TypeCondition typeCondition;

    if (types & typeNode) {
      canBeNode=true;
    }

    if (types & typeWay) {
      canBeWay=true;
    }

    if (types & typeArea) {
      canBeArea=true;
    }

    if (types & typeRelation) {
      canBeRelation=true;
    }

    typeCondition.types=types;
    typeCondition.condition=condition;

    conditions.push_back(typeCondition);

    return *this;
  }

  TypeInfo& TypeInfo::AddFeature(const FeatureRef& feature)
  {
    assert(feature.Valid());
    assert(featureSet.find(feature->GetName())==featureSet.end());

    size_t offset=0;
    size_t alignment=std::max(sizeof(size_t),sizeof(void*));

    if (!features.empty()) {
      offset=features.back().GetOffset()+features.back().GetFeature()->GetValueSize();
      if (offset%alignment!=0) {
        offset=(offset/alignment+1)*alignment;
      }
    }

    features.push_back(FeatureInstance(feature,
                                       offset));
    featureSet.insert(feature->GetName());

    return *this;
  }

  bool TypeInfo::HasFeature(const std::string& featureName) const
  {
    return featureSet.find(featureName)!=featureSet.end();
  }

  size_t TypeInfo::GetFeatureValueBufferSize() const
  {
    if (features.empty()) {
      return 0;
    }

    return features.back().GetOffset()+features.back().GetFeature()->GetValueSize();
  }

  TypeConfig::TypeConfig()
   : nextTagId(0),
     nextTypeId(0),
     typeInfoIgnore(new TypeInfo())
  {
    // Make sure, that this is always registered first.
    // It assures that id 0 is always reserved for tagIgnore
    RegisterTagForInternalUse("");

    RegisterTagForExternalUse("name");
    RegisterTagForExternalUse("ref");
    RegisterTagForExternalUse("bridge");
    RegisterTagForExternalUse("tunnel");
    RegisterTagForExternalUse("layer");
    RegisterTagForExternalUse("width");
    RegisterTagForExternalUse("oneway");
    RegisterTagForExternalUse("addr:housenumber");
    RegisterTagForExternalUse("addr:street");
    RegisterTagForExternalUse("junction");
    RegisterTagForExternalUse("maxspeed");
    RegisterTagForExternalUse("surface");
    RegisterTagForExternalUse("tracktype");
    RegisterTagForExternalUse("admin_level");

    RegisterTagForExternalUse("access");
    RegisterTagForExternalUse("access:foward");
    RegisterTagForExternalUse("access:backward");

    RegisterTagForExternalUse("access:foot");
    RegisterTagForExternalUse("access:foot:foward");
    RegisterTagForExternalUse("access:foot:backward");

    RegisterTagForExternalUse("access:bicycle");
    RegisterTagForExternalUse("access:bicycle:foward");
    RegisterTagForExternalUse("access:bicycle:backward");

    RegisterTagForExternalUse("access:motor_vehicle");
    RegisterTagForExternalUse("access:motor_vehicle:foward");
    RegisterTagForExternalUse("access:motor_vehicle:backward");

    RegisterTagForExternalUse("access:motorcar");
    RegisterTagForExternalUse("access:motorcar:foward");
    RegisterTagForExternalUse("access:motorcar:backward");

    RegisterTagForInternalUse("area");
    RegisterTagForInternalUse("natural");
    RegisterTagForInternalUse("type");
    RegisterTagForInternalUse("restriction");

    RegisterFeature(new NameFeature());
    RegisterFeature(new NameAltFeature());
    RegisterFeature(new RefFeature());
    RegisterFeature(new AddressFeature());
    RegisterFeature(new AccessFeature());
    RegisterFeature(new LayerFeature());
    RegisterFeature(new WidthFeature());
    RegisterFeature(new MaxSpeedFeature());
    RegisterFeature(new GradeFeature());
    RegisterFeature(new BridgeFeature());
    RegisterFeature(new TunnelFeature());
    RegisterFeature(new RoundaboutFeature());

    // Make sure, that this is always registered first.
    // It assures that id 0 is always reserved for typeIgnore
    typeInfoIgnore->SetType("");

    AddTypeInfo(typeInfoIgnore);

    TypeInfoRef route(new TypeInfo());

    // Internal type for showing routes
    route->SetType("_route")
          .CanBeWay(true);

    AddTypeInfo(route);

    TypeInfoRef tileLand(new TypeInfo());

    // Internal types for the land/sea/coast tiles building the base layer for map drawing
    tileLand->SetType("_tile_land")
              .CanBeArea(true);

    AddTypeInfo(tileLand);

    TypeInfoRef tileSea(new TypeInfo());

    tileSea->SetType("_tile_sea")
             .CanBeArea(true);

    AddTypeInfo(tileSea);

    TypeInfoRef tileCoast(new TypeInfo());

    tileCoast->SetType("_tile_coast")
               .CanBeArea(true);

    AddTypeInfo(tileCoast);

    TypeInfoRef tileUnknown(new TypeInfo());

    tileUnknown->SetType("_tile_unknown")
                .CanBeArea(true);

    AddTypeInfo(tileUnknown);

    TypeInfoRef tileCoastline(new TypeInfo());

    tileCoastline->SetType("_tile_coastline")
                   .CanBeWay(true);

    AddTypeInfo(tileCoastline);

    typeTileLand=GetTypeId("_tile_land");
    typeTileSea=GetTypeId("_tile_sea");
    typeTileCoast=GetTypeId("_tile_coast");
    typeTileUnknown=GetTypeId("_tile_unknown");
    typeTileCoastline=GetTypeId("_tile_coastline");

    tagRef=GetTagId("ref");
    tagBridge=GetTagId("bridge");
    tagTunnel=GetTagId("tunnel");
    tagLayer=GetTagId("layer");
    tagWidth=GetTagId("width");
    tagOneway=GetTagId("oneway");
    tagHouseNr=GetTagId("addr:housenumber");
    tagJunction=GetTagId("junction");
    tagMaxSpeed=GetTagId("maxspeed");
    tagSurface=GetTagId("surface");
    tagTracktype=GetTagId("tracktype");
    tagAdminLevel=GetTagId("admin_level");

    tagAccess=GetTagId("access");
    tagAccessForward=GetTagId("access:foward");
    tagAccessBackward=GetTagId("access:backward");

    tagAccessFoot=GetTagId("access:foot");
    tagAccessFootForward=GetTagId("access:foot:foward");
    tagAccessFootBackward=GetTagId("access:foot:backward");

    tagAccessBicycle=GetTagId("access:bicycle");
    tagAccessBicycleForward=GetTagId("access:bicycle:foward");
    tagAccessBicycleBackward=GetTagId("access:bicycle:backward");

    tagAccessMotorVehicle=GetTagId("access:motor_vehicle");
    tagAccessMotorVehicleForward=GetTagId("access:motor_vehicle:foward");
    tagAccessMotorVehicleBackward=GetTagId("access:motor_vehicle:backward");

    tagAccessMotorcar=GetTagId("access:motorcar");
    tagAccessMotorcarForward=GetTagId("access:motorcar:foward");
    tagAccessMotorcarBackward=GetTagId("access:motorcar:backward");

    tagAddrStreet=GetTagId("addr:street");

    tagArea=GetTagId("area");
    tagNatural=GetTagId("natural");
    tagType=GetTagId("type");
    tagRestriction=GetTagId("restriction");

    assert(tagRef!=tagIgnore);
    assert(tagBridge!=tagIgnore);
    assert(tagTunnel!=tagIgnore);
    assert(tagLayer!=tagIgnore);
    assert(tagWidth!=tagIgnore);
    assert(tagOneway!=tagIgnore);
    assert(tagHouseNr!=tagIgnore);
    assert(tagJunction!=tagIgnore);
    assert(tagMaxSpeed!=tagIgnore);
    assert(tagSurface!=tagIgnore);
    assert(tagTracktype!=tagIgnore);
    assert(tagAdminLevel!=tagIgnore);

    assert(tagAccess!=tagIgnore);
    assert(tagAccessForward!=tagIgnore);
    assert(tagAccessBackward!=tagIgnore);

    assert(tagAccessFoot!=tagIgnore);
    assert(tagAccessFootForward!=tagIgnore);
    assert(tagAccessFootBackward!=tagIgnore);

    assert(tagAccessBicycle!=tagIgnore);
    assert(tagAccessBicycleForward!=tagIgnore);
    assert(tagAccessBicycleBackward!=tagIgnore);

    assert(tagAccessMotorVehicle!=tagIgnore);
    assert(tagAccessMotorVehicleForward!=tagIgnore);
    assert(tagAccessMotorVehicleBackward!=tagIgnore);

    assert(tagAccessMotorcar!=tagIgnore);
    assert(tagAccessMotorcarForward!=tagIgnore);
    assert(tagAccessMotorcarBackward!=tagIgnore);

    assert(tagAddrStreet!=tagIgnore);

    assert(tagArea!=tagIgnore);
    assert(tagNatural!=tagIgnore);
    assert(tagType!=tagIgnore);
    assert(tagRestriction!=tagIgnore);
  }

  TypeConfig::~TypeConfig()
  {
    // no code
  }

  const std::vector<TagInfo>& TypeConfig::GetTags() const
  {
    return tags;
  }

  const std::vector<TypeInfoRef>& TypeConfig::GetTypes() const
  {
    return types;
  }

  TagId TypeConfig::RegisterTagForInternalUse(const std::string& tagName)
  {
    OSMSCOUT_HASHMAP<std::string,TagId>::const_iterator mapping=stringToTagMap.find(tagName);

    if (mapping!=stringToTagMap.end()) {
      return mapping->second;
    }

    TagInfo tagInfo(tagName,true);

    if (tagInfo.GetId()==0) {
      tagInfo.SetId(nextTagId);

      nextTagId++;
    }
    else {
      nextTagId=std::max(nextTagId,(TagId)(tagInfo.GetId()+1));
    }

    tags.push_back(tagInfo);
    stringToTagMap[tagInfo.GetName()]=tagInfo.GetId();

    return tagInfo.GetId();
  }

  TagId TypeConfig::RegisterTagForExternalUse(const std::string& tagName)
  {
    OSMSCOUT_HASHMAP<std::string,TagId>::const_iterator mapping=stringToTagMap.find(tagName);

    if (mapping!=stringToTagMap.end()) {
      tags[mapping->second].SetToExternal();

      return mapping->second;
    }

    TagInfo tagInfo(tagName,false);

    if (tagInfo.GetId()==0) {
      tagInfo.SetId(nextTagId);

      nextTagId++;
    }
    else {
      nextTagId=std::max(nextTagId,(TagId)(tagInfo.GetId()+1));
    }

    tags.push_back(tagInfo);
    stringToTagMap[tagInfo.GetName()]=tagInfo.GetId();

    return tagInfo.GetId();
  }

  TagId TypeConfig::RegisterNameTag(const std::string& tagName, uint32_t priority)
  {
    TagId tagId=RegisterTagForExternalUse(tagName);

    nameTagIdToPrioMap.insert(std::make_pair(tagId,priority));

    return tagId;
  }

  TagId TypeConfig::RegisterNameAltTag(const std::string& tagName, uint32_t priority)
  {
    TagId tagId=RegisterTagForExternalUse(tagName);

    nameAltTagIdToPrioMap.insert(std::make_pair(tagId,priority));

    return tagId;
  }

  void TypeConfig::RegisterFeature(const FeatureRef& feature)
  {
    assert(feature.Valid());
    assert(!feature->GetName().empty());

    nameToFeatureMap[feature->GetName()]=feature;

    feature->Initialize(*this);
  }

  FeatureRef TypeConfig::GetFeature(const std::string& name) const
  {
    OSMSCOUT_HASHMAP<std::string,FeatureRef>::const_iterator feature=nameToFeatureMap.find(name);

    if (feature!=nameToFeatureMap.end()) {
      return feature->second;
    }
    else {
      return NULL;
    }
  }

  TypeConfig& TypeConfig::AddTypeInfo(const TypeInfoRef& typeInfo)
  {
    assert(typeInfo.Valid());

    if (nameToTypeMap.find(typeInfo->GetName())!=nameToTypeMap.end()) {
      return *this;
    }

    if ((typeInfo->CanBeArea() || typeInfo->CanBeNode()) &&
        !typeInfo->HasFeature(AddressFeature::NAME)) {
      typeInfo->AddFeature(GetFeature(AddressFeature::NAME));
    }

    if (typeInfo->GetId()==0) {
      typeInfo->SetId(nextTypeId);

      nextTypeId++;
    }
    else {
      nextTypeId=std::max(nextTypeId,(TypeId)(typeInfo->GetId()+1));
    }

    //std::cout << "Type: " << typeInfo.GetId() << " " << typeInfo.GetName() << std::endl;

    types.push_back(typeInfo);
    nameToTypeMap[typeInfo->GetName()]=typeInfo;

    idToTypeMap[typeInfo->GetId()]=typeInfo;

    return *this;
  }

  TypeId TypeConfig::GetMaxTypeId() const
  {
    if (nextTypeId==0) {
      return 0;
    }
    else {
      return nextTypeId-1;
    }
  }

  TagId TypeConfig::GetTagId(const char* name) const
  {
    OSMSCOUT_HASHMAP<std::string,TagId>::const_iterator iter=stringToTagMap.find(name);

    if (iter!=stringToTagMap.end()) {
      return iter->second;
    }
    else {
      return tagIgnore;
    }
  }

  const TagInfo& TypeConfig::GetTagInfo(TagId id) const
  {
    assert(id<tags.size());

    return tags[id];
  }

  const TypeInfoRef& TypeConfig::GetTypeInfo(TypeId id) const
  {
    assert(id<types.size());

    return types[id];
  }

  void TypeConfig::ResolveTags(const std::map<TagId,std::string>& map,
                               std::vector<Tag>& tags) const
  {
    tags.clear();

    for (std::map<TagId,std::string>::const_iterator t=map.begin();
         t!=map.end();
         ++t) {
      if (GetTagInfo(t->first).IsInternalOnly()) {
        continue;
      }

      Tag tag;

      tag.key=t->first;
      tag.value=t->second;

      tags.push_back(tag);
    }
  }

  bool TypeConfig::IsNameTag(TagId tag, uint32_t& priority) const
  {
    if (nameTagIdToPrioMap.empty()) {
      return false;
    }

    OSMSCOUT_HASHMAP<TagId,uint32_t>::const_iterator entry=nameTagIdToPrioMap.find(tag);

    if (entry==nameTagIdToPrioMap.end()) {
      return false;
    }

    priority=entry->second;

    return true;
  }

  bool TypeConfig::IsNameAltTag(TagId tag, uint32_t& priority) const
  {
    if (nameAltTagIdToPrioMap.empty()) {
      return false;
    }

    OSMSCOUT_HASHMAP<TagId,uint32_t>::const_iterator entry=nameAltTagIdToPrioMap.find(tag);

    if (entry==nameAltTagIdToPrioMap.end()) {
      return false;
    }

    priority=entry->second;

    return true;
  }

  TypeInfoRef TypeConfig::GetNodeType(const std::map<TagId,std::string>& tagMap) const
  {
    if (tagMap.empty()) {
      return typeInfoIgnore;
    }

    for (auto type : types) {
      if (!type->HasConditions() ||
          !type->CanBeNode()) {
        continue;
      }

      for (auto cond : type->GetConditions()) {
        if (!(cond.types & TypeInfo::typeNode)) {
          continue;
        }

        if (cond.condition->Evaluate(tagMap)) {
          return type;
        }
      }
    }

    return typeInfoIgnore;
  }

  bool TypeConfig::GetWayAreaTypeId(const std::map<TagId,std::string>& tagMap,
                                    TypeId &wayType,
                                    TypeId &areaType) const
  {
    wayType=typeIgnore;
    areaType=typeIgnore;

    if (tagMap.empty()) {
      return false;
    }

    for (size_t i=0; i<types.size(); i++) {
      if (!((types[i]->CanBeWay() ||
             types[i]->CanBeArea()) &&
             types[i]->HasConditions())) {
        continue;
      }

      for (std::list<TypeInfo::TypeCondition>::const_iterator cond=types[i]->GetConditions().begin();
           cond!=types[i]->GetConditions().end();
           ++cond) {
        if (!((cond->types & TypeInfo::typeWay) || (cond->types & TypeInfo::typeArea))) {
          continue;
        }

        if (cond->condition->Evaluate(tagMap)) {
          if (wayType==typeIgnore &&
              (cond->types & TypeInfo::typeWay)) {
            wayType=types[i]->GetId();
          }

          if (areaType==typeIgnore &&
              (cond->types & TypeInfo::typeArea)) {
            areaType=types[i]->GetId();
          }

          if (wayType!=typeIgnore ||
              areaType!=typeIgnore) {
            return true;
          }
        }
      }
    }

    return false;
  }

  bool TypeConfig::GetRelationTypeId(const std::map<TagId,std::string>& tagMap,
                                     TypeId &typeId) const
  {
    typeId=typeIgnore;

    if (tagMap.empty()) {
      return false;
    }

    std::map<TagId,std::string>::const_iterator relationType=tagMap.find(tagType);

    if (relationType!=tagMap.end() &&
        relationType->second=="multipolygon") {
      for (size_t i=0; i<types.size(); i++) {
        if (!types[i]->HasConditions() ||
            !types[i]->CanBeArea()) {
          continue;
        }

        for (std::list<TypeInfo::TypeCondition>::const_iterator cond=types[i]->GetConditions().begin();
             cond!=types[i]->GetConditions().end();
             ++cond) {
          if (!(cond->types & TypeInfo::typeArea)) {
            continue;
          }

          if (cond->condition->Evaluate(tagMap)) {
            typeId=types[i]->GetId();
            return true;
          }
        }
      }
    }
    else {
      for (size_t i=0; i<types.size(); i++) {
        if (!types[i]->HasConditions() ||
            !types[i]->CanBeRelation()) {
          continue;
        }

        for (std::list<TypeInfo::TypeCondition>::const_iterator cond=types[i]->GetConditions().begin();
             cond!=types[i]->GetConditions().end();
             ++cond) {
          if (!(cond->types & TypeInfo::typeRelation)) {
            continue;
          }

          if (cond->condition->Evaluate(tagMap)) {
            typeId=types[i]->GetId();
            return true;
          }
        }
      }
    }

    return false;
  }

  TypeId TypeConfig::GetTypeId(const std::string& name) const
  {
    OSMSCOUT_HASHMAP<std::string,TypeInfoRef>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end()) {
      return iter->second->GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetNodeTypeId(const std::string& name) const
  {
    OSMSCOUT_HASHMAP<std::string,TypeInfoRef>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second->CanBeNode()) {
      return iter->second->GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetWayTypeId(const std::string& name) const
  {
    OSMSCOUT_HASHMAP<std::string,TypeInfoRef>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second->CanBeWay()) {
      return iter->second->GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetAreaTypeId(const std::string& name) const
  {
    OSMSCOUT_HASHMAP<std::string,TypeInfoRef>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second->CanBeArea()) {
      return iter->second->GetId();
    }

    return typeIgnore;
  }

  TypeId TypeConfig::GetRelationTypeId(const std::string& name) const
  {
    OSMSCOUT_HASHMAP<std::string,TypeInfoRef>::const_iterator iter=nameToTypeMap.find(name);

    if (iter!=nameToTypeMap.end() &&
        iter->second->CanBeRelation()) {
      return iter->second->GetId();
    }

    return typeIgnore;
  }

  void TypeConfig::GetAreaTypes(std::set<TypeId>& types) const
  {
    for (std::vector<TypeInfoRef>::const_iterator t=this->types.begin();
         t!=this->types.end();
         t++) {
      TypeInfoRef type(*t);

      if (type->GetId()==typeIgnore) {
        continue;
      }

      if (type->GetIgnore()) {
        continue;
      }

      if (type->CanBeArea()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::GetWayTypes(std::set<TypeId>& types) const
  {
    for (std::vector<TypeInfoRef>::const_iterator t=this->types.begin();
         t!=this->types.end();
         t++) {
      TypeInfoRef type(*t);

      if (type->GetId()==typeIgnore) {
        continue;
      }

      if (type->GetIgnore()) {
        continue;
      }

      if (type->CanBeWay()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::GetRoutables(std::set<TypeId>& types) const
  {
    types.clear();

    for (std::vector<TypeInfoRef>::const_iterator t=this->types.begin();
         t!=this->types.end();
         ++t) {
      TypeInfoRef type(*t);

      if (type->CanRouteFoot() ||
          type->CanRouteBicycle() ||
          type->CanRouteCar()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::GetIndexAsLocationTypes(OSMSCOUT_HASHSET<TypeId>& types) const
  {
    types.clear();

    for (std::vector<TypeInfoRef>::const_iterator t=this->types.begin();
         t!=this->types.end();
         ++t) {
      TypeInfoRef type(*t);

      if (type->GetIndexAsLocation()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::GetIndexAsRegionTypes(OSMSCOUT_HASHSET<TypeId>& types) const
  {
    types.clear();

    for (std::vector<TypeInfoRef>::const_iterator t=this->types.begin();
         t!=this->types.end();
         ++t) {
      TypeInfoRef type(*t);

      if (type->GetIndexAsRegion()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::GetIndexAsPOITypes(OSMSCOUT_HASHSET<TypeId>& types) const
  {
    types.clear();

    for (std::vector<TypeInfoRef>::const_iterator t=this->types.begin();
         t!=this->types.end();
         ++t) {
      TypeInfoRef type(*t);

      if (type->GetIndexAsPOI()) {
        types.insert(type->GetId());
      }
    }
  }

  void TypeConfig::RegisterSurfaceToGradeMapping(const std::string& surface,
                                                 size_t grade)
  {
    surfaceToGradeMap.insert(std::make_pair(surface,
                                            grade));
  }

  bool TypeConfig::GetGradeForSurface(const std::string& surface,
                                      size_t& grade) const
  {
    OSMSCOUT_HASHMAP<std::string,size_t>::const_iterator entry=surfaceToGradeMap.find(surface);

    if (entry!=surfaceToGradeMap.end()) {
      grade=entry->second;

      return true;
    }
    else {
      return false;
    }
  }

  /**
   * Loads the type configuration from the given *.ost file.
   *
   * Note:
   * Make sure that you load from a OST file only onto a freshly initialized
   * TypeConfig instance.
   *
   * @param filename
   *    Full filename including path of the OST file
   * @return
   *    True, if there were no errors, else false
   */
  bool TypeConfig::LoadFromOSTFile(const std::string& filename)
  {
    FileOffset fileSize;
    FILE*      file;
    bool       success=false;

    if (!GetFileSize(filename,
                     fileSize)) {
      std::cerr << "Cannot get size of file '" << filename << "'" << std::endl;
      return false;
    }

    file=fopen(filename.c_str(),"rb");
    if (file==NULL) {
      std::cerr << "Cannot open file '" << filename << "'" << std::endl;
      return false;
    }

    unsigned char* content=new unsigned char[fileSize];

    if (fread(content,1,fileSize,file)!=(size_t)fileSize) {
      std::cerr << "Cannot load file '" << filename << "'" << std::endl;
      delete [] content;
      fclose(file);
      return false;
    }

    fclose(file);

    ost::Scanner *scanner=new ost::Scanner(content,
                                           fileSize);
    ost::Parser  *parser=new ost::Parser(scanner,
                                         *this);

    delete [] content;

    parser->Parse();

    success=!parser->errors->hasErrors;

    delete parser;
    delete scanner;

    return success;
  }

  /**
   * Loads the type configuration from the given binary data file.
   *
   * Note:
   * Make sure that you load from afile only onto a freshly initialized
   * TypeConfig instance.
   *
   * @param directory
   *    Full path excluding the actual filename of the data file
   *    (filename is always "types.dat")
   * @return
   *    True, if there were no errors, else false
   */
  bool TypeConfig::LoadFromDataFile(const std::string& directory)
  {
    FileScanner scanner;

    if (!scanner.Open(AppendFileToDir(directory,
                                      "types.dat"),
                      FileScanner::Sequential,
                      true)) {
      std::cerr << "Cannot open file '" << scanner.GetFilename() << "'" << std::endl;
     return false;
    }

    uint32_t tagCount;

    if (!scanner.ReadNumber(tagCount)) {
      std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
      return false;
    }

    for (size_t i=1; i<=tagCount; i++) {
      TagId       requestedId;
      TagId       actualId;
      std::string name;
      bool        internalOnly;

      if (!(scanner.ReadNumber(requestedId) &&
            scanner.Read(name),
            scanner.Read(internalOnly))) {
        std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
        return false;
      }

      if (internalOnly) {
        actualId=RegisterTagForInternalUse(name);
      }
      else {
        actualId=RegisterTagForExternalUse(name);
      }

      if (actualId!=requestedId) {
        std::cerr << "Requested and actual tag id do not match" << std::endl;
        return false;
      }
    }

    uint32_t nameTagCount;

    if (!scanner.ReadNumber(nameTagCount)) {
      std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
      return false;
    }

    for (size_t i=1; i<=nameTagCount; i++) {
      TagId       requestedId;
      TagId       actualId;
      std::string name;
      uint32_t    priority = 0;

      if (!(scanner.ReadNumber(requestedId) &&
            scanner.Read(name) &&
            scanner.ReadNumber(priority))) {
        std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
      }

      actualId=RegisterNameTag(name,priority);

      if (actualId!=requestedId) {
        std::cerr << "Requested and actual name tag id do not match" << std::endl;
        return false;
      }
    }

    uint32_t nameAltTagCount;

    if (!scanner.ReadNumber(nameAltTagCount)) {
      std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
      return false;
    }

    for (size_t i=1; i<=nameAltTagCount; i++) {
      TagId       requestedId;
      TagId       actualId;
      std::string name;
      uint32_t    priority = 0;

      if (!(scanner.ReadNumber(requestedId) &&
            scanner.Read(name) &&
            scanner.ReadNumber(priority))) {
        std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
      }

      actualId=RegisterNameAltTag(name,priority);

      if (actualId!=requestedId) {
        std::cerr << "Requested and actual name alt tag id do not match" << std::endl;
        return false;
      }
    }

    uint32_t typeCount;

    if (!scanner.ReadNumber(typeCount)) {
      std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
      return false;
    }

    for (size_t i=1; i<=typeCount; i++) {
      TypeId      id;
      std::string name;
      bool        canBeNode;
      bool        canBeWay;
      bool        canBeArea;
      bool        canBeRelation;
      bool        canRouteFoot;
      bool        canRouteBicycle;
      bool        canRouteCar;
      bool        indexAsLocation;
      bool        indexAsRegion;
      bool        indexAsPOI;
      bool        optimizeLowZoom;
      bool        multipolygon;
      bool        pinWay;
      bool        ignore;
      bool        ignoreSeaLand;

      if (!(scanner.ReadNumber(id) &&
            scanner.Read(name) &&
            scanner.Read(canBeNode) &&
            scanner.Read(canBeWay) &&
            scanner.Read(canBeArea) &&
            scanner.Read(canBeRelation) &&
            scanner.Read(canRouteFoot) &&
            scanner.Read(canRouteBicycle) &&
            scanner.Read(canRouteCar) &&
            scanner.Read(indexAsLocation) &&
            scanner.Read(indexAsRegion) &&
            scanner.Read(indexAsPOI) &&
            scanner.Read(optimizeLowZoom) &&
            scanner.Read(multipolygon) &&
            scanner.Read(pinWay) &&
            scanner.Read(ignoreSeaLand) &&
            scanner.Read(ignore))) {

        std::cerr << "Format error in file '" << scanner.GetFilename() << "'" << std::endl;
        return false;
      }

      TypeInfoRef typeInfo=new TypeInfo();

      typeInfo->SetId(id);

      typeInfo->SetType(name);

      typeInfo->CanBeNode(canBeNode);
      typeInfo->CanBeWay(canBeWay);
      typeInfo->CanBeArea(canBeArea);
      typeInfo->CanBeRelation(canBeRelation);
      typeInfo->CanRouteFoot(canRouteFoot);
      typeInfo->CanRouteBicycle(canRouteBicycle);
      typeInfo->CanRouteCar(canRouteCar);
      typeInfo->SetIndexAsLocation(indexAsLocation);
      typeInfo->SetIndexAsRegion(indexAsRegion);
      typeInfo->SetIndexAsPOI(indexAsPOI);
      typeInfo->SetIgnore(optimizeLowZoom);
      typeInfo->SetIgnore(multipolygon);
      typeInfo->SetIgnore(pinWay );
      typeInfo->SetIgnore(ignoreSeaLand);
      typeInfo->SetIgnore(ignore);

      uint32_t featureCount;

      if (!scanner.ReadNumber(featureCount)) {
        return false;
      }

      for (size_t i=0; i<featureCount; i++) {
        std::string featureName;

        if (!scanner.Read(featureName)) {
          return false;
        }

        FeatureRef feature=GetFeature(featureName);

        if (feature.Invalid()) {
          std::cerr << "Feature '" << featureName << "' not found" << std::endl;
          return false;
        }

        typeInfo->AddFeature(feature);
      }

      AddTypeInfo(typeInfo);
    }

    return !scanner.HasError() && scanner.Close();
  }

  /**
   * Store the part of the TypeConfig information to a data file,
   * which is necessary to review later on when reading and
   * evaluation an import.
   *
   * @param directory
   *    Directory the data file should be written to
   * @return
   *    True, if there were no errors, else false
   */
  bool TypeConfig::StoreToDataFile(const std::string& directory) const
  {
    FileWriter writer;

     if (!writer.Open(AppendFileToDir(directory,
                                      "types.dat"))) {
       //progress.Error("Cannot create 'types.dat'");
       return false;
     }

     writer.WriteNumber((uint32_t)GetTags().size());
     for (std::vector<TagInfo>::const_iterator tag=GetTags().begin();
          tag!=GetTags().end();
          ++tag) {
       writer.WriteNumber(tag->GetId());
       writer.Write(tag->GetName());
       writer.Write(tag->IsInternalOnly());
     }

     uint32_t nameTagCount=0;
     uint32_t nameAltTagCount=0;

     for (std::vector<TagInfo>::const_iterator tag=GetTags().begin();
          tag!=GetTags().end();
          ++tag) {
       uint32_t priority;

       if (IsNameTag(tag->GetId(),priority)) {
         nameTagCount++;
       }

       if (IsNameAltTag(tag->GetId(),priority)) {
         nameAltTagCount++;
       }
     }

     writer.WriteNumber(nameTagCount);
     for (std::vector<TagInfo>::const_iterator tag=GetTags().begin();
      tag!=GetTags().end();
      ++tag) {
       uint32_t priority;

       if (IsNameTag(tag->GetId(),priority)) {
         writer.WriteNumber(tag->GetId());
         writer.Write(tag->GetName());
         writer.WriteNumber((uint32_t)priority);
       }
     }

     writer.WriteNumber(nameAltTagCount);
     for (std::vector<TagInfo>::const_iterator tag=GetTags().begin();
      tag!=GetTags().end();
      ++tag) {
       uint32_t priority;

       if (IsNameAltTag(tag->GetId(),priority)) {
         writer.WriteNumber(tag->GetId());
         writer.Write(tag->GetName());
         writer.WriteNumber((uint32_t)priority);
       }
     }


     writer.WriteNumber((uint32_t)GetTypes().size());

     for (auto type : GetTypes()) {
       writer.WriteNumber(type->GetId());
       writer.Write(type->GetName());
       writer.Write(type->CanBeNode());
       writer.Write(type->CanBeWay());
       writer.Write(type->CanBeArea());
       writer.Write(type->CanBeRelation());
       writer.Write(type->CanRouteFoot());
       writer.Write(type->CanRouteBicycle());
       writer.Write(type->CanRouteCar());
       writer.Write(type->GetIndexAsLocation());
       writer.Write(type->GetIndexAsRegion());
       writer.Write(type->GetIndexAsPOI());
       writer.Write(type->GetOptimizeLowZoom());
       writer.Write(type->GetMultipolygon());
       writer.Write(type->GetPinWay());
       writer.Write(type->GetIgnoreSeaLand());
       writer.Write(type->GetIgnore());

       writer.WriteNumber((uint32_t)type->GetFeatures().size());
       for (auto feature : type->GetFeatures()) {
         writer.Write(feature.GetFeature()->GetName());
       }
     }

     return !writer.HasError() && writer.Close();
  }
}
