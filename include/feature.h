//
//  feature.h
//  bilab
//
//  Created by 曹 巍 on 2013/12/05.
//  Copyright (c) 2013年 曹 巍. All rights reserved.
//

#ifndef bilab_feature_h
#define bilab_feature_h

#include "common.h"

// Note:
// Binding functions take a const v8::FunctionCallbackInfo<T>& args
//   rather than v8::Arguments& args
// Binding functions return void rather than v8::Handle<v8::Value>/
//   The return value is returned with args.GetReturnValue().Set() family
//   of functions

//typedef std::pair<std::string,double> aaProp;
typedef std::map<std::string,double> aaIndex;

typedef struct _Property
{
  std::map<std::string, aaIndex> prop;
  //std::map<std::string, aaIndex>::iterator it = prop.begin();
  _Property()
  {};
  bool add(const std::string& name, const aaIndex& index)
  {
  return (prop.insert(std::pair<std::string,aaIndex>(name,index)).second == false)?
  //std::cout<<"Failed to add the aaindex " << name << std::endl;
  false : true;
  };
} Property;


class AAproperties : public node::ObjectWrap {
  
private:
  //version
  std::string version;
  
  //variables
  Property aaProperties;
  std::vector<std::string> PropNameList;
  
  // V8 related
  v8::Persistent<v8::Context> context_;    
  v8::Isolate* isolate_;

  // Private functions
  v8::Isolate* GetIsolate() { return isolate_; }

public:
  
  //Constructor
  AAproperties(v8::Isolate* isolate);
  
  //Deconstructor
  ~AAproperties();
  
  //version info
  std::string info() { return version; };
  
  static void Log(const char* event);
  void addProperties(std::string const& name,aaIndex index);
  //double getProperty(std::string inxName,std::string resName);
  std::vector<double> get(const std::string& inxName,
                          const std::string& seq);
  // export to v8
  static v8::Persistent<v8::Function> constructor;
  //static v8::Persistent<v8::FunctionTemplate> persistent_function_template;
  
  // Called by Nodejs module
  static void Init(v8::Handle<v8::Object> target);
  
  // v8 binding in old fashion
  //static v8::Handle<v8::Value> New(v8::internal::Arguments& args);
  //static v8::Handle<v8::Value> NewInstance(v8::internal::Arguments& args);
  //static v8::Handle<v8::Value> getProperty (v8::internal::Arguments& args);
  
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
  
  // Callback without return value
  //static v8::Handle<v8::Array> getProperty(v8::Local<v8::String> inxNam
  //    v8::Local<v8::String> seq, const v8::AccessorInfo& info);
  //static v8::Handle<v8::Value> getProperty (const v8::FunctionCallbackInfo<v8::Value>& args);
  static void getProperty (const v8::FunctionCallbackInfo<v8::Value>& args);
};

extern "C" {
  static void init(v8::Handle<v8::Object> target){
    AAproperties::Init(target);
  }
  // @see node.h: without semicolon
  // module_name needs to match the filename of the final binary
  // (minus the .node suffix)
  NODE_MODULE(aaproperties,init)
}


#endif
