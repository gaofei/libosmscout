#ifndef OSMSCOUT_TYPEFEATURE_H
#define OSMSCOUT_TYPEFEATURE_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2018  Tim Teulings

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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <osmscout/CoreImportExport.h>

#include <osmscout/util/FileScanner.h>
#include <osmscout/util/FileWriter.h>

#include <osmscout/util/TagErrorReporter.h>

namespace osmscout {

  // Forward declaration of classes TypeConfig and TypeInfo because
  // of circular dependency between them and Feature
  class FeatureValueBuffer;
  class FeatureInstance;
  class TypeConfig;
  class TypeInfo;

  class OSMSCOUT_API FeatureValue
  {
  public:
    FeatureValue();

    virtual ~FeatureValue()=default;

    inline virtual std::string GetLabel(size_t /*labelIndex*/) const
    {
      return "";
    }

    virtual void Read(FileScanner& scanner);

    virtual void Write(FileWriter& writer);

    virtual FeatureValue& operator=(const FeatureValue& other);

    virtual bool operator==(const FeatureValue& other) const = 0;

    virtual inline bool operator!=(const FeatureValue& other) const
    {
      return !(*this==other);
    }
  };

  /**
   * A feature combines one or multiple tags  to build information attribute for a type.
   *
   * The class "Feature" is the abstract base class for a concrete feature implementation
   * like "NameFeature" or "AccessFeature".
   *
   * A feature could just be an alias for one tag (like "name") but it could also combine
   * a number of attributes (e.g. access and all its variations).
   */
  class OSMSCOUT_API Feature
  {
  private:
    std::unordered_map<std::string,size_t>      labels;
    std::unordered_map<std::string,std::string> descriptions; //!< Map of descriptions for given language codes

  protected:
    size_t RegisterLabel(const std::string& labelName,
                         size_t index);

  public:
    Feature();

    virtual ~Feature()=default;

    /**
     * Does further initialization based on the current TypeConfig. For example
     * it registers Tags (and stores their TagId) for further processing.
     */
    virtual void Initialize(TypeConfig& typeConfig) = 0;

    void AddDescription(const std::string& languageCode,
                        const std::string& description);

    /**
     * Returns the name of the feature
     */
    virtual std::string GetName() const = 0;

    /**
     * A feature, if set for an object, can hold a value. If there is no value object,
     * this method returns 0, else it returns the C++ size of the value object.
     */
    inline virtual size_t GetValueSize() const
    {
      return 0;
    }

    /**
     * This method returns the number of additional feature bits reserved. If there are
     * additional features bit, 0 is returned.
     *
     * A feature may reserve additional feature bits. Feature bits should be used
     * if a custom value object is too expensive. Space for feature bits is always reserved
     * even if the feature itself is not set for a certain object.
     */
    inline virtual size_t GetFeatureBitCount() const
    {
      return 0;
    }

    /**
     * Returns 'true' if the feature has an value object.
     */
    inline virtual bool HasValue() const
    {
      return GetValueSize()>0;
    }

    /**
     * Returns 'true' if the feature provides labels.
     */
    inline virtual bool HasLabel() const
    {
      return !labels.empty();
    }

    /**
     * Returns the index of the label with the given name. Method returns 'true'
     * if the feature has labels and a label with the given name exists. Else
     * 'false' is returned.
     */
    bool GetLabelIndex(const std::string& labelName,
                       size_t& index) const;

    std::string GetDescription(const std::string& languageCode) const;

    inline const std::unordered_map<std::string,std::string>& GetDescriptions() const
    {
      return descriptions;
    };

    virtual FeatureValue* AllocateValue(void* buffer);

    virtual void Parse(TagErrorReporter& reporter,
                       const TypeConfig& typeConfig,
                       const FeatureInstance& feature,
                       const ObjectOSMRef& object,
                       const TagMap& tags,
                       FeatureValueBuffer& buffer) const = 0;
  };

  typedef std::shared_ptr<Feature> FeatureRef;

  /**
   * An instantiation of a feature for a certain type.
   */
  class OSMSCOUT_API FeatureInstance CLASS_FINAL
  {
  private:
    FeatureRef    feature;    //!< The feature we are an instance of
    const TypeInfo* type;      //!< The type we are assigned to (we are no Ref type to avoid circular references)
    size_t        featureBit; //!< index of the bit that signals that the feature is available
    size_t        index;      //!< The index we have in the list of features
    size_t        offset;     //!< Our offset into the value buffer for our data

  public:
    FeatureInstance();

    FeatureInstance(const FeatureRef& feature,
                    const TypeInfo* type,
                    size_t featureBit,
                    size_t index,
                    size_t offset);

    /**
     * Return the feature itself.
     */
    inline FeatureRef GetFeature() const
    {
      return feature;
    }

    /**
     * Return a pointer back tot he type we are assigned to.
     */
    inline const TypeInfo* GetType() const
    {
      return type;
    }

    /**
     * return the index of this feature within the list of features of the type.
     */
    inline size_t GetFeatureBit() const
    {
      return featureBit;
    }

    /**
     * return the index of this feature within the list of features of the type.
     */
    inline size_t GetIndex() const
    {
      return index;
    }

    /**
     * Return the file offset within the feature value buffer for the value of this feature.
     */
    inline size_t GetOffset() const
    {
      return offset;
    }
  };
}

#endif