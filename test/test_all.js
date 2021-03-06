/*
 * aaproperties.js
 *
 *  Created on:
 *      Author: Wei Cao (davecao@bi.a.u-tokyo.ac.jp)
 * Description:
 *
 */

//var aaprop = require("../build/Release/aaproperties");
//var svmpredict = require("../build/Release/svmpredict");
var aaprop = require("../build/Debug/aaproperties");
var svmpredict = require("../build/Debug/svmpredict");

console.log(aaprop);
var x = new aaprop.aaProperties();
//console.log(x());
x.log("test");
for (var p in x){
  console.log(p);
}
console.log(x.getProperty("KYTJ820101","ACDE"));
console.log(x.getProperty("KYTJ820101","ACD"));
console.log(x.getProperty("KYTJ820101","AC"));
console.log(x.getProperty("KYTJ820101","A"));
//console.log(x.getProperty("KYTJ820101"));

var sum = avg = 0;
var s = x.getProperty("KYTJ820101","ACDE");
s.forEach(function(n){
    sum += n;
    avg = sum / s.length;
});
console.log("Mean of ACDE in KYTJ820101: "+ avg+"["+sum+"]");

console.log(x.getProperty("KYTJ820101","ACDE",function(s){
  var sum = avg = 0;
  s.forEach(function(n){
    sum += n;
    avg = sum / s.length;
  });
  return avg;
})
);

console.log("SVM predictor---------------");
console.log(svmpredict);
var predictor = new svmpredict.libsvm();
console.log(predictor);
console.log("Predictor[probability]: "+predictor.hasProb());
var file = __dirname+'/'+'models'+'/'+'GPImodel.model';
console.log("Model file:" +file);
var ok = predictor.loadSVMModel(file);
console.log("load SVM model file: "+ok);
console.log("Predictor[probability]: "+predictor.hasProb());
problem = {
  id:"random",
  values: {
    1:1.5,
    4:0.9,
    6:10
 }
};
prob_sv = {
 id:"seq",
 values:{
    1:-1.311111,
    2:-1.266667,
    3:-1.266667,
    4:-1.222222,
    5:-1.111111,
    6:-0.255556,
    7:-0.255556,
    8:-0.255556,
    9:-0.011111,
   10:0.877778,
   11:1.177778,
   12:0.244444,
   13:0.844444,
   14:-0.055556,
   15:-0.055556,
   16:0.455556,
   17:0.355556,
   18:0.322222,
   19:0.022222,
   20:0.577778,
   21:0.622222,
   22:0.366667,
   23:0.355556,
   24:0.388889,
   25:-0.422222,
   26:0.033333,
   27:0.122222,
   28:0.122222,
   29:-0.388889,
   30:-0.088889,
   31:0.311111,
   32:1.033333,
   33:0.811111,
   34:1.155556,
   35:0.755556,
   36:1.155556,
   37:0.866667,
   38:1.333333,
   39:1.922222,
   40:1.877778,
   41:2.022222,
   42:1.655556,
   43:2.166667,
   44:2.677778,
   45:2.566667,
   46:3.122222,
   47:3.200000,
   48:3.200000,
   49:3.388889,
   50:3.388889,
   51:3.333333,
   52:2.477778
 }
};

console.log(problem);
var re_rand = predictor.predict(problem);
var re_real = predictor.predict(prob_sv);
console.log("----- random -----");
console.log(re_rand);
console.log("----- sv -----");
console.log(re_real);
