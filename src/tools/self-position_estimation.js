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
  wheelSize : 26,
  interval : 1/50,

  weight: 0.001,

  existance: [],
  featureCount: [],
  thresholdOfCurve : {},

  ssv : [],

  //J Initialize
  initialize: function(stateSpaceVec, unit_mm, wheelSize, interval, thresholdOfCurve) {
    this.ssv = stateSpaceVec;
    this.unit_mm = unit_mm;
    this.wheelSize = wheelSize;
    this.interval = interval;
    this.thresholdOfCurve = thresholdOfCurve;

    //J 移動距離を初期化
    this.milage_mm = 0.0;
    this.milage_index = 0;

    //J 存在確率分布の初期値を入れる
    this.existance = new Array(this.ssv.length);
    this.existance.fill(1.0/this.ssv.length); //一様分布でいんだろうか…。

    //J 特徴量の個数だけ先に求めておく
    this.featureCount = new Array(9); //TODO
    this.featureCount.fill(0);

    for (var i=0 ; i<this.ssv.length ; ++i) {
      this.featureCount[this.ssv[i].feature]++;
    }
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
  estimate: function(delta_mm, feature) {
    this.milage_mm += delta_mm;

    if (this.featureCount[feature] == 0) {
      return -1;
    }

    var max = 0;
    var max_index = -1;
    var weighted_existance = this.weight / this.featureCount[feature];
    for (var i=0 ; i<this.existance.length ; ++i) {
      var prob = this.getExistance(i) * (1-this.weight);
      if (this.ssv[i].feature == feature) {
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
}


//
// シミュレータのアップデート処理. 時間ごとの処理を行う
//
function updateSimulater(tick, log)
{
  var delta_mm = SelfPositionEstimater.wheelSize * Math.PI * (log[IDX_RPM][tick] / 60.0) * SelfPositionEstimater.interval;

  var yaw_deg = log[IDX_YAW][tick]*SelfPositionEstimater.interval/1000.0;;
  if (log[IDX_YAW][tick] < 10000.0 && log[IDX_YAW][tick] > -10000.0) {
    yaw_deg = 0.0
  }

  var pitch_deg = log[IDX_PITCH][tick]*SelfPositionEstimater.interval/1000.0;
  var feature  = featureValue(delta_mm, yaw_deg, pitch_deg, SelfPositionEstimater.thresholdOfCurve);

  var position = SelfPositionEstimater.estimate(delta_mm, feature);

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
function initializeSimulater(stateSpaceVec, unit_mm, wheelSize, interval, thresholdOfCurve)
{
  SelfPositionEstimater.initialize(stateSpaceVec, unit_mm, wheelSize, interval, thresholdOfCurve);
}

