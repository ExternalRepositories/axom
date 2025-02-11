// Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef AXOM_ARRAY_HPP_
#define AXOM_ARRAY_HPP_

#include "axom/config.hpp"                    // for compile-time defines
#include "axom/core/Macros.hpp"               // for axom macros
#include "axom/core/memory_management.hpp"    // for memory allocation functions
#include "axom/core/utilities/Utilities.hpp"  // for processAbort()
#include "axom/core/Types.hpp"                // for IndexType definition
#include "axom/core/IteratorBase.hpp"         // for Iterator

// C/C++ includes
#include <iostream>  // for std::cerr and std::ostream

namespace axom
{
// Forward declare the templated classes and operator function(s)
template <typename T>
class Array;

/// \name Overloaded Array Operator(s)
/// @{

/*! 
 * \brief Overloaded output stream operator. Outputs the Array to the
 *  given output stream.
 *
 * \param [in,out] os output stream object.
 * \param [in] arr user-supplied Array instance.
 * \return os the updated output stream object.
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const Array<T>& arr);

/*!
 * \brief Equality comparison operator for Arrays
 *
 * \param [in] lhs left Array to compare
 * \param [in] rhs right Array to compare
 * \return true if the Arrays have the same allocator ID, are of equal length,
 * and have the same elements.
 */
template <typename T>
bool operator==(const Array<T>& lhs, const Array<T>& rhs);

/*!
 * \brief Inequality comparison operator for Arrays
 *
 * \param [in] lhs left Array to compare
 * \param [in] rhs right Array to compare
 * \return true if the Arrays do not have the same allocator ID, are not of
 * equal length, or do not have the same elements.
 */
template <typename T>
bool operator!=(const Array<T>& lhs, const Array<T>& rhs);

/// @}

/*!
 * \class Array
 *
 * \brief Provides a generic array container.
 *
 *  The Array class provides a generic array container with
 *  dynamic reallocation and insertion. Each element in the array is
 *  stored contiguously.
 *
 *  \note For a multi-component array container, where each element
 *  is a tuple of 1 or more components, Axom provides the MCArray class.
 *
 *  The Array class mirrors std::vector, with future support for GPUs
 *  in-development.
 * 
 *  This class is meant to be a drop-in replacement for std::vector.
 *  However, it differs in its memory management and construction semantics.
 *  Specifically, we also allow axom::Array to wrap memory that it does
 *  not own (external storage), we do not require axom::Array to initialize/construct
 *  its memory at allocation time, and we use axom's memory_management
 *  and allocator ID abstractions rather than std::allocator.
 *
 *  Depending on which constructor is used, the Array object can have two
 *  different underlying storage types:
 *
 *  * <b> Native Storage </b> <br />
 *
 *     When using native storage, the Array object manages all memory.
 *     Typically, the Array object will allocate extra space to facilitate
 *     the insertion of new elements and minimize the number of reallocations.
 *     The actual capacity of the array (i.e., total number of elements that
 *     the Array can hold) can be queried by calling the capacity() function.
 *     When allocated memory is used up, inserting a new element triggers a
 *     reallocation.  At each reallocation, extra space is allocated
 *     according to the <em> resize_ratio </em> parameter, which is set to 2.0
 *     by default. To return all extra memory, an application can call
 *     `shrink()`.
 *
 *     \warning Reallocations tend to be costly operations in terms of performance.
 *      Use `reserve()` when the number of nodes is known a priori, or
 *      use a constructor that takes an actual size and capacity when possible.
 *
 *     \note The Array destructor deallocates and returns all memory associated
 *      with it to the system.
 *
 *  * <b> External Storage </b> <br />
 *
 *    An Array object may be constructed from an external, user-supplied buffer
 *    consisting of the given number of elements. In this case, the Array
 *    object does not own the memory.  Instead, the Array object makes a
 *    shallow copy of the pointer.
 *
 *    \warning An Array object that points to an external buffer has a fixed
 *     size and cannot be dynamically resized.
 *
 *    \note The Array destructor does not deallocate a user-supplied buffer,
 *     since it does not manage that memory.
 *
 *
 * \tparam T the type of the values to hold.
 *
 */
template <typename T>
class Array
{
public:
  static constexpr double DEFAULT_RESIZE_RATIO = 2.0;
  static constexpr IndexType MIN_DEFAULT_CAPACITY = 32;
  class ArrayIterator;

public:
  /// \name Native Storage Array Constructors
  /// @{

  /*! 
   * \brief Default constructor. Constructs an Array instance with no elements
   *  and default allocator ID. 
   *
   */
  Array();

  /*!
   * \brief Constructs an Array instance with the given number of elements.
   *
   * \param [in] num_elements the number of elements the Array holds.
   * \param [in] capacity the number of elements to allocate space for.
   * \param [in] allocator_id the ID of the allocator to use (optional)
   *
   * \note If no capacity or capacity less than num_elements is specified
   *  then it will default to at least num_elements * DEFAULT_RESIZE_RATIO.
   * \note a capacity is specified for the number of elements to store in the
   *  array and does not correspond to the actual bytesize.
   *
   * \pre num_elements >= 0
   *
   * \post capacity() >= size()
   * \post size() == num_elements
   * \post getResizeRatio() == DEFAULT_RESIZE_RATIO
   */
  Array(IndexType num_elements,
        IndexType capacity = 0,
        int allocator_id = axom::getDefaultAllocatorID());

  /*! 
   * \brief Copy constructor for an Array instance 
   * 
   * \param [in] allocator_id the ID of the allocator to use (optional)
   *
   * \note If you use the copy constructor on an argument Array 
   *  with data from an external data buffer, the copy-constructed Array 
   *  will have a deep copy of the data and own the data copy. 
   */
  Array(const Array& other, int allocator_id = axom::getDefaultAllocatorID());

  /*! 
   * \brief Move constructor for an Array instance 
   *
   * \note If you use the move constructor on an argument Array with an 
   *  external data buffer, the move-constructed Array will wrap the external 
   *  data buffer and the argument Array will be left in a valid empty state. 
   */
  Array(Array&& other);

  /// @}

  /// \name External Storage Array Constructors
  /// @{

  /*!
   * \brief Constructs an Array instance with the given number of elements from
   *  an external data buffer.
   *
   * \param [in] data the external data this Array will wrap.
   * \param [in] num_elements the number of elements in the Array.
   * \param [in] capacity the capacity of the external buffer.
   *
   * \pre data != nullptr
   * \pre num_elements > 0
   *
   * \post getResizeRatio == 0.0
   *
   * \note a capacity is specified for the number of elements to store in the
   *  array and does not correspond to the actual bytesize.
   * \note If no capacity or capacity less than num_elements is specified then
   *  it will default to the number of elements.
   *
   * \note This constructor wraps the supplied buffer and does not own the data.
   *  Consequently, the Array instance cannot be reallocated.
   */
  Array(T* data, IndexType num_elements, IndexType capacity = 0);

  /// @}

  /// \name Array copy and move operators
  /// @{

  /*! 
   * \brief Copy assignment operator for Array 
   * 
   * \note If you use the copy assignment operator on an argument Array 
   *  with data from an external data buffer, the copy-assigned Array 
   *  will have a deep copy of the data and own the data copy. 
   *
   * \note The data will be allocated using the allocator ID of the
   *  copy-assigned Array, not the argument Array.
   */
  Array& operator=(const Array& other)
  {
    if(this != &other)
    {
      m_resize_ratio = other.m_resize_ratio;
      m_is_external = false;
      initialize(other.size(), other.capacity());
      axom::copy(m_data, other.data(), m_num_elements * sizeof(T));
    }

    return *this;
  }

  /*! 
   * \brief Move assignment operator for Array 
   * 
   * \note If you use the move assignment operator on an argument Array with 
   *  an external data buffer, the move-assigned Array will wrap the external 
   *  data buffer and the argument Array will be left in a valid empty state. 
   */
  Array& operator=(Array&& other)
  {
    if(this != &other)
    {
      if(m_data != nullptr && !m_is_external)
      {
        axom::deallocate(m_data);
      }

      m_data = other.m_data;
      m_num_elements = other.m_num_elements;
      m_capacity = other.m_capacity;
      m_resize_ratio = other.m_resize_ratio;
      m_is_external = other.m_is_external;
      m_allocator_id = other.m_allocator_id;

      other.m_data = nullptr;
      other.m_num_elements = 0;
      other.m_capacity = 0;
      other.m_resize_ratio = DEFAULT_RESIZE_RATIO;
      other.m_is_external = false;
      other.m_allocator_id = INVALID_ALLOCATOR_ID;
    }

    return *this;
  }

  /// @}

  /*!
   * Destructor. Frees the associated buffer unless the memory is external.
   */
  virtual ~Array();

  /// \name Array element access operators
  /// @{

  /*!
   * \brief Accessor, returns a reference to the given value.
   *
   * \param [in] idx the position of the value to return.
   *
   * \note equivalent to *(array.data() + idx).
   *
   * \pre 0 <= idx < m_num_elements
   */
  /// @{

  T& operator[](IndexType idx)
  {
    assert(inBounds(idx));

    return m_data[idx];
  }

  const T& operator[](IndexType idx) const
  {
    assert(inBounds(idx));

    return m_data[idx];
  }

  /// @}

  /*!
   * \brief Return a pointer to the array of data.
   */
  /// @{

  T* data() { return m_data; }
  const T* data() const { return m_data; }

  /// @}

  /// @}

  /// \name Array methods to modify the data.
  /// @{

  /*!
   * \brief Set all the values of the array.
   *
   * \param [in] value the value to set to.
   */
  void fill(const T& value);

  /*!
   * \brief Push a value to the back of the array.
   *
   * \param [in] value the value to the back.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   */
  void push_back(const T& value);

  /*!
   * \brief Push a value to the back of the array.
   *
   * \param [in] value the value to move to the back.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   */
  void push_back(T&& value);

  /*!
   * \brief Modify the values of existing elements.
   *
   * \param [in] elements the new elements to write.
   * \param [in] n the number of elements to write.
   * \param [in] pos the position at which to begin writing.
   *
   * \note It's assumed that elements is of length n.
   * \note The size is unchanged by calls to set.
   *
   * \pre pos + n <= m_num_elements.
   */
  void set(const T* elements, IndexType n, IndexType pos);

  /*!
   * \brief Clears the contents of the array
   * 
   * \post size of Array is 0
   * \post capacity is unchanged
   */
  void clear();

  /*!
   * \brief Insert an element into the array at the given position.
   *
   * \param [in] pos the position at which to insert.
   * \param [in] value the element value to insert.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by 1.
   *
   */
  void insert(IndexType pos, const T& value);

  /*!
   * \brief Insert an element into the array at the value before pos.
   *
   * \param [in] pos the ArrayIterator before which value will be inserted.
   * \param [in] value the element value to insert.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by 1.
   *
   * \return ArrayIterator to inserted value
   */
  ArrayIterator insert(ArrayIterator pos, const T& value);

  /*!
   * \brief Insert elements into the array at the given position.
   *
   * \param [in] pos the position at which to begin the insertion.
   * \param [in] n the number of elements to insert.
   * \param [in] values the element values to insert.
   *
   * \note It's assumed that elements is of length n.
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by n.
   *
   * \pre pos <= m_num_elements.
   */
  void insert(IndexType pos, IndexType n, const T* values);

  /*!
   * \brief Insert elements into the array at the value before pos.
   *
   * \param [in] pos the ArrayIterator before which value will be inserted.
   * \param [in] n the number of elements to insert.
   * \param [in] values the element values to insert.
   *
   * \note It's assumed that elements is of length n.
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by n.
   *
   * \pre pos <= end()
   *
   * \return ArrayIterator to first element inserted (pos if n == 0)
   */
  ArrayIterator insert(ArrayIterator pos, IndexType n, const T* values);

  /*!
   * \brief Insert n copies of element into the array at the given position.
   *
   * \param [in] pos the position at which to begin the insertion.
   * \param [in] n the number of elements to insert.
   * \param [in] value the element value to insert.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by n.
   * \note This method is used to create space for elements in the middle of
   *  the array.
   *
   * \pre pos <= m_num_elements.
   */
  void insert(IndexType pos, IndexType n, const T& value);

  /*!
   * \brief Insert n copies of element into the array at the value before pos.
   *
   * \param [in] pos the ArrayIterator before which value will be inserted.
   * \param [in] n the number of elements to insert.
   * \param [in] value the element value to insert.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by n.
   * \note This method is used to create space for elements in the middle of
   *  the array.
   *
   * \pre pos <= end()
   *
   * \return ArrayIterator to first element inserted (pos if n == 0)
   */
  ArrayIterator insert(ArrayIterator pos, IndexType n, const T& value);

  /*!
   * \brief Erases an element from the Array 
   *
   * \param [in] pos the ArrayIterator to the element in the Array
   *
   * \return An ArrayIterator following the last element removed.
   */
  ArrayIterator erase(ArrayIterator pos);

  /*!
   * \brief Erases elements in the range [first, last) from the Array
   *
   * \param [in] first the ArrayIterator to the beginning of the range.
   * \param [in] last the ArrayIterator to end of range.
   *
   * \return An ArrayIterator following the last element removed. 
   */
  ArrayIterator erase(ArrayIterator first, ArrayIterator last);

  /*!
   * \brief Inserts new element into Array at the given position.
   *
   * \param [in] pos the position to insert element at.
   * \param [in] args the arguments to forward to constructor of the element.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by 1.
   */
  template <typename... Args>
  void emplace(IndexType pos, Args&&... args);

  /*!
   * \brief Inserts new element into Array before pos.
   *
   * \param [in] pos the ArrayIterator to insert element before.
   * \param [in] args the arguments to forward to constructor of the element.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by 1.
   *
   * \return An ArrayIterator to the emplaced element.
   */
  template <typename... Args>
  ArrayIterator emplace(ArrayIterator pos, Args&&... args);

  /*!
   * \brief Inserts new element at the end of the Array.
   *
   * \param [in] args the arguments to forward to constructor of the element.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   * \note The size increases by 1.
   */
  template <typename... Args>
  void emplace_back(Args&&... args);

  /// @}

  /// \name Array methods to query and set attributes
  /// @{

  /*!
   * \brief Return the number of elements allocated for the data array.
   */
  IndexType capacity() const { return m_capacity; }

  /*!
   * \brief Increase the capacity. Does nothing if the new capacity is less
   *  than the current capacity.
   *
   * \param [in] capacity the new number of elements to allocate.
   */
  void reserve(IndexType capacity)
  {
    if(capacity > m_capacity)
    {
      setCapacity(capacity);
    }
  }

  /*!
   * \brief Returns an ArrayIterator to the first element of the Array
   */
  ArrayIterator begin()
  {
    assert(m_data != nullptr);
    return ArrayIterator(0, this);
  }

  /*!
   * \brief Returns an ArrayIterator to the element following the last
   *  element of the Array.
   */
  ArrayIterator end()
  {
    assert(m_data != nullptr);
    return ArrayIterator(size(), this);
  }

  /*!
   * \brief Shrink the capacity to be equal to the size.
   */
  void shrink() { setCapacity(m_num_elements); }

  /*!
   * \brief Returns true iff the Array stores no elements.
   *
   * \note If the Array is empty the capacity can still be greater than zero.
   */
  bool empty() const { return m_num_elements == 0; }

  /*!
   * \brief Return the number of elements stored in the data array.
   */
  IndexType size() const { return m_num_elements; }

  /*!
   * \brief Update the number of elements stored in the data array.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   */
  void resize(IndexType new_num_elements);

  /*!
   * \brief Exchanges the contents of this Array with the other.
   *
   * \note The externality of the buffers will follow the swap
   */
  void swap(Array<T>& other);

  /*!
   * \brief Get the ratio by which the capacity increases upon dynamic resize.
   */
  double getResizeRatio() const { return m_resize_ratio; }

  /*!
   * \brief Set the ratio by which the capacity increases upon dynamic resize.
   *
   * \param [in] ratio the new resize ratio.
   */
  void setResizeRatio(double ratio) { m_resize_ratio = ratio; }

  /*!
   * \brief Get the ID for the umpire allocator
   */
  int getAllocatorID() const { return m_allocator_id; }

  /*!
   * \brief Return true iff the external buffer constructor was called.
   */
  bool isExternal() const { return m_is_external; }

  /*!
   * \brief Prints the Array
   *
   * \param os The output stream to write to
   * \return A reference to the modified ostream
   */
  std::ostream& print(std::ostream& os) const;

  /// @}

public:
  /// \name ArrayIterator to iterate through Array
  /// @{

  /**
   * \class   ArrayIterator
   * \brief   An iterator type for Array.
   *          Each increment operation advances the iterator to the next
   *          element in the Array.
   */
  class ArrayIterator : public IteratorBase<ArrayIterator, IndexType>
  {
  public:
    ArrayIterator(IndexType pos, Array* arr)
      : IteratorBase<ArrayIterator, IndexType>(pos)
      , m_arrayPtr(arr)
    { }

    /**
     * \brief Returns the current iterator value
     */
    T& operator*()
    {
      return (*m_arrayPtr)[IteratorBase<ArrayIterator, IndexType>::m_pos];
    }

  protected:
    /** Implementation of advance() as required by IteratorBase */
    void advance(IndexType n)
    {
      IteratorBase<ArrayIterator, IndexType>::m_pos += n;
    }

  protected:
    Array* const m_arrayPtr;
  };  // end of ArrayIterator class

  /// @}

protected:
  /*!
   * \brief Initialize an Array instance with the given number of elements.
   *
   * \param [in] num_elements the number of elements the Array holds.
   * \param [in] capacity the number of elements to allocate space for.
   *
   * \note If no capacity or capacity less than num_elements is specified
   *  then it will default to at least num_elements * DEFAULT_RESIZE_RATIO.
   * \note a capacity is specified for the number of elements to store in the
   *  array and does not correspond to the actual bytesize.
   *
   * \pre num_elements >= 0
   *
   * \post capacity() >= size()
   * \post size() == num_elements
   * \post getResizeRatio() == DEFAULT_RESIZE_RATIO
   */
  void initialize(IndexType num_elements, IndexType capacity);

  /*!
   * \brief Make space for a subsequent insertion into the array.
   *
   * \param [in] n the number of elements to insert.
   * \param [in] pos the position at which to begin the insertion.
   *
   * \return a pointer to the beginning of the insertion space.
   *
   * \note Reallocation is done if the new size will exceed the capacity.
   */
  T* reserveForInsert(IndexType n, IndexType pos);

  /*!
   * \brief Update the number of elements.
   *
   * \param [in] new_num_elements the new number of elements.
   */
  virtual void updateNumElements(IndexType new_num_elements);

  /*!
   * \brief Set the number of elements allocated for the data array.
   *
   * \param [in] capacity the new number of elements to allocate.
   */
  virtual void setCapacity(IndexType new_capacity);

  /*!
   * \brief Reallocates the data array when the size exceeds the capacity.
   *
   * \param [in] new_num_elements the number of elements which exceeds the
   *  current capacity.
   */
  virtual void dynamicRealloc(IndexType new_num_elements);

  /// \name Internal bounds-checking routines
  /// @{

  /*! \brief Test if idx is within bounds */
  inline bool inBounds(IndexType idx) const
  {
    return idx >= 0 && idx < m_num_elements;
  }
  /// @}

  T* m_data;
  IndexType m_num_elements;
  IndexType m_capacity;
  double m_resize_ratio;
  bool m_is_external;
  int m_allocator_id;
};

//------------------------------------------------------------------------------
//                            Array IMPLEMENTATION
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template <typename T>
Array<T>::Array()
  : m_data(nullptr)
  , m_num_elements(0)
  , m_capacity(0)
  , m_resize_ratio(DEFAULT_RESIZE_RATIO)
  , m_is_external(false)
  , m_allocator_id(axom::getDefaultAllocatorID())
{ }

//------------------------------------------------------------------------------
template <typename T>
Array<T>::Array(IndexType num_elements, IndexType capacity, int allocator_id)
  : m_data(nullptr)
  , m_num_elements(0)
  , m_capacity(0)
  , m_resize_ratio(DEFAULT_RESIZE_RATIO)
  , m_is_external(false)
  , m_allocator_id(allocator_id)
{
  initialize(num_elements, capacity);
}

//------------------------------------------------------------------------------
template <typename T>
Array<T>::Array(T* data, IndexType num_elements, IndexType capacity)
  : m_data(data)
  , m_num_elements(num_elements)
  , m_capacity(0)
  , m_resize_ratio(0.0)
  , m_is_external(true)
  , m_allocator_id(INVALID_ALLOCATOR_ID)
{
  m_capacity = (capacity < num_elements) ? num_elements : capacity;

  assert(m_num_elements >= 0);
  assert(m_num_elements <= m_capacity);
  assert(m_data != nullptr || m_capacity <= 0);
}

//------------------------------------------------------------------------------
template <typename T>
Array<T>::Array(const Array& other, int allocator_id)
  : m_data(nullptr)
  , m_num_elements(0)
  , m_capacity(0)
  , m_resize_ratio(DEFAULT_RESIZE_RATIO)
  , m_is_external(false)
  , m_allocator_id(allocator_id)
{
  initialize(other.size(), other.capacity());
  axom::copy(m_data, other.data(), m_num_elements * sizeof(T));
}

//------------------------------------------------------------------------------
template <typename T>
Array<T>::Array(Array&& other)
  : m_data(nullptr)
  , m_num_elements(0)
  , m_capacity(0)
  , m_resize_ratio(0.0)
  , m_is_external(false)
  , m_allocator_id(axom::getDefaultAllocatorID())
{
  m_data = other.m_data;
  m_num_elements = other.m_num_elements;
  m_capacity = other.m_capacity;
  m_resize_ratio = other.m_resize_ratio;
  m_is_external = other.m_is_external;
  m_allocator_id = other.m_allocator_id;

  other.m_data = nullptr;
  other.m_capacity = 0;
  other.m_resize_ratio = DEFAULT_RESIZE_RATIO;
  other.m_is_external = false;
  other.m_allocator_id = INVALID_ALLOCATOR_ID;
}

//------------------------------------------------------------------------------
template <typename T>
Array<T>::~Array()
{
  if(m_data != nullptr && !m_is_external)
  {
    axom::deallocate(m_data);
  }

  m_data = nullptr;
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::fill(const T& value)
{
  for(IndexType i = 0; i < m_num_elements; i++)
  {
    m_data[i] = value;
  }
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::push_back(const T& value)
{
  emplace_back(value);
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::push_back(T&& value)
{
  emplace_back(std::move(value));
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::set(const T* elements, IndexType n, IndexType pos)
{
  assert(elements != nullptr);
  assert(pos >= 0);
  assert(pos + n <= m_num_elements);

  for(IndexType i = 0; i < n; ++i)
  {
    m_data[pos + i] = elements[i];
  }
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::clear()
{
  // This most likely needs to be a call to erase() instead.
  for(IndexType i = 0; i < m_num_elements; ++i)
  {
    m_data[i].~T();
  }

  updateNumElements(0);
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::insert(IndexType pos, const T& value)
{
  reserveForInsert(1, pos);
  m_data[pos] = value;
}

//------------------------------------------------------------------------------
template <typename T>
inline typename Array<T>::ArrayIterator Array<T>::insert(Array<T>::ArrayIterator pos,
                                                         const T& value)
{
  assert(pos >= begin() && pos <= end());
  insert(pos - begin(), value);
  return pos;
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::insert(IndexType pos, IndexType n, const T* values)
{
  assert(values != nullptr);
  reserveForInsert(n, pos);
  for(IndexType i = 0; i < n; ++i)
  {
    m_data[pos + i] = values[i];
  }
}

//------------------------------------------------------------------------------
template <typename T>
inline typename Array<T>::ArrayIterator Array<T>::insert(Array<T>::ArrayIterator pos,
                                                         IndexType n,
                                                         const T* values)
{
  assert(pos >= begin() && pos <= end());
  insert(pos - begin(), n, values);
  return pos;
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::insert(IndexType pos, IndexType n, const T& value)
{
  reserveForInsert(n, pos);
  for(IndexType i = 0; i < n; ++i)
  {
    m_data[pos + i] = value;
  }
}

//------------------------------------------------------------------------------
template <typename T>
inline typename Array<T>::ArrayIterator Array<T>::insert(Array<T>::ArrayIterator pos,
                                                         IndexType n,
                                                         const T& value)
{
  assert(pos >= begin() && pos <= end());
  insert(pos - begin(), n, value);
  return pos;
}

//------------------------------------------------------------------------------
template <typename T>
inline typename Array<T>::ArrayIterator Array<T>::erase(Array<T>::ArrayIterator pos)
{
  assert(pos >= begin() && pos < end());
  int counter = 0;

  while(pos < end() - 1)
  {
    *pos = *(pos + 1);
    pos += 1;
    counter += 1;
  }
  (*pos).~T();

  updateNumElements(m_num_elements - 1);
  return pos - counter;
}

//------------------------------------------------------------------------------
template <typename T>
inline typename Array<T>::ArrayIterator Array<T>::erase(
  Array<T>::ArrayIterator first,
  Array<T>::ArrayIterator last)
{
  assert(first >= begin() && first < end());
  assert(last >= first && last <= end());

  // Empty range, return last
  if(first == last)
  {
    return last;
  }

  int count = 0;

  // Erase [first,last) elements
  while(first < last)
  {
    (*first).~T();
    first++;
    count++;
  }

  first -= count;
  int shifted = 0;

  // Shift [last, end) elements over
  while(last < end())
  {
    *first = *last;
    first++;
    last++;
    shifted++;
  }

  updateNumElements(m_num_elements - count);
  return first - shifted;
}

//------------------------------------------------------------------------------
template <typename T>
template <typename... Args>
inline void Array<T>::emplace(IndexType pos, Args&&... args)
{
  reserveForInsert(1, pos);
  m_data[pos] = std::move(T(std::forward<Args>(args)...));
}

//------------------------------------------------------------------------------
template <typename T>
template <typename... Args>
inline typename Array<T>::ArrayIterator Array<T>::emplace(Array<T>::ArrayIterator pos,
                                                          Args&&... args)
{
  assert(pos >= begin() && pos <= end());
  emplace(pos - begin(), args...);
  return pos;
}

//------------------------------------------------------------------------------
template <typename T>
template <typename... Args>
inline void Array<T>::emplace_back(Args&&... args)
{
  emplace(m_num_elements, args...);
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::resize(IndexType new_num_elements)
{
  assert(new_num_elements >= 0);

  if(new_num_elements > m_capacity)
  {
    dynamicRealloc(new_num_elements);
  }

  updateNumElements(new_num_elements);
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::swap(Array<T>& other)
{
  T* temp_data = m_data;
  IndexType temp_num_elements = m_num_elements;
  IndexType temp_capacity = m_capacity;
  double temp_resize_ratio = m_resize_ratio;
  bool temp_is_external = m_is_external;

  m_data = other.m_data;
  m_num_elements = other.m_num_elements;
  m_capacity = other.m_capacity;
  m_resize_ratio = other.m_resize_ratio;
  m_is_external = other.m_is_external;

  other.m_data = temp_data;
  other.m_num_elements = temp_num_elements;
  other.m_capacity = temp_capacity;
  other.m_resize_ratio = temp_resize_ratio;
  other.m_is_external = temp_is_external;
}

//------------------------------------------------------------------------------
template <typename T>
inline std::ostream& Array<T>::print(std::ostream& os) const
{
#if defined(AXOM_USE_UMPIRE)
  if(m_allocator_id ==
       axom::getUmpireResourceAllocatorID(umpire::resource::Device) ||
     m_allocator_id ==
       axom::getUmpireResourceAllocatorID(umpire::resource::Constant))
  {
    std::cerr << "Cannot print Array allocated on the GPU" << std::endl;
    utilities::processAbort();
  }
#endif

  os << "[ ";
  for(IndexType i = 0; i < m_num_elements; i++)
  {
    os << m_data[i] << " ";
  }
  os << " ]";

  return os;
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::initialize(IndexType num_elements, IndexType capacity)
{
  assert(num_elements >= 0);

  m_num_elements = num_elements;

  if(capacity < 0 || num_elements > capacity)
  {
    capacity = 0;
  }

  if(capacity == 0)
  {
    capacity = (num_elements > MIN_DEFAULT_CAPACITY) ? num_elements
                                                     : MIN_DEFAULT_CAPACITY;
  }
  setCapacity(capacity);

  // quick checks
  assert(m_data != nullptr);
  assert(m_num_elements >= 0);
  assert(m_capacity >= m_num_elements);
}

//------------------------------------------------------------------------------
template <typename T>
inline T* Array<T>::reserveForInsert(IndexType n, IndexType pos)
{
  assert(n >= 0);
  assert(pos >= 0);
  assert(pos <= m_num_elements);

  if(n == 0)
  {
    return m_data + pos;
  }

  IndexType new_size = m_num_elements + n;
  if(new_size > m_capacity)
  {
    dynamicRealloc(new_size);
  }

  T* const insert_pos = m_data + pos;
  T* cur_pos = m_data + m_num_elements - 1;
  for(; cur_pos >= insert_pos; --cur_pos)
  {
    *(cur_pos + n) = *cur_pos;
  }

  updateNumElements(new_size);
  return insert_pos;
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::updateNumElements(IndexType new_num_elements)
{
  assert(new_num_elements >= 0);
  assert(new_num_elements <= m_capacity);
  m_num_elements = new_num_elements;
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::setCapacity(IndexType new_capacity)
{
  assert(new_capacity >= 0);

  if(m_is_external && new_capacity <= m_capacity)
  {
    return;
  }

  if(m_is_external)
  {
    std::cerr << "Cannot reallocate an externally provided buffer.";
    utilities::processAbort();
  }

  if(new_capacity < m_num_elements)
  {
    updateNumElements(new_capacity);
  }

  m_data = axom::reallocate<T>(m_data, new_capacity, m_allocator_id);
  m_capacity = new_capacity;

  assert(m_data != nullptr || m_capacity <= 0);
}

//------------------------------------------------------------------------------
template <typename T>
inline void Array<T>::dynamicRealloc(IndexType new_num_elements)
{
  if(m_is_external)
  {
    std::cerr << "Cannot reallocate an externally provided buffer.";
    utilities::processAbort();
  }

  assert(m_resize_ratio >= 1.0);
  const IndexType new_capacity = new_num_elements * m_resize_ratio + 0.5;

  if(m_resize_ratio < 1.0)
  {
    std::cerr << "ERROR: resize ratio must be greater than 1.0.\n";
    std::cerr << "Set a valid resize ratio via calling setResizeRatio() with "
              << "an appropriate value.\n";

    utilities::processAbort();
  }

  m_data = axom::reallocate<T>(m_data, new_capacity, m_allocator_id);
  m_capacity = new_capacity;

  assert(m_data != nullptr || m_capacity <= 0);
}

//------------------------------------------------------------------------------
/// Free functions implementing Array's operator(s)
//------------------------------------------------------------------------------
template <typename T>
std::ostream& operator<<(std::ostream& os, const Array<T>& arr)
{
  arr.print(os);
  return os;
}

template <typename T>
bool operator==(const Array<T>& lhs, const Array<T>& rhs)
{
  if(lhs.getAllocatorID() != rhs.getAllocatorID())
  {
    return false;
  }

  if(lhs.size() != rhs.size())
  {
    return false;
  }

  for(int i = 0; i < lhs.size(); i++)
  {
    if(!(lhs[i] == rhs[i]))
    {
      return false;
    }
  }

  return true;
}

template <typename T>
bool operator!=(const Array<T>& lhs, const Array<T>& rhs)
{
  return !(lhs == rhs);
}

} /* namespace axom */

#endif /* AXOM_ARRAY_HPP_ */
