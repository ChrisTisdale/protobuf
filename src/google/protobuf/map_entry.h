// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef GOOGLE_PROTOBUF_MAP_ENTRY_H__
#define GOOGLE_PROTOBUF_MAP_ENTRY_H__

#include <cstddef>
#include <cstdint>
#include <string>

#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/has_bits.h"
#include "google/protobuf/map_type_handler.h"
#include "google/protobuf/message.h"
#include "google/protobuf/message_lite.h"
#include "google/protobuf/parse_context.h"
#include "google/protobuf/unknown_field_set.h"
#include "google/protobuf/wire_format_lite.h"

// Must be included last.
#include "google/protobuf/port_def.inc"

#ifdef SWIG
#error "You cannot SWIG proto headers"
#endif

namespace google {
namespace protobuf {
class Arena;
namespace internal {
template <typename Derived, typename Key, typename Value,
          WireFormatLite::FieldType kKeyFieldType,
          WireFormatLite::FieldType kValueFieldType>
class MapField;
}
}  // namespace protobuf
}  // namespace google

namespace google {
namespace protobuf {
namespace internal {

// MapEntry is the returned google::protobuf::Message when calling AddMessage of
// google::protobuf::Reflection. In order to let it work with generated message
// reflection, its in-memory type is the same as generated message with the same
// fields. However, in order to decide the in-memory type of key/value, we need
// to know both their cpp type in generated api and proto type. In
// implementation, all in-memory types have related wire format functions to
// support except ArenaStringPtr. Therefore, we need to define another type with
// supporting wire format functions. Since this type is only used as return type
// of MapEntry accessors, it's named MapEntry accessor type.
//
// cpp type:               the type visible to users in public API.
// proto type:             WireFormatLite::FieldType of the field.
// in-memory type:         type of the data member used to stored this field.
// MapEntry accessor type: type used in MapEntry getters/mutators to access the
//                         field.
//
// cpp type | proto type  | in-memory type | MapEntry accessor type
// int32_t    TYPE_INT32    int32_t          int32_t
// int32_t    TYPE_FIXED32  int32_t          int32_t
// string     TYPE_STRING   ArenaStringPtr   string
// FooEnum    TYPE_ENUM     int              int
// FooMessage TYPE_MESSAGE  FooMessage*      FooMessage
//
// The in-memory types of primitive types can be inferred from its proto type,
// while we need to explicitly specify the cpp type if proto type is
// TYPE_MESSAGE to infer the in-memory type.
template <typename Derived, typename Key, typename Value,
          WireFormatLite::FieldType kKeyFieldType,
          WireFormatLite::FieldType kValueFieldType>
class MapEntry : public Message {
  // Provide utilities to parse/serialize key/value.  Provide utilities to
  // manipulate internal stored type.
  using KeyTypeHandler = MapTypeHandler<kKeyFieldType, Key>;
  using ValueTypeHandler = MapTypeHandler<kValueFieldType, Value>;

  // Define internal memory layout. Strings and messages are stored as
  // pointers, while other types are stored as values.
  using KeyOnMemory = typename KeyTypeHandler::TypeOnMemory;
  using ValueOnMemory = typename ValueTypeHandler::TypeOnMemory;

 public:
  constexpr MapEntry()
      : key_(KeyTypeHandler::Constinit()),
        value_(ValueTypeHandler::Constinit()) {}

  explicit MapEntry(Arena* arena)
      : Message(arena),
        key_(KeyTypeHandler::Constinit()),
        value_(ValueTypeHandler::Constinit()) {}

  MapEntry(const MapEntry&) = delete;
  MapEntry& operator=(const MapEntry&) = delete;

  ~MapEntry() override {
    if (GetArena() != nullptr) return;
    Message::_internal_metadata_.template Delete<UnknownFieldSet>();
    KeyTypeHandler::DeleteNoArena(key_);
    ValueTypeHandler::DeleteNoArena(value_);
  }

  using InternalArenaConstructable_ = void;
  using DestructorSkippable_ = void;

  Message* New(Arena* arena) const final {
    return Arena::CreateMessage<Derived>(arena);
  }

 protected:
  friend class google::protobuf::Arena;

  HasBits<1> _has_bits_{};
  mutable CachedSize _cached_size_{};

  KeyOnMemory key_;
  ValueOnMemory value_;
};

}  // namespace internal
}  // namespace protobuf
}  // namespace google

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_MAP_ENTRY_H__
