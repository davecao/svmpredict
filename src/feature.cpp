//
//  feature.cpp
//  bilab
//
//  Created by 曹 巍 on 2013/12/05.
//  Copyright (c) 2013年 曹 巍. All rights reserved.
//

#include "node.h"
#include "feature.h"

//Kyte J., Doolittle R.F. J. Mol. Biol. 157:105-132(1982).
aaIndex KYTJ820101 = {
  {"A", 1.800}, // A  alanine
  {"B", 0.000}, // B  aspartic acid or asparagine
  {"C", 2.500}, // C  cysteine
  {"D",-3.500}, // D  aspartic acid
  {"E",-3.500}, // E  glutamic acid
  {"F", 2.800}, // F  phenylalanine
  {"G",-0.400}, // G  glycine
  {"H",-3.200}, // H  histidine
  {"I", 4.500}, // I  isoleucine
  {"J", 0.000}, // J  not used
  {"K",-3.900}, // K  lysine
  {"L", 3.800}, // L  leucine
  {"M", 1.900}, // M  methionine
  {"N",-3.500}, // N  asparagine
  {"O", 0.000}, // O  not used
  {"P",-1.600}, // P  proline
  {"Q",-3.500}, // Q  glutamine
  {"R",-4.500}, // R  arginine
  {"S",-0.800}, // S  serine
  {"T",-0.700}, // T  threonine
  {"U", 0.000}, // U  not used
  {"V", 4.200}, // V  valine
  {"W",-0.900}, // W  tryptophan
  {"X", 0.000}, // X  any amino acid
  {"Y",-1.300}, // Y  tyrosine
  {"Z", 0.000}  // Z  glutamic acid or glutamine
};
//Bulkness: Zimmerman J.M., Eliezer N., Simha R. J. Theor. Biol. 21:170-201(1968).
aaIndex ZIMJ680102 = {
  {"A",11.500}, // A  alanine
  {"B", 0.000}, // B  aspartic acid or asparagine
  {"C",13.460}, // C  cysteine
  {"D",11.680}, // D  aspartic acid
  {"E",13.570}, // E  glutamic acid
  {"F",19.800}, // F  phenylalanine
  {"G", 3.400}, // G  glycine
  {"H",13.690}, // H  histidine
  {"I",21.400}, // I  isoleucine
  {"J", 0.000}, // J  not used
  {"K",15.710}, // K  lysine
  {"L",21.400}, // L  leucine
  {"M",16.250}, // M  methionine
  {"N",12.820}, // N  asparagine
  {"O", 0.000}, // O  not used
  {"P",17.430}, // P  proline
  {"Q",14.450}, // Q  glutamine
  {"R",14.280}, // R  arginine
  {"S", 9.470}, // S  serine
  {"T",15.770}, // T  threonine
  {"U", 0.000}, //U  not used
  {"V",21.570}, // V  valine
  {"W",21.670}, // W  tryptophan
  {"X", 0.000}, // X  any amino acid
  {"Y",18.030}, // Y  tyrosine
  {"Z", 0.000}  // Z  glutamic acid or glutamine
};
//Bulkness: Zimmerman J.M., Eliezer N., Simha R. J. Theor. Biol. 21:170-201(1968).
aaIndex DAWD720101 = {
  {"A", 2.500}, // A  alanine
  {"B", 0.000}, // B  aspartic acid or asparagine
  {"C", 3.000}, // C  cysteine
  {"D", 2.500}, // D  aspartic acid
  {"E", 5.000}, // E  glutamic acid
  {"F", 6.500}, // F  phenylalanine
  {"G", 0.500}, // G  glycine
  {"H", 6.000}, // H  histidine
  {"I", 5.500}, // I  isoleucine
  {"J", 0.000}, // J  not used
  {"K", 7.000}, // K  lysine
  {"L", 5.500}, // L  leucine
  {"M", 6.000}, // M  methionine
  {"N", 5.000}, // N  asparagine
  {"O", 0.000}, // O  not used
  {"P", 5.500}, // P  proline
  {"Q", 6.000}, // Q  glutamine
  {"R", 7.500}, // R  arginine
  {"S", 3.000}, // S  serine
  {"T", 5.000}, // T  threonine
  {"U", 0.000}, //U  not used
  {"V", 5.000}, // V  valine
  {"W", 7.000}, // W  tryptophan
  {"X", 0.000}, // X  any amino acid
  {"Y", 7.000}, // Y  tyrosine
  {"Z", 0.000}  // Z  glutamic acid or glutamine
};
/* Old fashion:
static v8::Handle<v8::Value> LogCallBack(v8::internal::Arguments& args) {
  if(args.length() < 1) return v8::Undefined();

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  // Object -> Value
  v8::Handle<v8::Value> arg = v8::Utils::ToLocal(
    v8::internal::Handle<v8::internal::Object>(args[0]));

  v8::String::Utf8Value value(arg);
  AAproperties::Log(*value);
  return v8::Undefined();
};
*/

static void LogCallBack(
  const v8::FunctionCallbackInfo<v8::Value>& args){

  if (args.Length() < 1) {
    return;
  }
  v8::EscapableHandleScope scop(args.GetIsolate());
  v8::Handle<v8::Value> arg = args[0];
  v8::String::Utf8Value value(arg);
  AAproperties::Log(*value);
};

v8::Handle<v8::Value> RunCallBack(
      const v8::FunctionCallbackInfo<v8::Value>& args) {
  ////////////////////
  // NOT in use now.
  ////////////////////
  // In old fashion:
  // v8::HandleScope scope;
  //v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //v8::HandleScope scope(isolate);
  //v8::HandleScope scope(args.GetIsolate());
  v8::Isolate* isolate = args.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  bool isJSfunc = args[0]->IsFunction();

  if (! isJSfunc ) {
    isolate->ThrowException(
      v8::Exception::TypeError(
        v8::String::NewFromUtf8(isolate, "The first argument is not JS function.")));
  }

  v8::Local<v8::Function> cb = args[0].As<v8::Function>();
  //v8::Local<v8::Function>::Cast(args[0]);

  const unsigned argc = 1;

  v8::Local<v8::Value> argv[argc] = {
    //v8::Local<v8::Value>::New(args.GetIsolate(),v8::String::New("Hello World"))
    v8::Local<v8::Value>::New(isolate,v8::String::NewFromUtf8(isolate, "Hello World"))
  };
  // execute
  //v8::Local<v8::Context> ctx = v8::
  cb->Call(isolate->GetCurrentContext()->Global(),argc,argv);
  //"Use EscapableHandleScope::Escape instead of Close()
  //return scope.Escape(cb); * Not Work *
  return scope.Escape(cb);
};

v8::Persistent<v8::Function> AAproperties::constructor;

//constructor
AAproperties::AAproperties(v8::Isolate* isolate): isolate_(isolate)
{
  this->version = "1.0";
  //this->context_ = v8::Context::New(isolate);

  // Initialize arributes
  std::string KY("KYTJ820101");
  std::string ZI("ZIMJ680102");
  std::string DW("DAWD720101");

  this->aaProperties.add(KY,KYTJ820101);
  this->aaProperties.add(ZI,ZIMJ680102);
  this->aaProperties.add(DW,DAWD720101);
  this->PropNameList.push_back("KYTJ820101");
  this->PropNameList.push_back("ZIMJ680102");
  this->PropNameList.push_back("DAWD720101");
};

//deconstructor
AAproperties::~AAproperties()
{
  //Dispose persistent handles
  context_.Reset();
  constructor.Reset();
  //process_.Reset();
};

// log
void AAproperties::Log(const char* event) {
  std::cout<< "Logged: " << event <<std::endl;
}

void AAproperties::addProperties(std::string const& name,
                                 aaIndex index) {
  this->aaProperties.add(name, index);
  this->PropNameList.push_back(name);
};

std::vector<double> AAproperties::get(const std::string& inxName,
                                      const std::string& seq) {
  std::vector<double> p;
  double val=0.00;

  for(const char &c : seq){
    std::string aa(&c,1);
    val = this->aaProperties.prop[inxName][aa];
    p.push_back(val);
  }
  return p;
};

//v8::Handle<v8::Array> AAproperties::getProperty(v8::Local<v8::String> inxName,
//        v8::Local<v8::String> seq, const v8::AccessorInfo& info){
// Static member function:
/*
v8::Handle<v8::Value> AAproperties::getProperty (v8::internal::Arguments& args){

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  // Extract C++ object reference from "this"
  // javascript:
  //  var x = new aaprop.aaProperties();
  //  x.getProperty(string, sequence, [callback]);
  //AAproperties* aaprop_instance =
  //   node::ObjectWrap::Unwrap<AAproperties>(args.This());

  AAproperties* aaprop_instance =
    node::ObjectWrap::Unwrap<AAproperties>(
        v8::Utils::ToLocal(
            v8::internal::Handle<v8::internal::Object>(args.arguments())
        )
    );

  if (args.length() < 2 ) return ThrowError("Wrong number of arguments!");
  // get arguments in old style
  //v8::String::Utf8Value iName(args[0]);
  //v8::String::Utf8Value iSeq(args[1]);
    v8::String::Utf8Value iName(
      v8::Utils::ToLocal(v8::internal::Handle<v8::internal::Object>(args[0])));

    v8::String::Utf8Value iSeq(
      v8::Utils::ToLocal(v8::internal::Handle<v8::internal::Object>(args[1])));

  std::vector<double> result = aaprop_instance->get(std::string(*iName),
                                                    std::string(*iSeq));
  // convert to javascript array
  v8::Handle<v8::Array> r = ArrayToV8<double>(&result[0],result.size());

  // If callback exists
  if ( args[2]->IsJSFunction() ){
    // Old interface:
    // v8::Local<v8::Function>::Cast(args[2]);
    v8::Local<v8::Function> cb = v8::Utils::ToLocal(
      v8::internal::Handle<v8::internal::Object>(args[2]));

    // pass property array to the script
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { v8::Local<v8::Value>::New(isolate,r) };
    // execute
    v8::Handle<v8::Value> js_res = cb->Call(v8::Context::GetCurrent()->Global(),
                                            argc,
                                            argv);

    return scope.Close(js_res);
  }
  return scope.Close(r);
};
*/

void AAproperties::getProperty(
    const v8::FunctionCallbackInfo<v8::Value> &args){

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  // instance of AAproperties
  // Unwrap problem:
  //    Assertion failed: (handle->InternalFieldCount() > 0), function Unwrap,
  //    file /Users/cthaxton/.nvm/HEAD/include/node/node_object_wrap.h
  // Reason:
  //  The issue is that the JavaScript "this" reference doesn't refer to an
  //  instance of PrimitiveClass, but rather of FullClass.  However, the
  //  Wrap/Unwrap code in C++ uses a pattern like:
  //    PrimitiveClass* primitive =
  //      ObjectWrap::Unwrap<PrimitiveClass>(args.This());
  // Solution:
  //   Change args.This() to args.Holder()
  v8::Local<v8::Object> lobj = args.Holder();
  if (lobj.IsEmpty()) {
    std::cout<<"Input object is empty."<<std::endl;
  }
  AAproperties* aaprop_instance =
      node::ObjectWrap::Unwrap<AAproperties>(lobj);
  if (aaprop_instance == NULL) {
    std::cout<<"Failed to unwrap the object.\n";
  }
  if (args.Length() < 2 ) {
    args.GetIsolate()->ThrowException(
      v8::String::NewFromUtf8(isolate, "Wrong number of arguments!"));
  }

  v8::String::Utf8Value iName(args[0]);
  v8::String::Utf8Value iSeq(args[1]);

  std::vector<double> result =
        aaprop_instance->get(std::string(*iName),
                             std::string(*iSeq));
  // convert to javascript array
  v8::Handle<v8::Array> r = ArrayToV8<double>(&result[0],
                                              static_cast<int>(result.size()));

  if (args[2]->IsFunction()) {
    v8::Local<v8::Function> cb = args[2].As<v8::Function>();;
    //std::cout<<" js Callbackfunction is called\n";
    // pass property array to the script
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = {
        v8::Local<v8::Value>::New(isolate,r)
    };
    // execute
    v8::Handle<v8::Value> js_res =
      cb->Call(isolate->GetCurrentContext()->Global(),
                                             argc,
                                             argv);
    //set return value
    args.GetReturnValue().Set(js_res);
    //scope.Escape(js_res);
  }else{
    //set return value
    args.GetReturnValue().Set(r);
    //scope.Escape(r);
  }
};

void AAproperties::New(
          const v8::FunctionCallbackInfo<v8::Value>& args){

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //v8::Isolate* isolate = args.GetIsolate();
  v8::EscapableHandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: new AAproperties(...)
    //AAproperties* aa_instance = new AAproperties(args.GetIsolate());
    AAproperties* aa_instance = new AAproperties(isolate);
    //set private variables
    // Wrap c++ object as a Javascript object
    //aa_instance->Wrap(args.This());

    aa_instance->Wrap(args.Holder());
    //return args.Holder(); //args.This();

  } else {
    // Invoked as plain function `AAproperties(...)`,
    // turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { args[0] };
    v8::Local<v8::Function> func =
       v8::Local<v8::Function>::New(isolate, constructor);
    //return scope.Escape(func->NewInstance(argc, argv));
    scope.Escape(func->NewInstance(argc, argv));
  }

  //Constructor function return a Javascript object
  //which is a wrapper for our C++ object,
  //This is the expected behavior when calling a constructor
  //function with the new operator in Javascript.
  //return args.This();
};

void AAproperties::NewInstance(
        const v8::FunctionCallbackInfo<v8::Value>& args){

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //v8::Isolate* isolate = args.GetIsolate();
  v8::EscapableHandleScope scope(isolate);
  //v8::EscapableHandleScop scope;

  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = { args[0] };

  ////////////////////////////////////////////////////////////////
  // Old fashion:
  //   v8::Local<v8::Object> instance =
  //    AAproperties::constructor->NewInstance(argc,argv);
  // In new version:
  //  Problem: v8::Persistent<v8::Function> is not a pointer.
  //  Reason: line 706 in v8.h
  //     #ifndef V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
  //    if set in default, cannot access raw handle constructor
  // New uage:
  //  Convert to Local<Function>
  ////////////////////////////////////////////////////////////////
  v8::Local<v8::Function> func =
    v8::Local<v8::Function>::New(isolate, constructor);
  v8::Local<v8::Object> instance = func->NewInstance(argc, argv);

  //return scope.Escape(instance);
  scope.Escape(instance);
}

//////////////////////
// Nodejs's entry
//////////////////////
void AAproperties::Init(v8::Handle<v8::Object> target){

  //Isolate:
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  // Prepare constructor template: New is AAproperties::New
  v8::Local<v8::FunctionTemplate> func_tpl = v8::FunctionTemplate::New(isolate, New);

  // Set a "class" name for objects created with our constructor
  func_tpl->SetClassName(v8::String::NewFromUtf8(isolate, "aaProperties"));

  // Each JavaScript object keeps a reference to the C++ object
  // for which it is a wrapper with an internal field.
  // 1 since a constructor function only references 1 object
  func_tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Add to object
  // FunctionTemplate: add to Prototype
  // { aaProperties: [Function] }
  func_tpl->PrototypeTemplate()->Set(
           v8::String::NewFromUtf8(isolate, "log"),
           v8::FunctionTemplate::New(isolate, LogCallBack));
  func_tpl->PrototypeTemplate()->Set(
           v8::String::NewFromUtf8(isolate, "getProperty"),
           v8::FunctionTemplate::New(isolate, AAproperties::getProperty));

  ////////////////////////////////////////////////////
  //func_tpl->Set
  // { aaProperties:
  //   { [Function] log: [Function: log],
  //        getProperty: [Function: getProperty]
  //   }
  // }
  /////////////////////////////////////////////////////
/*
  func_tpl->Set(
           v8::String::NewFromUtf8(isolate, "log"),
           v8::FunctionTemplate::New(LogCallBack));
  func_tpl->Set(
           v8::String::NewFromUtf8(isolate, "getProperty"),
           v8::FunctionTemplate::New(AAproperties::getProperty));
*/

  //constructor =
  //  v8::Persistent<v8::Function>::New(isolate,func_tpl->GetFunction());
  constructor.Reset(isolate, func_tpl->GetFunction());

  target->Set(
    v8::String::NewFromUtf8(isolate,"aaProperties", v8::String::kInternalizedString),
    func_tpl->GetFunction()
  );

  // Using ObjectTemplate
  /*
  v8::Local<v8::ObjectTemplate> ob_tpl = func_tpl->InstanceTemplate();
  ob_tpl->SetAccessor(v8::String::NewFromUtf8(isolate, "log"),
                      LogCallBack);
  ob_tpl->SetAccessor(v8::String::NewFromUtf8(isolate, "getProperty"),
                      v8::FunctionTemplate::New(AAproperties::getProperty));
  target = ob_tpl->NewInstance();
  */
};

/*
void AAproperties::Init(v8::Handle<v8::Object> target){
  // We need to declare a V8 scope so that our local handles are
  // eligible for garbage collection.
  // Once the Init() returns.
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  // Prepare constructor template
  // In old version:
  // v8::Local<v8::FunctionTemplate> tpl =
  //   v8::FunctionTemplate::New(New);
  v8::Local<v8::FunctionTemplate> tpl =
    v8::FunctionTemplate::New();

  // javascript function <=> java/C++ class
  // Set a class name for objects created with our constructor
  tpl->SetClassName(v8::String::NewSymbol("AAProperties"));

  // Each JavaScript object keeps a reference to the C++ object for which
  // it is a wrapper with an internal field.
  // 1 since this is a constructor function
  //AAproperties::persistent_function_template->InstanceTemplate()
  //  ->SetInternalFieldCount(1);
  v8::Local<v8::ObjectTemplate> instant_tpl = tpl->InstanceTemplate();
  instant_tpl->SetInternalFieldCount(1);

  //Callback
  v8::Local<v8::Function> clsGetProperty =
    v8::Local<v8::Function>::New(isolate, AAproperties::getProperty);

  // Use NODE_SET_METHOD or direct tpl
  //node::NODE_SET_METHOD(tpl,"log",LogCallBack);
  //node::NODE_SET_METHOD(tpl,"getProperty",AAproperties::getProperty);

  //  v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New();
  //    t->Set("func_property", v8::Number::New(1));
  // func_property in function == true;
  // function.func_property == 1;
  tpl->Set(v8::String::New("log"),
           v8::FunctionTemplate::New(LogCallBack));

  tpl->Set(v8::String::New("getProperty"),
           clsGetProperty);
//       v8::FunctionTemplate::New(AAproperties::getProperty)->GetFunction());


  // Set property accessors
  //tpl->InstanceTemplate()->SetAccessor(v8::String::New("prop"),
  //                                     setProperty, getProperty);
  //
  //instant_tpl->SetAccessor(
  //  v8::String::New("log"),
  //  LogCallBack);

  //instant_tpl->SetAccessor(
  //  v8::String::New("getProperty"),
  //  v8::FunctionTemplate::New(AAproperties::getProperty)->GetFunction());
  //


  //tpl->InstanceTemplate()->Set(v8::String::New("log"),v8::FunctionTemplate::New());
  // Arguments: our constructor function, Javascript method name, C++ method
  //NODE_SET_PROTOTYPE_METHOD(AAproperties::persistent_function_template, "", );

  // Make it persistent and assign it to our object's
  // persistent_function_template attribute
  //v8::Persistent<v8::Function>::New(isolate,instant_tpl->GetFunction());
  //AAproperties::persistent_function_template =
  //  v8::Persistent<v8::FunctionTemplate>::New(isolate,tpl);

  // Set the "aaProperties" property to the target and assign it to our constructor function
  //target->Set(
  //  v8::String::NewSymbol("aaProperties"),
  //            v8::Persistent<v8::Function>::New(isolate, tpl->GetFunction());
  //AAproperties::persistent_function_template->GetFunction());

};
*/
