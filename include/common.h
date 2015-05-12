//
//  common.h
//  bilab
//
//  Created by 曹 巍 on 2013/12/05.
//  Copyright (c) 2013年 曹 巍. All rights reserved.
//

#ifndef bilab_common_h
#define bilab_common_h

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sstream>
#include <iostream>
#include <cmath>
#include <climits> // or #include <limits.h>
#include <limits>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include <stdarg.h> // svm.cpp need va_start, va_end, va_list,va_arg

// c++11
#include <cstddef>

// V8 engine
#include "v8.h"

// Nodejs
#include "node.h"
#include "node_object_wrap.h"

// Converter HP
inline v8::Handle<v8::Value> ThrowError(const char* msg) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return isolate->ThrowException(v8::String::NewFromUtf8(isolate, msg));
}

inline v8::Handle<v8::Value> ThrowObjectDisposed() {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return isolate->ThrowException(
    v8::Exception::ReferenceError(
        v8::String::NewFromUtf8(isolate, "Object has been disposed"))
    );
}

inline v8::Handle<v8::Value> ThrowArgCount() {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return isolate->ThrowException(
    v8::Exception::TypeError(
      v8::String::NewFromUtf8(isolate, "Not enough arguments"))
    );
}

inline v8::Handle<v8::Value> ThrowTypeError() {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return isolate->ThrowException(
    v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Type error"))
    );
}

inline v8::Handle<v8::Value> ThrowRangeError(const char* msg) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return isolate->ThrowException(
    v8::Exception::RangeError(
      v8::String::NewFromUtf8(isolate, msg)));
}

template<typename T>
v8::Handle<v8::Value> ToV8(T value);

// Specialization
template<>
inline v8::Handle<v8::Value> ToV8<bool>(bool value){
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
	return value ? v8::True(isolate) : v8::False(isolate);
}

template<>
inline v8::Handle<v8::Value> ToV8<int32_t>(int32_t value){
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::Integer::New(isolate, value);
}

template<>
inline v8::Handle<v8::Value> ToV8<uint32_t>(uint32_t value) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::Integer::NewFromUnsigned(isolate, value);
}

template<>
inline v8::Handle<v8::Value> ToV8<double>(double value) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::Number::New(isolate, value);
}

template<>
inline v8::Handle<v8::Value> ToV8<const char*>(const char* value) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(isolate, value);
}

template<>
inline v8::Handle<v8::Value> ToV8<std::string>(std::string value) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //return v8::String::NewFromUtf8(isolate, value.data(), static_cast<int>(value.size()));
  return v8::String::NewFromUtf8(isolate, value.data());
}

template<typename T>
v8::Handle<v8::Array> ArrayToV8(T* values, uint32_t length) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Array> array = v8::Array::New(isolate, length);
  for (uint32_t i = 0; i < length; i++)
    array->Set(ToV8(i), ToV8<T>(values[i]));
  return array;
}


template<typename T>
T FromV8(v8::Handle<v8::Value> value, bool* ok);

template<>
inline std::string FromV8<std::string>(v8::Handle<v8::Value> value, bool* ok){
  *ok = true;
  v8::String::Utf8Value utf8_value(value);
  if (!*utf8_value) {
    *ok = false;
    return std::string();
  }
  return std::string(*utf8_value, utf8_value.length());
}

template<>
inline bool FromV8<bool>(v8::Handle<v8::Value> value, bool* ok) {
  *ok = true;
  return value->BooleanValue();
}

template<>
inline double FromV8<double>(v8::Handle<v8::Value> value, bool* ok){
  *ok = true;

  v8::Local<v8::Number> num_value = value->ToNumber();
  if (num_value.IsEmpty()) {
    *ok = false;
    return 0;
  }
  return num_value->Value();
}

template<>
inline float FromV8<float>(v8::Handle<v8::Value> value, bool* ok) {
  return static_cast<float>(FromV8<double>(value, ok));
}

template<>
inline int32_t FromV8<int32_t>(v8::Handle<v8::Value> value, bool* ok){
	*ok = true;

  v8::Local<v8::Int32> int_value = value->ToInt32();
  if (int_value.IsEmpty()) {
    *ok = false;
    return 0;
  }
  return int_value->Value();
}

template<>
inline uint32_t FromV8<uint32_t>(v8::Handle<v8::Value> value, bool* ok){
  *ok = true;

  v8::Local<v8::Uint32> uint_value = value->ToUint32();
  if (uint_value.IsEmpty()) {
    *ok = false;
    return 0;
  }
  return uint_value->Value();
}

template<class T>
T* NativeFromV8(v8::Handle<v8::Value> value, bool* ok) {
  *ok = true;
  // Not an error if undefined/null
  if (value->IsUndefined() || value->IsNull())
    return NULL;

  if (!T::HasInstance(value)) {
    *ok = false;
    return NULL;
  }
  v8::Handle<v8::Object> object = value->ToObject();
  T* native = T::FromV8Object(object);
  if (!native) {
    *ok = false;
    ThrowObjectDisposed();
    return NULL;
  }
  return native;
}

template<class T>
std::vector<T> ArrayFromV8(v8::Handle<v8::Value> value, bool* ok) {
  *ok = true;

  if (value->IsUndefined() || value->IsNull())
    return std::vector<T>();
  if (!value->IsArray()){
    *ok = false;
    return std::vector<T>();
  }
  v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(value);
  uint32_t length = array->Length();
  std::vector<T> vector(length);
  for (uint32_t i = 0; i < length; i++) {
    v8::Local<v8::Value> entry = array->Get(i);
    vector[i] = FromV8<T>(entry, ok);
    if (!*ok)
      return std::vector<T>();
  }
  return vector;
}

template <class T>
std::string _toString(T t, std::ios_base & (*f)(std::ios_base&))
{
  std::ostringstream oss;
  oss << f << t;
  return oss.str();
}


#endif
