/*
 * self-position_estimation.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

//J 自己位置推定系の変数
var SelfPositionEstimater = {
  milage_mm : 0,
  milage_index : 0,
  unit_mm : 50,
  weight: 0.001,

  existance: [],
  featureCount: [],
  thresholdOfCurve : {},

  //J Initialize
  initialize: function(stateSpaceVec, unit_mm) {
    this.unit_mm = unit_mm;

    //J 移動距離を初期化
    this.milage_mm = 0.0;
    this.milage_index = 0;

    //J 存在確率分布の初期値を入れる
    this.existance = new Array(stateSpaceVec.length);
    this.existance.fill(0); //スタート地点に集める
    var lap = 0;
    this.existance[0] = 1.0/3.0;
    for (var i=1 ; i<stateSpaceVec.length ; ++i) {
      if (stateSpaceVec[i].lap != lap) {
        lap = stateSpaceVec[i].lap;
        this.existance[i] = 1.0/3.0;
      }
    }
    this.existance.fill(1.0/stateSpaceVec.length); //一様分布でいんだろうか…。

    //J 特徴量の個数だけ先に求めておく
    this.featureCount = new Array(9); //TODO
    this.featureCount.fill(0);

    for (var i=0 ; i<stateSpaceVec.length ; ++i) {
      this.featureCount[stateSpaceVec[i].feature]++;
    }
    console.log(this.featureCount);
  },

  //J 存在確率分布を返す
  getExistanceArr: function() {
    var slicePos = (this.existance.length-1) - Math.round(this.milage_mm/this.unit_mm) % this.existance.length;
　　if (slicePos < 1) {
      return this.existance;
    } else {
      return (this.existance.slice(slicePos)).concat(this.existance.slice(0, slicePos-1))
    }
  },
  //J 位置Indexにマシンがいる確率を取得
  getExistance: function(index) {return this.existance[(this.existance.length + index - Math.round(this.milage_mm/this.unit_mm)) % this.existance.length]},
  //J 位置Indexにマシンがいる確率を設定
  setExistance: function(index, val) {this.existance[(this.existance.length + index - Math.round(this.milage_mm/this.unit_mm)) % this.existance.length] = val},
  //J 移動距離、移動中の場所の特徴量から、存在確率をアップデートする
  estimate: function(stateSpaceVec, delta_mm, feature) {
    this.milage_mm += delta_mm;

    if (this.featureCount[feature] == 0) {
      return -1;
    }

    var max = 0;
    var max_index = -1;
    var weighted_existance = this.weight / this.featureCount[feature];
    for (var i=0 ; i<this.existance.length ; ++i) {
      var prob = this.getExistance(i) * (1-this.weight);
      if (stateSpaceVec[i].feature == feature) {
        prob += weighted_existance;
      }
      this.setExistance(i, prob);

      if (prob > max) {
        max = prob;
        max_index = i;
      }
    }

    return max_index;
  },

  //J 回転半径の閾値を設定
  setThresholdOfCurve : function (ThresholdOfCurve) {
    this.thresholdOfCurve = ThresholdOfCurve
  }
}


//
// シミュレータのアップデート処理. 時間ごとの処理を行う
//
function updateSimulater(stateSpaceVec, tick)
{
  var Interval = 1.0/52.0;
  var WheelSize = parseFloat(document.getElementById("wheel_size").value);
  if (WheelSize == NaN) {
    return;
  }

  var delta_mm = WheelSize * Math.PI * (TestData[IDX_RPM][tick] / 60.0) * Interval;
  var feature  = featureValue(delta_mm, TestData[IDX_YAW][tick]*Interval/1000.0, TestData[IDX_PITCH][tick]*Interval/1000.0, SelfPositionEstimater.thresholdOfCurve);

  var position = SelfPositionEstimater.estimate(stateSpaceVec, delta_mm, feature);

  return position;
}


//
// 存在確率分布を返す
//
function getExistanceArray()
{
  return SelfPositionEstimater.getExistanceArr();
}

//
// シミュレータの初期化処理
//
function initializeSimulater(stateSpaceVec, unit_mm)
{
  SelfPositionEstimater.initialize(stateSpaceVec, unit_mm);
}

