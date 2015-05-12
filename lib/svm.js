/*
 * svm.js
 *
 *  Created on: 
 *      Author: Wei Cao (davecao@bi.a.u-tokyo.ac.jp)
 * Description:
 *    
 */
var aaprop = require("../build/Release/aaproperties");
var predictor = require("../build/Release/svmpredict");

module.exports = {
	aaprop:aaprop.aaProperties,
	predictor:predictor.predictor
};
//exports.predictor;
