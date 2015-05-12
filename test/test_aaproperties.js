/*
 * test_aaproperties.js
 *
 *  Created on: 
 *      Author: Wei Cao (davecao@bi.a.u-tokyo.ac.jp)
 * Description:
 *    
 */
 
var aaprop = require("../build/Release/aaproperties");

if (aaprop) {
  console.log("Package: ");
  console.log(aaprop);  
}

var x = new aaprop.aaProperties();
//var x = aaprop.aaProperties;
console.log(x);
x.log("test");
for (var p in x){
  console.log(p);
}
console.log("Input: ACDE");
console.log(x.getProperty("KYTJ820101","ACDE"));
console.log("Input: ACD");
console.log(x.getProperty("KYTJ820101","ACD"));
console.log("Input: AC");
console.log(x.getProperty("KYTJ820101","AC"));
console.log("Input: A");
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

