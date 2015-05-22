//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/rtti/tType.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-11-02
 *
 * \brief   Contains tType
 *
 * \b tType
 *
 * Class with runtime information on all kinds of C++ types.
 *
 * A unique 16 bit type-id is assigned to each type.
 *
 * Can be used as factory to create instances of C++ classes
 * (necessary for deserializing vectors containing pointers).
 *
 * To create a tType object containing information on a C++ class,
 * an instance of DataType<T> is to be created.
 *
 * One such object needs be created before a type T can be
 * deserialized/looked up etc. generically.
 *
 * tType is passed by value
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tType_h__
#define __rrlib__rtti__tType_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tNoncopyable.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
namespace serialization
{
class tInputStream;
class tOutputStream;
class tStringInputStream;
class tStringOutputStream;
}

namespace rtti
{
class tTypeAnnotation;
class tGenericObject;
class tFactory;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Data type
/*!
 * Class with runtime information on all kinds of C++ types.
 *
 * A unique 16 bit type-id is assigned to each type.
 *
 * Can be used as factory to create instances of C++ classes
 * (necessary for deserializing vectors containing pointers).
 *
 * To create a tType object containing information on a C++ class,
 * an instance of DataType<T> is to be created.
 *
 * One such object needs be created before a type T can be
 * deserialized/looked up etc. generically.
 *
 * tType is passed by value
 */
class tType
{
protected:
  struct tInfo;

  /*! Maximum number of annotations per type */
  enum { cMAX_ANNOTATIONS = 10 };

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*! Classifies wrapped type */
  enum class tClassification
  {
    PLAIN, LIST, PTR_LIST, NULL_TYPE, OTHER, UNKNOWN
  };

  tType() : info(NULL) {}

  /*!
   * Add annotation to this data type.
   *
   * \param annotation Annotation (may only be added to one data type. Is deleted automatically on data type destruction.)
   * \tparam T Annotation type. Must be derived from tDataTypeAnnotation.
   *           T is used for lookup later (may also be a base class of annotation).
   *           Only one annotation object per type T may be added
   */
  template <typename T>
  inline void AddAnnotation(T* annotation)
  {
    AddAnnotationImplementation(annotation, AnnotationIndexValid<T>, tAnnotationIndex<T>::index);
  }

  /*!
   * \param placement (Optional) Destination for placement new (needs to have at least size GetSize(emplace_generic_object))
   * \param emplace_generic_object If placement is specified: place generic objects at this memory address (true) or only data (false)?
   * \return Instance of data type wrapped as tGenericObject
   */
  inline tGenericObject* CreateInstanceGeneric(void* placement = NULL, bool emplace_generic_object = true) const
  {
    if (info)
    {
      return info->CreateInstanceGeneric(placement, emplace_generic_object);
    }
    return NULL;
  }

  /*!
   * Deep copy objects
   *
   * \param source Source object
   * \param destination Destination object
   * \param factory Factory to use (optional, required for pointer lists etc.)
   */
  void DeepCopy(const void* source, void* destination, tFactory* factory = NULL) const;

  /*!
   * Lookup data type by name.
   *
   * If a match with the complete namespace cannot be found, then
   * 1) if 'name' does not contain a namespace, any type with the same name including a namespace will be returned as a match
   * 2) if 'name' contains a namespace, a type with the same name but without namespace will be returned as a match
   *
   * \param name Data Type name
   * \return Data type with specified name (== NULL if it could not be found)
   */
  static tType FindType(const std::string& name);

  /*!
   * Lookup data type by rtti name
   *
   * \param rtti_name rtti name
   * \return Data type with specified name (== NULL if it could not be found)
   */
  static tType FindTypeByRtti(const char* rtti_name);

  /*!
   * Get annotation of specified class
   *
   * \param c Class of annotation we're looking for
   * \return Annotation. Null if data type has no annotation of this type.
   */
  template <typename T>
  inline T* GetAnnotation() const
  {
    return info ? static_cast<T*>(info->annotations[tAnnotationIndex<T>::index]) : NULL;
  }

  /*!
   * \param include_path Return file name including the path name?
   * \return binary file that initializes data type statically.
   * On non-Linux platforms this always returns the empty string.
   */
  const std::string GetBinary(bool include_path) const
  {
    if (info)
    {
      if (include_path || info->binary.find('/') == std::string::npos)
      {
        return info->binary;
      }
      else
      {
        return info->binary.substr(info->binary.rfind('/') + 1);
      }
    }
    return "";
  }

  /*!
   * Obtain type name in rrlib::rtti format
   * (no 't' prefixes; '.' instead of '::' for namespace separation; e.g. "rrlib.distance_data.DistanceData")
   * from rtti type name.
   *
   * \param rtti Mangled rtti type name
   * \param remove_namespaces Remove namespaces from type names? (should only be used if name collisions are not an issue or cannot occur)
   * \return (Demangled) type name in rrlib::rtti format
   */
  static std::string GetTypeNameFromRtti(const char* rtti, bool remove_namespaces = false);

  /*!
   * \return In case of list: type of elements
   */
  inline tType GetElementType() const
  {
    return tType(info ? info->element_type : NULL);
  }

  /*!
   * \return Pointer to enum strings data if this is an enum type - otherwise NULL
   */
  const make_builder::internal::tEnumStrings* GetEnumStringsData()
  {
    return info ? info->enum_strings : NULL;
  }

  /*!
   * \return Vector with all numeric constants in string representation if this is an enum with custom (non-standard) values - otherwise NULL (for an enum meaning we have: 0, 1, 2, ..., n)
   */
  const std::vector<std::string>* GetNonStandardEnumValueStrings()
  {
    return info && info->non_standard_enum_value_strings.size() ? &info->non_standard_enum_value_strings : NULL;
  }

  /*!
   * \return If this is a plain type and a list type has been initialized: list type (std::vector<T>) - otherwise NULL
   */
  inline tType GetListType() const
  {
    return tType(info ? info->list_type : NULL);
  }

  /*!
   * \param without_namespace Return name of data type without any namespaces?
   * \return Name of data type
   */
  inline const std::string& GetName(bool without_namespace = false) const
  {
    static const std::string null_type_string = "NULL";
    return info ? (without_namespace ? info->short_name : info->name) : null_type_string;
  }

  /*!
   * \return rtti name of data type  (note that this is the 'normalized' type for integral types - e.g. "i" for 'long' on 32 bit platforms)
   */
  const char* GetRttiName() const
  {
    return info ? info->rtti_name : typeid(void).name();
  }

  /*!
   * \return Demangled rtti name of data type
   */
  inline const std::string& GetRttiNameDemangled() const
  {
    static const std::string null_type_string = "NULL";
    return info ? info->demangled_rtti_name : null_type_string;
  }

  /*!
   * \return In case of element: shared pointer list type (std::vector<std::shared_ptr<T>>)
   */
  inline tType GetSharedPtrListType() const
  {
    return tType(info ? info->shared_ptr_list_type : NULL);
  }

  /*!
   * \param Obtain size as generic object?
   * \return size of data type (as returned from sizeof(T) or sizeof(tGenericObjectInstance<T>))
   */
  size_t GetSize(bool as_generic_object = false) const
  {
    return info ? (as_generic_object ? info->generic_object_size : info->size) : 0;
  }

  /*!
   * \return returns "Type" of data type (see enum)
   */
  inline tClassification GetType() const
  {
    return info ? info->type : tClassification::NULL_TYPE;
  }

  /*!
   * \param uid Data type uid
   * \return Data type with specified uid (== NULL if there's no type with this uid)
   */
  static tType GetType(int16_t uid);

  /*!
   * \return Number of registered types
   */
  static uint16_t GetTypeCount();

  /*!
   * \return Bit vector of type traits determined at compile time (see type_traits.h)
   */
  inline int GetTypeTraits() const
  {
    return info ? info->type_traits : 0;
  }

  /*!
   * \return uid of data type
   */
  inline int16_t GetUid() const
  {
    return info ? info->uid : -1;
  }

  /*!
   * Can object of this data type be converted to specified type?
   * (In C++ currently only returns true, when types are equal)
   *
   * \param data_type Other type
   * \return Answer
   */
  inline bool IsConvertibleTo(const tType& data_type) const
  {
    return data_type == *this;
  }

  /*!
   * for checks against NULL (if (type == NULL) {...} )
   */
  bool operator== (void* info_ptr) const
  {
    return info == info_ptr;
  }

  bool operator== (const tType& other) const
  {
    return info == other.info;
  }

  bool operator!= (void* info_ptr) const
  {
    return info != info_ptr;
  }

  bool operator!= (const tType& other) const
  {
    return info != other.info;
  }

  bool operator< (const tType& other) const
  {
    return info < other.info;
  }

  /*!
   * Deserialize object from input stream
   *
   * \param os InputStream
   * \param obj Object to deserialize
   */
  void Deserialize(serialization::tInputStream& is, void* obj) const;

  /*!
   * Serialize object to output stream
   *
   * \param os OutputStream
   * \param obj Object to serialize
   */
  void Serialize(serialization::tOutputStream& os, const void* obj) const;

//----------------------------------------------------------------------
// Protected information class
//----------------------------------------------------------------------
protected:

  /*! Generic data type information */
  struct tInfo : private util::tNoncopyable
  {
    /*! Type of data type */
    const tType::tClassification type;

    /*! Name of data type */
    const std::string name;

    /*! Short name of data type */
    const std::string short_name;

    /*! RTTI name */
    const char* rtti_name;

    /*! Demangled RTTI name */
    const std::string demangled_rtti_name;

    /*! sizeof(T) */
    size_t size;

    /*! sizeof(tGenericObjectInstance<T>) */
    size_t generic_object_size;

    /*! Bit vector of type traits determined at compile time (see tTypeTraitVector) */
    int type_traits;

    /*! New info? */
    bool new_info;

    /*! Data type uid */
    int16_t uid;

    /*! In case of list: type of elements */
    tInfo* element_type;

    /*! In case of element: list type (std::vector<T>) */
    tInfo* list_type;

    /*! In case of element: shared pointer list type (std::vector<std::shared_ptr<T>>) */
    tInfo* shared_ptr_list_type;

    /*! Annotations to data type */
    tTypeAnnotation* annotations[cMAX_ANNOTATIONS];

    /*! binary file that initializes data type statically (includes path) */
    std::string binary;

    /*! pointer to enum string constants data - if this is an enum type */
    const make_builder::internal::tEnumStrings* enum_strings;

    /*! Strings of enum values - if they are custom (non-standard) */
    std::vector<std::string> non_standard_enum_value_strings;


    tInfo(tType::tClassification classification, const char* rtti_name, const std::string& name);

    virtual ~tInfo();

    /*!
     * \param placement (Optional) Destination for placement new
     * \param emplace_generic_object If placement is specified: place generic objects at this memory address (true) or only data (false)?
     * \return Instance of Datatype as Generic object
     */
    virtual tGenericObject* CreateInstanceGeneric(void* placement = NULL, bool emplace_generic_object = true) const
    {
      return NULL;
    }

    /*!
     * Deep copy objects
     *
     * \param src Src object
     * \param dest Destination object
     * \param f Factory to use
     */
    virtual void DeepCopy(const void* src, void* dest, tFactory* f) const;

    virtual void Init() {}

    /*!
     * Deserialize object from input stream
     *
     * \param os InputStream
     * \param obj Object to deserialize
     */
    virtual void Deserialize(serialization::tInputStream& is, void* obj) const;

    /*!
     * Serialize object to output stream
     *
     * \param os OutputStream
     * \param obj Object to serialize
     */
    virtual void Serialize(serialization::tOutputStream& os, const void* obj) const;

  };

  tType(tInfo* info);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Pointer to data type info (should not be copied every time for efficiency reasons) */
  const tInfo* info;

  /*! Helper struct for annotating types */
  template <typename T>
  struct tAnnotationIndex
  {
    static int index;
  };

  /*!
   * Implementation of AddAnnotation function
   */
  void AddAnnotationImplementation(tTypeAnnotation* annotation, bool (*annotation_index_valid_function)(bool), int& annotation_index);

  /*! Has specified type T a valid annotation index? */
  template <typename T>
  static bool AnnotationIndexValid(bool set_valid = false)
  {
    static bool valid = false;
    if (set_valid)
    {
      assert(!valid);
      valid = true;
    }
    return valid;
  }

  /*!
   * \param type_name Type name (in rrlib::rtti) format
   * \param The same type name without any namespaces (e.g. returns 'Pose2D' for 'rrlib.math.Pose2D')
   */
  static std::string RemoveNamespaces(const std::string& type_name);

};

template <typename T>
int tType::tAnnotationIndex<T>::index;

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tType& dt);
serialization::tInputStream& operator >> (serialization::tInputStream& stream, tType& dt);
serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tType& dt);
serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tType& dt);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
