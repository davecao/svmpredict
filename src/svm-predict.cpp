//
//  svm-predict.cpp
//  bilab
//
//  Created by 曹 巍 on 2013/12/06.
//  Copyright (c) 2013年 曹 巍. All rights reserved.
//

#include <cstddef>
#include "svm-predict.h"

#define PROCESS_DEBUG 1

#if defined(PROCESS_DEBUG)
#include <cassert>
#define CW_ASSERT(condition) \
{ \
if(!(condition)) \
{ \
std::cerr << __FILE__ << ":" << __LINE__; \
std::cerr << ": " << __FUNCTION__ ; \
std::cerr << "(...) -> " << #condition << std::endl; \
abort(); \
} \
}

#define log(message) { \
std::cout<<"Function: "<<__FUNCTION__<<" "<< #message <<std::endl; \
}
#endif

// Persistent: a heap-allocated object's handle, where it keep a reference to
//    an object for
//   1. multiple function call
//   2. when handle lifetimes do not correspond to C++ scopes
//   Persistent<SomeType>, Persistent::Dispose for disposal
v8::Persistent<v8::Function> SVMPredict::constructor_;

bool hasProperty(v8::Local<v8::Object> obj, std::string property) {
  //v8::HandleScope scope;
  //Check field: id
  v8::Local<v8::Value> id = obj->Get(ToV8<std::string>(property));
  bool res = ( id.IsEmpty() ) ? false : true;
  //return scope.Close(ToV8<bool>(res));
  return res;
}

SVMPredict::SVMPredict(v8::Isolate* isolate) {
  this->isolate_ = isolate;
  this->max_nr_attr = 64;
  this->predict_probability = false;
  this->isModelLoaded = false;
  //this->decision_val = std::numeric_limits<double>::infinity();
  //this->probPositive = std::numeric_limits<double>::infinity();
  //this->probNegative = std::numeric_limits<double>::min();
  this->model = nullptr;
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  context_.Reset(isolate,context);
  //this->x = nullptr;
};

SVMPredict::~SVMPredict() {
  //Dispose persistent handles
  context_.Reset();
  constructor_.Reset();
  //process_.Reset();
};
//
// Nodejs will call this function for initialization
//
void SVMPredict::Init(v8::Handle<v8::Object> target){
  // We need to declare a V8 scope so that our local handles are eligible
  // for garbage collection.
  // Once the Init() returns.
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  // Now it is still stack-allocated handle scope?
  // used for the Local<SomeType>
  v8::EscapableHandleScope scope(isolate);

  // Wrap our New() method so that it's accessible from Javascript
  v8::Local<v8::FunctionTemplate> fun_tpl =
  v8::FunctionTemplate::New(isolate, New);
  //v8::Local<v8::ObjectTemplate> obj_tpl = fun_tpl->PrototypeTemplate();

  // Each JavaScript object keeps a reference to the C++ object for which
  // it is a wrapper with an internal field.
  // 1 since this is a constructor function
  fun_tpl->InstanceTemplate()->SetInternalFieldCount(1);

#if defined(PROCESS_DEBUG)
  log("is called.");
#endif

  // Set a class name for objects created with our constructor
  fun_tpl->SetClassName(v8::String::NewFromUtf8(isolate, "SVMPredictor"));
  // Set functions
  fun_tpl->InstanceTemplate()->Set(
                                   v8::String::NewFromUtf8(isolate, "loadSVMModel"),
                                   v8::FunctionTemplate::New(isolate, SVMPredict::loadSVMModel)->GetFunction());

  fun_tpl->InstanceTemplate()->Set(
                                   v8::String::NewFromUtf8(isolate, "hasProb"),
                                   v8::FunctionTemplate::New(isolate, SVMPredict::hasProb)->GetFunction());

  fun_tpl->InstanceTemplate()->Set(
                                   v8::String::NewFromUtf8(isolate, "predict"),
                                   v8::FunctionTemplate::New(isolate, SVMPredict::predict)->GetFunction());

  fun_tpl->InstanceTemplate()->Set(
                                   v8::String::NewFromUtf8(isolate, "print_model"),
                                   v8::FunctionTemplate::New(isolate, SVMPredict::print_model)->GetFunction());

  fun_tpl->InstanceTemplate()->Set(
                                   v8::String::NewFromUtf8(isolate, "version"),
                                   ToV8<int32_t>(LIBSVM_VERSION));

  // Arguments: our constructor function, Javascript method name, C++ method
  //NODE_SET_PROTOTYPE_METHOD(SVMPredict::persistent_function_template, "", );
  // Make it persistent and assign it to our object's
  // persistent_function_template attribute
  constructor_.Reset(isolate,fun_tpl->GetFunction());

  // expose to javascript with name 'libsvm'
  target->Set(
              v8::String::NewFromUtf8(isolate, "libsvm"),
              fun_tpl->GetFunction());

};

//
// Response to create object from javascript's new()
//
void SVMPredict::New(const v8::FunctionCallbackInfo<v8::Value>& args) {

#if defined(PROCESS_DEBUG)
  log("is called.");
#endif
  //Creating temporary handles so we use a handle scope
  v8::Isolate* isolate = args.GetIsolate();//v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  if (args.IsConstructCall()) {
    SVMPredict* svmpredict_instance = new SVMPredict(isolate);
    //set private variables
    // Wrap c++ object as a Javascript object
    svmpredict_instance->Wrap(args.Holder());
    args.GetReturnValue().Set(args.Holder());
  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { args[0] };
    v8::Local<v8::Function> func =
    v8::Local<v8::Function>::New(isolate, constructor_);
    //return scope.Close(func->NewInstance(argc, argv));
    //scope.Close(func->NewInstance(argc, argv));
    args.GetReturnValue().Set(func->NewInstance(argc, argv));
  }
  //Constructor function return a Javascript object
  //which is a wrapper for our C++ object,
  //This is the expected behavior when calling a constructor
  //function with the new operator in Javascript.
  //return args.This();
};

//NOT IN USE
// Response to create object from javascript's newInstance()
//
// Type: static
void SVMPredict::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args){

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

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
  v8::Local<v8::Function>::New(isolate, constructor_);
  v8::Local<v8::Object> instance = func->NewInstance(argc, argv);

  scope.Escape(instance);
};

//
// Print model in detail
//
void SVMPredict::print_model(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() > 0) {
    args.GetIsolate()->ThrowException(
                                      v8::String::NewFromUtf8(args.GetIsolate(), "Unexpected arguments"));
    return;
  }
  SVMPredict* instance =
  node::ObjectWrap::Unwrap<SVMPredict>(args.Holder());
  instance->print_model();
  //args.GetReturnValue().Set(instance->print_model());
};

//
// Called to load a svm model file
//
void SVMPredict::loadSVMModel(const v8::FunctionCallbackInfo<v8::Value>& args){
  //v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //v8::HandleScope scope(isolate);
  //bool ok;
  SVMPredict* svmPredict_instance =
  node::ObjectWrap::Unwrap<SVMPredict>(args.Holder());
  if (args.Length() != 1) {
    args.GetIsolate()->ThrowException(
                                      v8::String::NewFromUtf8(args.GetIsolate(),"Bad parameters"));
    return;
    //ThrowError("Wrong number of arguments!");
    //args.GetReturnValue().Set(v8::Undefined());
    //scope.Close(v8::Undefined());
  }
  if (!args[0]->IsString()) {
    args.GetIsolate()->ThrowException(
                                      v8::String::NewFromUtf8(args.GetIsolate(),"Wrong argument type!"));
    return;
    //ThrowError("Wrong arguments!");
    //scope.Close(v8::Undefined());
  }
  v8::String::Utf8Value File(args[0]);
  //std::string modelfile = FromV8<std::string>(args[0],&ok);
  std::string modelfile = *File;
  if ( !svmPredict_instance->load_svm_model(modelfile)){
    ThrowError(("Failed to load the SVM model file!"+modelfile).c_str());
    args.GetReturnValue().Set(false);
  }else{
    svmPredict_instance->isModelLoaded = true;
    svmPredict_instance->predict_probability =
    (svm_check_probability_model(svmPredict_instance->model)==0) ? false: true;
    args.GetReturnValue().Set(true);
  }
};

//
// Check class's field: whether the model support probability
//
void SVMPredict::hasProb(const v8::FunctionCallbackInfo<v8::Value>& args){
  v8::Isolate* isolate = args.GetIsolate();//v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  SVMPredict* svmPredict_instance =
  node::ObjectWrap::Unwrap<SVMPredict>(args.Holder());

  if (svmPredict_instance == NULL) {
    std::cout<<"Failed to unwrap the object.\n";
    //scope.Escape();
  }
  if (svmPredict_instance->model == NULL){
    std::cout<<"The model is not loaded.\n";
    args.GetReturnValue().Set(false);
    //exit(0);
    //scope.Escape(svmPredict_instance);
  }
  //bool re = svmPredict_instance->hasProb();
  bool re = svmPredict_instance->predict_probability;
  args.GetReturnValue().Set(re);
  //scope.Close(ToV8<bool>(re));
};

//
// Convert javascript's json object to libsvm's svm_node
//
struct svm_node* SVMPredict::FromJsonObj(v8::Local<v8::Object> obj){
  //v8::Isolate* isolate = v8::Isolate::GetCurrent();
  //v8::EscapableHandleScope scope(isolate);
#if defined(PROCESS_DEBUG)
  log("is called.");
#endif
  struct svm_node *prob_node = nullptr;
  v8::Local<v8::Value> id;
  v8::Local<v8::Object> values;
  bool ok;
  if (hasProperty(obj,std::string("id")) &&
      hasProperty(obj,std::string("values"))){

    id = obj->Get(ToV8<std::string>(std::string("id")));
    values = v8::Local<v8::Object>::Cast(
                                         obj->Get(ToV8<std::string>(std::string("values"))));

    if ( !values->IsObject() ){
      ThrowError("Values field seem not to be json object.");
      //return scope.Close(v8::Null());
      return nullptr;
    }

    v8::Local<v8::Array> v = values->GetOwnPropertyNames();
    int length = v->Length();
    //#if PROCESS_DEBUG
    //  std::cout<< "feature length: " <<  length << std::endl;
    //#endif
    prob_node =
    reinterpret_cast<svm_node*>(malloc((length+1) * sizeof(struct svm_node)));
    //(struct svm_node *) malloc ((length+1) * sizeof(struct svm_node));
    if (prob_node == NULL){
      std::cout<<"Failed to allocate memory for svm_node"<<std::endl;
      return nullptr;
    }

    for(int i=0; i<length; i++){
      prob_node[i].index = FromV8<int32_t>(v->Get(i),&ok);
      prob_node[i].value = FromV8<double>(values->Get(v->Get(i)),&ok);
      //std::cout<< "index " << i <<" :" << prob_node[i].index<< ", value: " << prob_node[i].value << std::endl;
    }
    prob_node[length].index = -1;
    struct svm_node *p = prob_node;
    printf("In FromJsonObj:");
    while(p->index != -1) {
      printf("%d:%.8g ",p->index,p->value);
      p++;
    }
    printf("\n");
    return prob_node;
  }
  return nullptr;
}

//
// Called from javascript to do prediction
//  This method will call local prediction function
//
void SVMPredict::predict(const v8::FunctionCallbackInfo<v8::Value>& args){

  //v8::Isolate* isolate = args.GetIsolate();//v8::Isolate::GetCurrent();
  //v8::EscapableHandleScope scope(isolate);
  // arguments: 1
  //    Json: { id:"", value:{1:0.1, 2:3.5, ...} }
  // instance of SVMPredict
  //
  if (args.Length()!=1) {
    args.GetIsolate()->ThrowException(
                                      v8::String::NewFromUtf8(args.GetIsolate(), "Wrong arguments"));
    //return;
  }
  // Check arguments type: Null or undefined
  if (args[0]->IsNull() || args[0]->IsUndefined()) {
    args.GetIsolate()->ThrowException(
                                      v8::String::NewFromUtf8( args.GetIsolate(),
                                                              "The Input variable is Null or Undefined"));
    //return;
  }
  v8::Local<v8::Object> lobj = args.Holder();
  if (lobj.IsEmpty()) {
    std::cout<<"Input arguments is empty" <<std::endl;
  }
  SVMPredict* predictor =
  node::ObjectWrap::Unwrap<SVMPredict>(lobj);

  v8::Isolate* isolate = predictor->GetIsolate();
  v8::EscapableHandleScope scope(isolate);

#if defined(PROCESS_DEBUG)
  log(" is called.");
  //CW_ASSERT(&lobj==NULL);
  //CW_ASSERT(predictor==NULL);
#endif

  if (predictor==NULL) {
    std::cout<<"Failed to unwrap predictor: NULL"<<std::endl;
    return;
  }
#if defined(PROCESS_DEBUG)
  std::cout<<"Function:"<<__FUNCTION__<<" Success to unwrap predictor: "
  <<predictor->isModelLoaded<<std::endl;
#endif

  if (!predictor->isModelLoaded) {
    // Model file is not loaded.
    ThrowError("SVM Model is not loaded.");
    //return;
  }
#if defined(PROCESS_DEBUG)
  if (predictor->isModelLoaded) {
    log(" svm model is ready.");
  }
#endif
  struct svm_model* model = predictor->model;
  int nr_class=svm_get_nr_class(predictor->model);
  //svm_save_model(std::string("model").c_str(),predictor->model);
  if (args[0]->IsObject()){
    // single
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(args[0]);
    // Data conversion
    struct svm_node *prob_node = predictor->FromJsonObj(obj);
    if (prob_node != nullptr){
      //Check field: id
      v8::Local<v8::Value> id = obj->Get(ToV8<std::string>(std::string("id")));
      //predict
      //predictor->print_node(prob_node);
      v8::Local<v8::Object> Result =
      predictor->predict_single(model, prob_node,id,nr_class);

      // free memory
      //free(prob_node);<-- V8 will take it easy.
      //free(labels);
      //free(probab_estimate);
      args.GetReturnValue().Set(Result);
      //scope.Close(id);
      //scope.Close(Result);
    }else{
      ThrowError("Object does not have property id or value.");
      //scope.Escape(obj);
    }

  }else if (args[0]->IsArray()) {
    // Array of Object
    v8::Local<v8::Array> obj = v8::Local<v8::Array>::Cast(args[0]);
    // Data conversion
    struct svm_node **prob_node = nullptr;
    ///////////////////////////
    // Not finished yet
    ///////////////////////////
    //scope.Escape(obj);
  }else{
    ThrowError("Wrong arguments!");
    //scope.Escape(v8::Undefined(isolate));
  }

};

v8::Local<v8::Object> SVMPredict::predict_single (
                                                  const struct svm_model* m,
                                                  const struct svm_node* p,
                                                  v8::Local<v8::Value> id,
                                                  int nr) {

#if defined(PROCESS_DEBUG)
  log("is called.");
#endif
  if (m==NULL) {
    std::cerr<<"The model variable is NULL\n";
  }
  if (p == NULL) {
    std::cerr<<"The query vector variable is NULL\n";
  }
  //
  v8::Isolate* isolate = GetIsolate();//v8::Isolate::GetCurrent();
  // v8::HandleScope scope(isolate);

  //  v8::Local<v8::Context> context =
  //      v8::Local<v8::Context>::New(isolate,context_);

  //  v8::Context::Scope context_scop(context);

  v8::Local<v8::Object> Result = v8::Object::New(isolate);
  double dec_values = std::numeric_limits<double>::infinity();
  double pred_result = std::numeric_limits<double>::infinity();

  double *probab_estimate = (double *) malloc(nr*sizeof(double));
  int *labels=(int *) malloc(nr*sizeof(int));
  svm_get_labels(this->model,labels);

#if defined(PROCESS_DEBUG)
  //  CW_ASSERT(m==NULL);
  //  CW_ASSERT(p==NULL);
  log(" Ready to call local function predict");
#endif
  //this->predict(m,p,&dec_values,&pred_result);
  //this->print_node(p);
  pred_result = svm_predict_values(m, p, &dec_values);

  Result->Set(v8::String::NewFromUtf8(isolate, "id"),id);
  Result->Set(v8::String::NewFromUtf8(isolate, "dec_val"),ToV8<double>(dec_values));
  Result->Set(v8::String::NewFromUtf8(isolate, "predict"),ToV8<double>(pred_result));
  if (this->predict_probability) {

#if defined(PROCESS_DEBUG)
    //CW_ASSERT(m==NULL);
    //CW_ASSERT(p==NULL);
    log(" Ready to call local function predict_prob");
#endif
    this->predict_prob(m, p, &pred_result, probab_estimate);
    //pred_result = svm_predict_probability(this->model,p,probab_estimate);

    v8::Local<v8::Object> prob = v8::Object::New(isolate);
    for(int i=0;i<nr;i++){
      std::string cl = _toString<int>(labels[i],std::dec);
      prob->Set(v8::String::NewFromUtf8(isolate, cl.c_str()),ToV8<double>(probab_estimate[i]));
    }
    Result->Set(v8::String::NewFromUtf8(isolate, "probability"),prob);
  }
  return Result;
}

void SVMPredict::predict(const struct svm_model* m,
                         const struct svm_node* x,
                         double* dec_values,
                         double* pred_result) {
  //v8::HandleScope scope(GetIsolate());
#if defined(PROCESS_DEBUG)
  log("is called.");
  log(" Problem vector");
  //  this->print_model();
  //this->print_node(x);
  //CW_ASSERT(m==NULL);
  //CW_ASSERT(x==NULL);
#endif
  *pred_result = svm_predict_values(m, x, dec_values);
}

void SVMPredict::predict_prob(const struct svm_model* m,
                              const struct svm_node* x,
                              double* label,
                              double* prob_estimate) {
  //v8::HandleScope scope(GetIsolate());
#if defined(PROCESS_DEBUG)
  log("is called");
  //CW_ASSERT(m==NULL);
  //CW_ASSERT(x==NULL);
#endif
  *label = svm_predict_probability(m,x,prob_estimate);
}

bool SVMPredict::load_svm_model(std::string& path){

  this->model = svm_load_model(path.c_str());
  //this->print_model();
  //this->x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
  return (this->model == nullptr)? false :true;
}

bool SVMPredict::hasProb(){
  return (svm_check_probability_model(this->model)==0) ? false: true;
}

bool SVMPredict::isReady(){
  return (this->isModelLoaded) ? true : false;
}

void SVMPredict::print_node(const svm_node* p) {
  while(p->index != -1) {
    printf("%d:%.8g ",p->index,p->value);
    p++;
  }
  printf("\n");
}

void SVMPredict::print_model(){
  v8::HandleScope scope(GetIsolate());
  /* number of classes, = 2 in regression/one class svm */
  printf("nr_class: %d\n",this->model->nr_class);
  /* total #SV */
  printf("l(total SV): %d\n", this->model->l);
  //  struct svm_node **SV;   /* SVs (SV[l]) */
  //double **sv_coef;
  /* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
  /* constants in decision functions (rho[k*(k-1)/2]) */
  printf("rho: %f\n",*(this->model->rho));
  /* pariwise probability information */
  printf("probA: %f\n",*(this->model->probA));
  printf("probB:%f \n",*(this->model->probB));
  //  int *sv_indices;
  /* sv_indices[0,...,nSV-1] are values in [1,...,num_traning_data] to
   indicate SVs in the training set */

  /* for classification only */

  //  int *label;   /* label of each class (label[k]) */
  //  int *nSV;   /* number of SVs for each class (nSV[k]) */
  /* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
  /* XXX */
  /* 1 if svm_model is created by svm_load_model*/
  printf("free_sv: %d \n",this->model->free_sv);
  // parameters
  printf("svm_type: %d \n",this->model->param.svm_type);
  printf("kernel_type: %d \n",this->model->param.kernel_type);
  printf("degree: %d\n",this->model->param.degree); /* for poly */
  printf("gamma for poly/rbf/sigmoid: %f\n",this->model->param.gamma);
  printf("coef0 for poly/sigmoid : %f \n",this->model->param.coef0);

  /* these are for training only */
  //  double cache_size; /* in MB */
  //  double eps; /* stopping criteria */
  //  double C; /* for C_SVC, EPSILON_SVR and NU_SVR */
  //  int nr_weight;    /* for C_SVC */
  //  int *weight_label;  /* for C_SVC */
  //  double* weight;   /* for C_SVC */
  //  double nu;  /* for NU_SVC, ONE_CLASS, and NU_SVR */
  //  double p; /* for EPSILON_SVR */
  //  int shrinking;  /* use the shrinking heuristics */
  //  int probability; /* do probability estimates */
  printf("labels: ");
  for(int i = 0; i<this->model->nr_class; i++){
    printf("%d ",this->model->label[i]);
  }
  printf("\n");
  printf("nSV: ");
  for(int i = 0; i<this->model->nr_class; i++){
    printf("%d ",this->model->nSV[i]);
  }
  printf("\n");
  printf("SVs: \n");

  int nr_class = model->nr_class;
  int l = model->l;
  const double * const *sv_coef = model->sv_coef;
  const svm_node * const *SV = model->SV;

  for(int i=0;i<l;i++) {
    for(int j=0;j<nr_class-1;j++)
      printf("%.16g ",sv_coef[j][i]);

    const svm_node *p = SV[i];
    while(p->index != -1) {
      printf("%d:%.8g ",p->index,p->value);
      p++;
    }
    printf("\n");
  }

  /*  struct svm_node **lpp,*lp;
   for(lpp = this->model->SV; *lpp != -1; lpp++){
   for (lp = *lpp; lp != NULL; lp++){
   printf("%d:%f,",lp->index,lp->value);
   }
   printf("\n");
   }
   printf("\n");
   */
};

