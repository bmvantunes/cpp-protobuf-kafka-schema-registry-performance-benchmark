#pragma once

#include <google/protobuf/descriptor.h>

namespace protobuf_c {

inline google::protobuf::FieldDescriptor::Label GetFieldLabel(
    const google::protobuf::FieldDescriptor* field) {
  if (field->is_repeated()) {
    return google::protobuf::FieldDescriptor::LABEL_REPEATED;
  }
  if (field->is_required()) {
    return google::protobuf::FieldDescriptor::LABEL_REQUIRED;
  }
  return google::protobuf::FieldDescriptor::LABEL_OPTIONAL;
}

}  // namespace protobuf_c
