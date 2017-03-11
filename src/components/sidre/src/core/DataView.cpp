/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */

/*!
 ******************************************************************************
 *
 * \file DataView.cpp
 *
 * \brief   Implementation file for DataView class.
 *
 ******************************************************************************
 */

// Associated header file
#include "DataView.hpp"

// Other axom headers
#include "axom/CommonTypes.hpp"
#include "slic/slic.hpp"

// Sidre project headers
#include "DataBuffer.hpp"
#include "DataGroup.hpp"
#include "DataStore.hpp"

namespace axom
{
namespace sidre
{

/*
 *************************************************************************
 *
 * Return path of View's owning Group object.
 * Needs to be in the .cpp file because DataGroup methods aren't
 * accessible in .hpp file.
 *
 *************************************************************************
 */
std::string DataView::getPath() const
{
  return getOwningGroup()->getPathName();
}

/*
 *************************************************************************
 *
 * Return full path of View object, including its name.
 * Needs to be in the .cpp file because DataGroup methods aren't
 * accessible in .hpp file.
 *
 *************************************************************************
 */
std::string DataView::getPathName() const
{
  if (getPath().length() < 1)
  {
    return getName();
  }

  return getPath() + getOwningGroup()->getPathDelimiter() + getName();
}


/*
 *************************************************************************
 *
 * Allocate data for view, previously described.
 * The state may transition from EMPTY to BUFFER;
 * otherwise, the state must already be BUFFER.
 *
 *************************************************************************
 */
DataView * DataView::allocate()
{
  if ( isAllocateValid() )
  {
    if (m_state == EMPTY)
    {
      SLIC_ASSERT( m_data_buffer == AXOM_NULLPTR );
      m_data_buffer = m_owning_group->getDataStore()->createBuffer();
      m_data_buffer->attachToView(this);
      m_state = BUFFER;
    }

    TypeID type = static_cast<TypeID>(m_schema.dtype().id());
    SidreLength num_elems = m_schema.dtype().number_of_elements();
    m_data_buffer->allocate(type, num_elems);
    apply();
  }

  return this;
}

/*
 *************************************************************************
 *
 * Allocate data for view with type and number of elements.
 *
 *************************************************************************
 */
DataView * DataView::allocate( TypeID type, SidreLength num_elems)
{
  if ( type == NO_TYPE_ID || num_elems < 0 )
  {
    SLIC_CHECK(type != NO_TYPE_ID);
    SLIC_CHECK(num_elems >= 0);
    return this;
  }

  describe(type, num_elems);
  allocate();

  return this;
}

/*
 *************************************************************************
 *
 * Allocate data for view described by a Conduit data type object.
 *
 *************************************************************************
 */
DataView * DataView::allocate(const DataType& dtype)
{
  if ( dtype.is_empty() )
  {
    SLIC_CHECK_MSG( !dtype.is_empty(),
                    "Unable to allocate with empty data type.");
    return this;
  }

  describe(dtype);
  allocate();

  return this;
}

/*
 *************************************************************************
 *
 * Reallocate data for view to given number of elements.
 * This function requires that the view is already described.
 * The state may transition from EMPTY to BUFFER;
 * otherwise, the state must already be BUFFER.
 *
 *************************************************************************
 */
DataView * DataView::reallocate(SidreLength num_elems)
{
  TypeID vtype = static_cast<TypeID>(m_schema.dtype().id());

  if ( num_elems < 0 )
  {
    SLIC_CHECK_MSG(false, "num_elems must be >= 0");
  }
  else if ( isAllocateValid() )
  {
    if ( m_state == EMPTY )
    {
      allocate( vtype, num_elems);
    }
    else if ( m_data_buffer->isAllocated() )    //  XXX if ( isAllocated() )
    {
      describe(vtype, num_elems);
      m_data_buffer->reallocate(num_elems);
      apply();
    }
    else
    {
      allocate( vtype, num_elems );
    }
  }

  return this;
}

/*
 *************************************************************************
 *
 * Deallocate data for view.
 *
 *************************************************************************
 */
DataView * DataView::deallocate()
{
  if ( !isAllocateValid() )
  {
    SLIC_CHECK_MSG( isAllocateValid(),
                    "View " << this->getName() << "'s state " <<
                    getStateStringName(m_state) <<
                    " does not allow data deallocation");
    return this;
  }

  if ( hasBuffer() )
  {
    m_data_buffer->deallocate();
  }

  return this;
}

/*
 *************************************************************************
 *
 * Reallocate data for view using a Conduit data type object.
 *
 *************************************************************************
 */
DataView * DataView::reallocate(const DataType& dtype)
{
  // If we don't have an allocated buffer, we can just call allocate.
  if ( !isAllocated() )
  {
    return allocate(dtype);
  }

  TypeID type = static_cast<TypeID>(dtype.id());
  TypeID view_type = static_cast<TypeID>(m_schema.dtype().id());

  if (dtype.is_empty() || !isAllocateValid() || type != view_type)
  {
    SLIC_CHECK_MSG( !dtype.is_empty(),
                    "Unable to re-allocate with empty data type.");
    SLIC_CHECK_MSG( isAllocateValid(),
                    "View " << this->getName() << "'s state " <<
                    getStateStringName(m_state) <<
                    " does not allow data re-allocation");
    SLIC_CHECK_MSG( type == view_type,
                    "View " << this->getName() <<
                    " attempting to re-allocate with different type.");
    return this;
  }

  describe(dtype);
  SidreLength num_elems = dtype.number_of_elements();
  m_data_buffer->reallocate(num_elems);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Attach/detach buffer to view.
 *
 *************************************************************************
 */
DataView * DataView::attachBuffer(DataBuffer * buff)
{
  if ( m_state == BUFFER && buff == AXOM_NULLPTR)
  {
    DataBuffer * old_buffer = detachBuffer();
    if (old_buffer->getNumViews() == 0)
    {
      getOwningGroup()->getDataStore()->destroyBuffer(old_buffer);
    }
  }
  else if ( m_state == EMPTY && buff != AXOM_NULLPTR )
  {
    m_data_buffer = buff;
    buff->attachToView(this);
    m_state = BUFFER;
    SLIC_ASSERT( m_is_applied == false );

    // If view is described and the buffer is allocated, then call apply.
    if ( isDescribed() && m_data_buffer->isAllocated() )
    {
      apply();
    }
  }

  return this;
}

/*
 *************************************************************************
 *
 * Detach buffer from view.
 *
 *************************************************************************
 */
DataBuffer * DataView::detachBuffer()
{
  DataBuffer * buff = AXOM_NULLPTR;

  if ( m_state == BUFFER)
  {
    buff = m_data_buffer;
    m_data_buffer->detachFromView(this);
  }

  return buff;
}

/*
 *************************************************************************
 *
 * Apply data description to data.
 *
 *************************************************************************
 */
DataView * DataView::apply()
{
  if ( !isApplyValid() )
  {
    SLIC_CHECK_MSG( isApplyValid(),
                    "View state, '" << getStateStringName(m_state) <<
                    "', does not allow apply operation");
    return this;
  }

  void * data_pointer = AXOM_NULLPTR;

  if ( hasBuffer() )
  {
    data_pointer = m_data_buffer->getVoidPtr();
  }
  else
  {
    SLIC_ASSERT( m_state == EXTERNAL );
    data_pointer = m_external_ptr;
  }

  m_node.set_external(m_schema, data_pointer);
  m_is_applied = true;

  return this;
}

/*
 *************************************************************************
 *
 * Apply given # elems, offset, stride description to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(SidreLength num_elems,
                           SidreLength offset,
                           SidreLength stride)
{
  if ( num_elems < 0 )
  {
    SLIC_CHECK(num_elems >= 0);
    return this;
  }

  DataType dtype(m_schema.dtype());
  if ( dtype.is_empty() )
  {
    dtype = conduit::DataType::default_dtype(m_data_buffer->getTypeID());
  }

  const size_t bytes_per_elem = dtype.element_bytes();

  dtype.set_number_of_elements(num_elems);
  dtype.set_offset(offset * bytes_per_elem );
  dtype.set_stride(stride * bytes_per_elem );

  describe(dtype);

  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply given type, # elems, offset, stride description to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(TypeID type, SidreLength num_elems,
                           SidreLength offset,
                           SidreLength stride)
{
  if ( type == NO_TYPE_ID || num_elems < 0 )
  {
    SLIC_CHECK(type != NO_TYPE_ID);
    SLIC_CHECK(num_elems >= 0);
    return this;
  }

  DataType dtype = conduit::DataType::default_dtype(type);

  const size_t bytes_per_elem = dtype.element_bytes();

  dtype.set_number_of_elements(num_elems);
  dtype.set_offset(offset * bytes_per_elem);
  dtype.set_stride(stride * bytes_per_elem);

  describe(dtype);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply given type, number of dimensions and shape to data view.
 * If ndims is 1 then do not save in m_shape.
 *
 *************************************************************************
 */
DataView * DataView::apply(TypeID type, int ndims, SidreLength * shape)
{
  if ( type == NO_TYPE_ID || ndims < 1 || shape == AXOM_NULLPTR )
  {
    SLIC_CHECK(type != NO_TYPE_ID);
    SLIC_CHECK(ndims >= 1);
    SLIC_CHECK(shape != AXOM_NULLPTR);

    return this;
  }

  describe(type, ndims, shape);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Apply a data type description to data view.
 *
 *************************************************************************
 */
DataView * DataView::apply(const DataType &dtype)
{
  if ( dtype.is_empty() )
  {
    SLIC_CHECK_MSG( !dtype.is_empty(),
                    "Unable to apply description, data type is empty.");
    return this;
  }

  describe(dtype);
  apply();

  return this;
}

/*
 *************************************************************************
 *
 * Get void pointer to any data held by the view.
 *
 *************************************************************************
 */
void * DataView::getVoidPtr() const
{
  void * rv = AXOM_NULLPTR;

  switch (m_state)
  {
  case EMPTY:
    break;
  case EXTERNAL:
    if (isApplied())
    {
      rv = const_cast<void *>(m_node.data_ptr());
    }
    else
    {
      rv = m_external_ptr;  // Opaque
    }
    break;
  case BUFFER:
    if (isApplied())
    {
      rv = const_cast<void *>(m_node.data_ptr());
    }
    else
    {
      SLIC_CHECK_MSG(false, "View has no applied data.");
    }
    break;
  case STRING:
  case SCALAR:
    rv = const_cast<void *>(m_node.data_ptr());
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }

  return rv;
}

/*
 *************************************************************************
 *
 * Set data view to hold external data.
 *
 *************************************************************************
 */
DataView * DataView::setExternalDataPtr(void * external_ptr)
{
  if ( m_state == EMPTY || m_state == EXTERNAL )
  {
    if (external_ptr == AXOM_NULLPTR)
    {
      unapply();
      m_external_ptr = AXOM_NULLPTR;
      m_state = EMPTY;
    }
    else
    {
      m_external_ptr = external_ptr;
      m_state = EXTERNAL;

      if ( isDescribed() )
      {
        apply();
      }
    }
  }
  else
  {
    SLIC_CHECK_MSG( m_state == EMPTY || m_state == EXTERNAL,
                    "Calling setExternalDataPtr on a view with " <<
                    getStateStringName(m_state) << " data is not allowed.");
  }

  return this;
}

/*
 *************************************************************************
 *
 * Return true if view contains allocated data.  This could mean a buffer
 * with allocated data, or a scalar value, or a string.
 *
 * Note: Most of our isXXX functions are implemented in the header.
 * This one is in not, because we are only forward declaring the buffer
 * class in the view header.
 *************************************************************************
 */
bool DataView::isAllocated()
{
  bool rv = false;

  switch (m_state)
  {
  case EMPTY:
    break;
  case BUFFER:
    // false if buffer is not allocated or view is not described
    rv = isDescribed() && m_data_buffer->isAllocated();
    break;
  case EXTERNAL:
  case STRING:
  case SCALAR:
    rv = true;
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }

  return rv;
}

/*
 *************************************************************************
 *
 * Return number of dimensions and fill in shape information.
 *
 *************************************************************************
 */
int DataView::getShape(int ndims, SidreLength * shape) const
{
  if (static_cast<unsigned>(ndims) < m_shape.size())
  {
    return -1;
  }

  const int shapeSize = getNumDimensions();
  for(int i = 0 ; i < shapeSize ; ++i)
  {
    shape[i] = m_shape[i];
  }

  // Fill the rest of the array with zeros (when ndims > shapeSize)
  if(ndims > shapeSize)
  {
    for(int i = shapeSize ; i < ndims ; ++i)
    {
      shape[i] = 0;
    }
  }

  return m_shape.size();
}

/*
 *************************************************************************
 *
 * Return offset from description in terms of number of elements (0 if not described)
 *
 *************************************************************************
 */
SidreLength DataView::getOffset() const
{
  int offset = 0;

  if( isDescribed() )
  {
    offset = m_schema.dtype().offset();

    const int bytes_per_elem = getBytesPerElement();
    if(bytes_per_elem != 0)
    {
      SLIC_ERROR_IF(offset % bytes_per_elem != 0,
                    "Unsupported operation.  Sidre assumes that offsets are"
                    << " given as integral number of elements into the array."
                    << " In this case, the offset was " << offset
                    << " bytes and each element is " << bytes_per_elem
                    << " bytes. If you have a need for non-integral offsets,"
                    << " please contact the Sidre team");

      offset /= bytes_per_elem;
    }
  }

  return static_cast<SidreLength>(offset);
}

/*
 *************************************************************************
 *
 * Return stride from description in terms of number of elements (1 if not described)
 *
 *************************************************************************
 */
SidreLength DataView::getStride() const
{
  int stride = 1;

  if( isDescribed() )
  {
    stride = m_schema.dtype().stride();

    const int bytes_per_elem = getBytesPerElement();
    if(bytes_per_elem != 0)
    {
      SLIC_ERROR_IF(stride % bytes_per_elem != 0,
                    "Unsupported operation.  Sidre assumes that strides are"
                    << " given as integral number of elements into the array."
                    << " In this case, the stride was " << stride << " bytes"
                    << " and each element is " << bytes_per_elem << " bytes."
                    << " If you have a need for non-integral strides,"
                    << " please contact the Sidre team");

      stride /= bytes_per_elem;
    }
  }

  return static_cast<SidreLength>(stride);
}

/*
 *************************************************************************
 *
 * Test equivalence of two DataViews
 *
 *************************************************************************
 */
bool DataView::isEquivalentTo(const DataView * other) const
{
  //add isAllocated() if it can be declared const
  return (getName() == other->getName()) && (getTypeID() == other->getTypeID())
         && (isApplied() == other->isApplied())
         && (hasBuffer() == other->hasBuffer())
         && (getTotalBytes() == other->getTotalBytes());
}


/*
 *************************************************************************
 *
 * Print JSON description of data view to stdout.
 *
 *************************************************************************
 */
void DataView::print() const
{
  print(std::cout);
}

/*
 *************************************************************************
 *
 * Print JSON description of data view to an  ostream.
 *
 *************************************************************************
 */
void DataView::print(std::ostream& os) const
{
  Node n;
  copyToConduitNode(n);
  n.to_json_stream(os);
}

/*
 *************************************************************************
 *
 * Copy data view description to given Conduit node.
 *
 *************************************************************************
 */
void DataView::copyToConduitNode(Node &n) const
{
  n["name"] = m_name;
  n["schema"] = m_schema.to_json();
  n["value"]  = m_node.to_json();
  n["state"]  = getStateStringName(m_state);
  n["is_applied"] = m_is_applied;
}

/*
 *************************************************************************
 *
 * Copy data view native layout to given Conduit node.
 *
 *************************************************************************
 */
void DataView::createNativeLayout(Node &n) const
{
  // see ATK-726 - Handle undescribed and unallocated views in Sidre's createNativeLayout()
  // TODO: Need to handle cases where the view is not described
  // TODO: Need to handle cases where the view is not allocated
  // TODO: Need to handle cases where the view is not applied

  // Note: We are using conduit's pointer rather than the DataView pointer
  //    since the conduit pointer handles offsetting
  // Note: const_cast the pointer to satisfy conduit's interface
  void * data_ptr = const_cast<void *>(m_node.data_ptr());
  n.set_external( m_node.schema(), data_ptr);
}

/*
 *************************************************************************
 *
 * PRIVATE ctor for DataView not associated with any data.
 *
 *************************************************************************
 */
DataView::DataView( const std::string& name)
  :   m_name(name),
  m_owning_group(AXOM_NULLPTR),
  m_data_buffer(AXOM_NULLPTR),
  m_schema(),
  m_node(),
  m_shape(),
  m_external_ptr(AXOM_NULLPTR),
  m_state(EMPTY),
  m_is_applied(false)
{}

/*
 *************************************************************************
 *
 * PRIVATE dtor.
 *
 *************************************************************************
 */
DataView::~DataView()
{
  if (m_data_buffer != AXOM_NULLPTR)
  {
    m_data_buffer->detachFromView(this);
  }
}

/*
 *************************************************************************
 *
 * PRIVATE method to describe data view with type and number of elements.
 *         Caller has already checked arguments.
 *
 *************************************************************************
 */
void DataView::describe(TypeID type, SidreLength num_elems)
{
  DataType dtype = conduit::DataType::default_dtype(type);
  dtype.set_number_of_elements(num_elems);
  m_schema.set(dtype);
  describeShape();
  m_is_applied = false;
}

/*
 *************************************************************************
 *
 * PRIVATE method to describe data view with type, number of dimensions,
 *         and number of elements per dimension.
 *         Caller has already checked arguments.
 *
 *************************************************************************
 */
void DataView::describe(TypeID type, int ndims, SidreLength * shape)
{
  SidreLength num_elems = 0;
  if (ndims > 0)
  {
    num_elems = shape[0];
    for (int i=1 ; i < ndims ; i++)
    {
      num_elems *= shape[i];
    }
  }

  describe(type, num_elems);
  describeShape(ndims, shape);
}

/*
 *************************************************************************
 *
 * PRIVATE method to describe data view with a Conduit data type object.
 *         Caller has already checked arguments.
 *
 *************************************************************************
 */
void DataView::describe(const DataType& dtype)
{
  m_schema.set(dtype);
  describeShape();
  m_is_applied = false;
}

/*
 *************************************************************************
 *
 * PRIVATE method set shape to described length.
 * This is called after describe to set the shape.
 *
 *************************************************************************
 */
void DataView::describeShape()
{
  m_shape.clear();
  m_shape.push_back(m_schema.dtype().number_of_elements());
}

/*
 *************************************************************************
 *
 * PRIVATE method set shape from user input.
 *
 *************************************************************************
 */
void DataView::describeShape(int ndims, SidreLength * shape)
{
  m_shape.clear();
  for (int i=0 ; i < ndims ; i++)
  {
    m_shape.push_back(shape[i]);
  }
}

/*
 *************************************************************************
 *
 * PRIVATE method copy the contents of this into a undescribed EMPTY view.
 *
 *************************************************************************
 */
void DataView::copyView( DataView * copy ) const
{
  SLIC_ASSERT( copy->m_state == EMPTY && !copy->isDescribed());

  if (isDescribed())
  {
    copy->describe(m_schema.dtype());
  }

  switch (m_state)
  {
  case EMPTY:
    // Nothing more to do
    break;
  case STRING:
  case SCALAR:
    copy->m_node = m_node;
    copy->m_state = m_state;
    copy->m_is_applied = true;
    break;
  case EXTERNAL:
    copy->setExternalDataPtr(m_external_ptr);
    break;
  case BUFFER:
    copy->attachBuffer( m_data_buffer );
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }
}

/*
 *************************************************************************
 *
 * PRIVATE method returns true if view can allocate data; else false.
 *
 * This method does not need to emit the view state as part of it's
 * checking.  The caller functions are already printing out the view
 * state if this function returns false.
 *************************************************************************
 */
bool DataView::isAllocateValid() const
{
  bool rv = false;

  switch (m_state)
  {
  case EMPTY:
    rv = isDescribed();
    break;
  case STRING:
  case SCALAR:
  case EXTERNAL:
    SLIC_CHECK_MSG( false,
                    "Allocate is not valid for " <<
                    getStateStringName(m_state) << "view");
    break;
  case BUFFER:
    rv = isDescribed() && m_data_buffer->getNumViews() == 1;
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }

  return rv;
}

/*
 *************************************************************************
 *
 * PRIVATE method returns true if apply is a valid operation on view;
 * else false.
 *
 * For an EXTERNAL view, assume user provided m_external_ptr and
 * description are consistent. This includes m_external_ptr == NULL.
 *
 *************************************************************************
 */
bool DataView::isApplyValid() const
{
  bool rv = false;

  if ( !isDescribed() )
  {
    SLIC_CHECK_MSG(false,
                   "Apply is not valid, no description in view to apply");
    return rv;
  }

  switch (m_state)
  {
  case EMPTY:
  case STRING:
  case SCALAR:
    SLIC_CHECK_MSG( false,
                    "Apply is not valid for view " <<
                    getStateStringName(m_state) << " with scalar data type.");
    break;
  case EXTERNAL:
    SLIC_ASSERT ( m_external_ptr != AXOM_NULLPTR );
    rv = isDescribed();
    break;
  case BUFFER:
    rv = 0 < getTotalBytes() &&
         getTotalBytes() <= m_data_buffer->getTotalBytes();;
    SLIC_CHECK_MSG( 0 < getTotalBytes(),
                    "Apply is not valid on data with zero length." );
    SLIC_CHECK_MSG(
      getTotalBytes() <= m_data_buffer->getTotalBytes(),
      "Apply is not valid, view's datatype length exceeds bytes in buffer.");
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }

  return rv;
}

/*
 *************************************************************************
 *
 * PRIVATE method returns string name of given view state enum value.
 *
 *************************************************************************
 */
char const * DataView::getStateStringName(State state)
{
  char const * ret_string = NULL;

  switch ( state )
  {
  case EMPTY:
    ret_string = "EMPTY";
    break;
  case BUFFER:
    ret_string = "BUFFER";
    break;
  case EXTERNAL:
    ret_string = "EXTERNAL";
    break;
  case SCALAR:
    ret_string = "SCALAR";
    break;
  case STRING:
    ret_string = "STRING";
    break;
  default:
    ret_string = "UNKNOWN";
  }

  return ret_string;
}

/*
 *************************************************************************
 *
 * PRIVATE method returns state enum value when given string with a
 * state name.
 *
 *************************************************************************
 */
DataView::State DataView::getStateId(const std::string &name)
{
  State res = EMPTY;
  if(name == "EMPTY")
  {
    res = EMPTY;
  }
  else if(name == "BUFFER")
  {
    res = BUFFER;
  }
  else if(name == "EXTERNAL")
  {
    res = EXTERNAL;
  }
  else if(name == "SCALAR")
  {
    res = SCALAR;
  }
  else if(name == "STRING")
  {
    res = STRING;
  }
  else if(name == "UNKNOWN")
  {
    res = EMPTY;
  }

  return res;
}

/*
 *************************************************************************
 *
 * PRIVATE method to copy view data to given Conduit node using
 * given set of ids to maintain correct association of data buffers
 * to data views.
 *
 *************************************************************************
 */
void DataView::exportTo(conduit::Node& data_holder,
                        std::set<IndexType>& buffer_indices) const
{
  data_holder["state"] = getStateStringName(m_state);

  switch (m_state)
  {
  case EMPTY:
    if (isDescribed())
    {
      exportDescription(data_holder);
    }
    break;
  case BUFFER: {
    IndexType buffer_id = getBuffer()->getIndex();
    data_holder["buffer_id"] = buffer_id;
    if (isDescribed())
    {
      exportDescription(data_holder);
    }
    data_holder["is_applied"] =  static_cast<unsigned char>(m_is_applied);
    buffer_indices.insert(buffer_id);
    break;
  }
  case EXTERNAL:
    if (isDescribed())
    {
      exportDescription(data_holder);
    }
    else
    {
      // If there is no description, make it an EMPTY view
      data_holder["state"] = getStateStringName(EMPTY);
    }
    break;
  case SCALAR:
  case STRING:
    data_holder["value"] = getNode();
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }
}

/*
 *************************************************************************
 * TODO
 *
 *************************************************************************
 */
void DataView::importFrom(conduit::Node& data_holder,
                          const std::map<IndexType, IndexType>& buffer_id_map)
{
  m_state = getStateId(data_holder["state"].as_string());

  switch (m_state)
  {
  case EMPTY:
    importDescription(data_holder);
    break;
  case BUFFER: {
    // If view has a buffer, the easiest way to restore it is to use a series of
    // API calls.
    // Start from scratch
    m_state = EMPTY;

    IndexType old_buffer_id = data_holder["buffer_id"].as_int();
    bool is_applied = data_holder["is_applied"].as_unsigned_char();

    SLIC_ASSERT_MSG( buffer_id_map.find(old_buffer_id) !=
                     buffer_id_map.end(),
                     "Buffer id map is old-new id entry for buffer " <<
                     old_buffer_id );

    DataBuffer * buffer = m_owning_group->getDataStore()->
                          getBuffer( buffer_id_map.at(old_buffer_id) );

    importDescription(data_holder);
    attachBuffer( buffer );
    if ( is_applied )
    {
      apply();
    }
    break;
  }
  case EXTERNAL:
    importDescription(data_holder);
    break;
  case SCALAR:
  case STRING:
    m_node = data_holder["value"];
    m_schema.set(m_node.schema());
    m_is_applied = true;
    break;
  default:
    SLIC_ASSERT_MSG(false, "Unexpected value for m_state");
  }
}

/*
 *************************************************************************
 *
 * PRIVATE method to save view's description to a conduit tree.
 * The shape information is only written if there is more than
 * one dimension.
 *
 *************************************************************************
 */
void DataView::exportDescription(conduit::Node& data_holder) const
{
  data_holder["schema"] = m_schema.to_json();
  if (getNumDimensions() > 1)
  {
    data_holder["shape"].set(m_shape);
  }
}

/*
 *************************************************************************
 *
 * PRIVATE method to restore a view's description from a conduit tree.
 *
 *************************************************************************
 */
void DataView::importDescription(conduit::Node& data_holder)
{
  if (data_holder.has_path("schema"))
  {
    conduit::Schema schema( data_holder["schema"].as_string() );
    describe( schema.dtype() );
    if (data_holder.has_path("shape"))
    {
      Node & n = data_holder["shape"];
      SidreLength * shape = n.as_long_ptr();
      int ndims = n.dtype().number_of_elements();
      describeShape(ndims, shape);
    }
  }
}

/*
 *************************************************************************
 *
 * Rename this View with a new string name.
 *
 *************************************************************************
 */
bool DataView::rename(const std::string& new_name)
{
  bool do_rename = true;
  if (new_name != m_name) {

    DataGroup * parent = getOwningGroup();
    SLIC_CHECK(parent != AXOM_NULLPTR);

    if (new_name.empty()) {
      SLIC_WARNING("Cannot rename View " << m_name << " to an empty " <<
                   "string.");
      do_rename = false;
    } else if (new_name.find(parent->getPathDelimiter()) != std::string::npos) {
      SLIC_WARNING("Cannot rename View "<< m_name << " to path name " <<
                   new_name << ". Only strings without path delimiters can " <<
                   "be passed into the rename method.");
      do_rename = false;
    } else if (parent->hasGroup(new_name) || parent->hasView(new_name)) {
      SLIC_WARNING("Parent group " << parent->getName() <<
                   " already has a child object named " << new_name <<
                   ". View " << m_name << " will not be renamed.");
      do_rename = false;
    } else {

      DataView * detached_view = parent->detachView(m_name);
      SLIC_CHECK(detached_view == this);

      m_name = new_name;

      DataView * attached_view = parent->attachView(detached_view);
      AXOM_DEBUG_VAR(attached_view);
      SLIC_CHECK(attached_view == this);
    }
  }

  return do_rename;
}

} /* end namespace sidre */
} /* end namespace axom */
