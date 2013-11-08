// Copyright 2013 Jan de Cuveland <cmail@cuveland.de>
#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <memory>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <fstream>

//! \file
//! This file describes the timeslice-based interface to FLES.

namespace fles
{

#pragma pack(1)

//! Microslice descriptor struct.
struct MicrosliceDescriptor
{
    uint8_t hdr_id;  ///< Header format identifier (0xDD)
    uint8_t hdr_ver; ///< Header format version (0x01)
    uint16_t eq_id;  ///< Equipment identifier
    uint16_t flags;  ///< Status and error flags
    uint8_t sys_id;  ///< Subsystem identifier
    uint8_t sys_ver; ///< Subsystem format version
    uint64_t idx;    ///< Microslice index
    uint32_t crc;    ///< CRC32 checksum
    uint32_t size;   ///< Content size (bytes)
    uint64_t offset; ///< Offset in event buffer (bytes)
};

//! Timeslice component descriptor struct.
struct TimesliceComponentDescriptor
{
    uint64_t ts_num; ///< Timeslice index.
    uint64_t offset; ///< Start offset (in bytes) of corresponding data.
    uint64_t size;   ///< Size (in bytes) of corresponding data.
    uint64_t num_microslices; ///< Number of microslices.
};

//! Timeslice work item struct.
struct TimesliceWorkItem
{
    uint64_t ts_pos; ///< Start offset (in items) of this timeslice
    uint32_t num_core_microslices; ///< Number of core microslices
    uint32_t num_components;       ///< Number of components (contributing input
    /// channels)
    uint32_t data_buffer_size_exp; ///< Exp. size (in bytes) of each data buffer
    uint32_t desc_buffer_size_exp; ///< Exp. size (in bytes) of each descriptor
    /// buffer

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /* version */)
    {
        ar& ts_pos;
        ar& num_core_microslices;
        ar& num_components;
        ar& data_buffer_size_exp;
        ar& desc_buffer_size_exp;
    }
};

//! Timeslice completion struct.
struct TimesliceCompletion
{
    uint64_t ts_pos; ///< Start offset (in items) of this timeslice
};

#pragma pack()

//! The TimesliceView class provides access to the data of a single timeslice in
// memory.
class TimesliceView
{
public:
    TimesliceView(const TimesliceView&) = delete;
    void operator=(const TimesliceView&) = delete;

    ~TimesliceView();

    /// Retrieve the timeslice index.
    uint64_t index() const
    {
        return desc(0).ts_num;
    }

    /// Retrieve the number of core microslices.
    uint64_t num_core_microslices() const
    {
        return _work_item.num_core_microslices;
    }

    /// Retrieve the total number of microslices.
    uint64_t num_microslices(uint64_t component) const
    {
        return desc(component).num_microslices;
    }

    /// Retrieve the number of components (contributing input channels).
    uint64_t num_components() const
    {
        return _work_item.num_components;
    }

    /// Retrieve a pointer to the data content of a given microslice
    const uint8_t* content(uint64_t component, uint64_t microslice) const
    {
        return &data(component, desc(component).offset)
               + desc(component).num_microslices * sizeof(MicrosliceDescriptor)
               + descriptor(component, microslice).offset
               - descriptor(component, 0).offset;
    }

    /// Retrieve the descriptor of a given microslice
    const MicrosliceDescriptor& descriptor(uint64_t component,
                                           uint64_t microslice) const
    {
        return (&reinterpret_cast<const MicrosliceDescriptor&>(
                     data(component, desc(component).offset)))[microslice];
    }

private:
    friend class TimesliceReceiver;
    friend class StorableTimeslice;

    TimesliceView(TimesliceWorkItem work_item,
                  uint8_t* data,
                  TimesliceComponentDescriptor* desc,
                  std::shared_ptr
                  <boost::interprocess::message_queue> completions_mq);

    const uint8_t& data(uint64_t component, uint64_t offset) const
    {
        return (_data + (component << _work_item.data_buffer_size_exp))
            [offset & _data_offset_mask];
    }

    const TimesliceComponentDescriptor& desc(uint64_t component) const
    {
        return (_desc + (component << _work_item.desc_buffer_size_exp))
            [_descriptor_offset];
    }

    TimesliceWorkItem _work_item;
    TimesliceCompletion _completion = TimesliceCompletion();

    const uint8_t* const _data;
    const TimesliceComponentDescriptor* const _desc;

    std::shared_ptr<boost::interprocess::message_queue> _completions_mq;

    uint64_t _descriptor_offset;
    uint64_t _data_offset_mask;
};

//! The StorableTimeslice class contains the data of a single timeslice.
class StorableTimeslice
{
public:
    StorableTimeslice() {}; // FIXME!

    StorableTimeslice(const TimesliceView& ts);

    /// Retrieve the timeslice index.
    uint64_t index() const
    {
        return _index;
    }

    /// Retrieve the number of core microslices.
    uint64_t num_core_microslices() const
    {
        return _work_item.num_core_microslices;
    }

    /// Retrieve the total number of microslices.
    uint64_t num_microslices() const
    {
        return 0; //_work_item.num_core_microslices // TODO FIXME!
    }

    /// Retrieve the number of components (contributing input channels).
    uint64_t num_components() const
    {
        return _work_item.num_components;
    }

    /// Retrieve a pointer to the data content of a given microslice
    const uint8_t* content(uint64_t component, uint64_t microslice) const
    {
        return
            &_data[component][num_microslices() * sizeof(MicrosliceDescriptor)
                              + descriptor(component, microslice).offset
                              - descriptor(component, 0).offset];
    }

    /// Retrieve the descriptor of a given microslice
    const MicrosliceDescriptor& descriptor(uint64_t component,
                                           uint64_t microslice) const
    {
        return (&reinterpret_cast
                <const MicrosliceDescriptor&>(_data[component][0]))[microslice];
    }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /* version */)
    {
        ar& _work_item;
        ar& _data;
        ar& _index;
    }

    TimesliceWorkItem _work_item;
    std::vector<std::vector<uint8_t> > _data;
    uint64_t _index;
};

//! The TimesliceArchiveDescriptor precedes a stream of serialized
// StorableTimeslice objects.
class TimesliceArchiveDescriptor
{
public:
    TimesliceArchiveDescriptor()
        : _time_created(std::chrono::system_clock::to_time_t(
              std::chrono::system_clock::now()))
    {
    }

    /// Retrieve the time of creation of the archive.
    std::time_t time_created() const;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int /* version */)
    {
        ar& _time_created;
    }

    std::time_t _time_created;
};

//! The TimesliceOutputArchive...
class TimesliceOutputArchive
{
public:
    TimesliceOutputArchive(const std::string& filename)
        : _ofstream(filename, std::ios::binary), _oarchive(_ofstream)
    {
        _oarchive << _descriptor;
    }

    TimesliceOutputArchive(const TimesliceOutputArchive&) = delete;
    void operator=(const TimesliceOutputArchive&) = delete;

    /// Store a timeslice.
    void write(const StorableTimeslice& timeslice)
    {
        _oarchive << timeslice;
    }

private:
    std::ofstream _ofstream;
    boost::archive::binary_oarchive _oarchive;
    TimesliceArchiveDescriptor _descriptor;
};

//! The TimesliceInputArchive...
class TimesliceInputArchive
{
public:
    TimesliceInputArchive(const std::string& filename)
        : _ifstream(filename, std::ios::binary), _iarchive(_ifstream)
    {
        _iarchive >> _descriptor;
    }

    TimesliceInputArchive(const TimesliceInputArchive&) = delete;
    void operator=(const TimesliceInputArchive&) = delete;

    /// Read the next timeslice.
    std::unique_ptr<StorableTimeslice> read()
    {
        StorableTimeslice s;
        try
        {
            _iarchive >> s;
        }
        catch (boost::archive::archive_exception e)
        {
            return nullptr;
        }
        std::unique_ptr<StorableTimeslice> sts = std::unique_ptr
            <StorableTimeslice>(new StorableTimeslice(s));
        return sts;
    }

private:
    std::ifstream _ifstream;
    boost::archive::binary_iarchive _iarchive;
    TimesliceArchiveDescriptor _descriptor;
};

//! The TimesliceReveicer class implements the IPC mechanisms to receive a
// timeslice.
class TimesliceReceiver
{
public:
    /// The TimesliceReceiver constructor.
    TimesliceReceiver(const std::string shared_memory_identifier);

    TimesliceReceiver(const TimesliceReceiver&) = delete;
    void operator=(const TimesliceReceiver&) = delete;

    /// Receive the next timeslice, block if not yet available.
    std::unique_ptr<const TimesliceView> receive();

private:
    const std::string _shared_memory_identifier;

    std::unique_ptr<boost::interprocess::shared_memory_object> _data_shm;
    std::unique_ptr<boost::interprocess::shared_memory_object> _desc_shm;

    std::unique_ptr<boost::interprocess::mapped_region> _data_region;
    std::unique_ptr<boost::interprocess::mapped_region> _desc_region;

    std::unique_ptr<boost::interprocess::message_queue> _work_items_mq;
    std::shared_ptr<boost::interprocess::message_queue> _completions_mq;

    bool _eof = false;
};

} // namespace fles {
