//
//  svm-predict.h
//  bilab
//
//  Created by 曹 巍 on 2013/12/06.
//  Copyright (c) 2013年 曹 巍. All rights reserved.
//

// Note:
// Binding functions take a const v8::FunctionCallbackInfo<T>& args
//   rather than v8::Arguments& args
// Binding functions return void rather than v8::Handle<v8::Value>/
//   The return value is returned with args.GetReturnValue().Set() family
//   of functions


#ifndef bilab_svm_predict_h
#define bilab_svm_predict_h

#include "common.h"
#include "svm.h"

class SVMPredict: public node::ObjectWrap
{
private:
  int max_nr_attr;

  bool predict_probability;
  bool isModelLoaded;
  //double decision_val;
  //double probPositive;
  //double probNegative;

  //struct svm_node *x;
  struct svm_model* model;

  v8::Isolate* isolate_;
  // Holds context env
  v8::Persistent<v8::Context> context_;

  // Holds constructor function
  static v8::Persistent<v8::Function> constructor_;

  // Private functions
  v8::Isolate* GetIsolate() { return isolate_; }
  //v8::Persistent<v8::Context> GetContext() { return context_; }
 // support new call from javascript
  //static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  // take care of instantiating the object
  // it does the job of new in Javascript
  // NOT in Use yet!!!!!!
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

  // v8 interface: will be used as property of javascript object
  //static v8::Handle<v8::Value> loadSVMModel(const v8::Arguments& args);
  //static v8::Handle<v8::Value> hasProb(const v8::Arguments& args);
  //static v8::Handle<v8::Value> predict(const v8::Arguments& args);
  static void loadSVMModel(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void hasProb(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void predict(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void print_model(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Data conversion
  struct svm_node* FromJsonObj(v8::Local<v8::Object> obj);
  //for single query
  v8::Local<v8::Object> predict_single (const struct svm_model* m,
                                        const struct svm_node* p,
                                        v8::Local<v8::Value> id,
                                        int nr);
  //Load trained SVM model
  bool load_svm_model(std::string& path);
  // Check whether the model support calculation of probability.
  bool hasProb();
  bool isReady();
  // Given the svm problem, predict one by one.
  void predict(const struct svm_model* m, const struct svm_node* x,
               double* dec_values, double* pred_result);

  void predict_prob(const struct svm_model* m, const struct svm_node* x,
                    double* label, double* prob_estimate);
  void print_model();
  void print_node(const svm_node* p);
public:

  SVMPredict(v8::Isolate* isolate);
  ~SVMPredict();

  //Nodejs will call Init()
  //when loading the extension through require().
  static void Init(v8::Handle<v8::Object> target);
  //void Init(v8::Handle<v8::Object> target);
};

extern "C" {
  static void init(v8::Handle<v8::Object> target){
    SVMPredict::Init(target);
  }
  // @see node.h: without semicolon
  NODE_MODULE(svmpredict,init)
}


#endif
