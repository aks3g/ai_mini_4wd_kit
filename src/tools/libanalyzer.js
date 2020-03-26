/*
 * libanlyzer.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
/*-------------------------------------------------------------------------------------------------
 * データ解析関係処理
 */
/*-------------------------------------------------------------------------------------------------
 * 各種変数/定数定義
 */
//J TODO読み込むセンサデータの情報を決め打ちにする（将来的に柔軟にやっていきたい）
var IDX_AX    = 0;
var IDX_AY    = 1;
var IDX_AZ    = 2
var IDX_PITCH = 3;
var IDX_ROLL  = 4;
var IDX_YAW   = 5;
var IDX_RPM   = 6;
var IDX_VBAT  = 7;
var IDX_IMOT  = 8;
var IDX_DUTY  = 9;

var Jcjc_CenterRadius = 540;
var Jcjc_OuterRadius  = Jcjc_CenterRadius + 115;
var Jcjc_InnerRadius  = Jcjc_CenterRadius - 115;


//J 3ラップ分の色情報とハイライト時の色情報
var FEATURE_UPDOWN = 1;
var FEATURE_OUT_LEFT = 2;
var FEATURE_CENTER_LEFT = 3;
var FEATURE_IN_LEFT = 4;
var FEATURE_OUT_RIGHT = 5;
var FEATURE_CENTER_RIGHT = 6;
var FEATURE_IN_RIGHT = 7;
var FEATURE_STRAIGHT = 8

var TracingContext = {
  xArr: [],
  yArr: [],
  lapArr: [],
}

//J 状態空間のベクトルを作る時に使う構造体もどき。死にたい
StateInfo = function(x, y, lap, feature) {
  this.x = x;
  this.y = y;
  this.lap = lap;
  this.feature = feature;
}


//
// 回転数、加速度から速度と移動量を割り出す
//
function estimateVelocityAndOdometory(data, wheelSize)
{
  var velocityArr = [];
  var velocityArrFromAccel = [];

  var odometryArr = [];
  var odometryArrFromAccel = [];

  var Interval = 1.0/52.0;

  var odometry = 0;
  var velocity = 0;
  //J タコメータベースの速度と距離
  for (var i=0 ; i<data[IDX_RPM].length ; ++i) {
    var delta_mm = wheelSize * Math.PI * (data[IDX_RPM][i] / 60.0) * Interval;
    velocity = (delta_mm / (1000.0 * 1000.0)) / (Interval / 3600.0);

    velocityArr.push(velocity)

    odometry += delta_mm;
    odometryArr.push(odometry);
  }

  //J 加速度センサベースの速度と距離
  var odometry = 0;
  var velocity = 0;
  for (var i=0 ; i<data[IDX_AY].length ; ++i) {
    odometry = 0.5 * (9.8 / 1000.0) * data[IDX_AY][i] * Interval * Interval + velocity * Interval + odometry;
    velocity = (9.8 / 1000.0) * data[IDX_AY][i] * Interval + velocity;

    velocityArrFromAccel.push(velocity * (3600.0 / 1000.0))   //J m/s -> km/h
    odometryArrFromAccel.push(odometry * 1000); //J m -> mm
  }

  return [velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel];
}

//
// Canvas上にミニ四駆の位置を推定する
//
function estimateMachinePosition(data, coeff, wheelSize)
{
  if (data.length == 0) {
    return 0;
  }

  TracingContext.xArr = [];
  TracingContext.yArr = [];
  TracingContext.lapArr = [];

  var Interval = 1.0/52.0;

  //J タコメータの値とYaw軸の回転を見ながら位置をプロットする
  var direction = 0;
  var posX = 0;
  var posY = 0;
  var lap = 0;

  for (var i=0 ; i<data[IDX_RPM].length ; ++i) {
    var delta_mm = wheelSize * Math.PI * (data[IDX_RPM][i] / 60.0) * Interval;

    yaw_corrected = (data[IDX_YAW][i] < 0) ? (data[IDX_YAW][i] * coeff.right) : (data[IDX_YAW][i] * coeff.left)
    direction += getValidAngularVelocity_degree(yaw_corrected) * Interval; //mdegree/sec -> degree
    direction = round180(direction)
    if (posX < 0 && (posX + delta_mm * Math.cos(direction * Math.PI/180.0)) >= 0) {
      lap++;
      direction = 0;
    }
    posX += delta_mm * Math.cos(direction * Math.PI/180.0);
    posY += delta_mm * Math.sin(direction * Math.PI/180.0);

    TracingContext.xArr.push(posX);
    TracingContext.yArr.push(posY);
    TracingContext.lapArr.push(lap);
  }

  return lap;
}

//
// 閾値チェックをして、信用できる値だけ返す
//
function getValidAngularVelocity_degree(mdegree)
{
  if (Math.abs(mdegree) < 10000.0) {
    return 0.0;
  }
  else {
    return mdegree / 1000.0;
  }
}

//
// 特定のラップの先頭位置を探す
//
function searchHeadOfLap(lap, lapArr)
{
  for(var i=0 ; i<lapArr.length ; ++i) {
    if(lapArr[i] == lap) {
      return i;
    }
  }

  return NaN;
}

//
// 特定のラップの終了位置を探す
//
function searchTailOfLap(lap, lapArr)
{
  for(var i=lapArr.length-1 ; i>=0 ; --i) {
    if(lapArr[i] == lap) {
      return i;
    }
  }

  return NaN;
}

//
// 角度(degree)を、プラスマイナス180度に正規化する
//
function round180(direction)
{
  direction = direction % 360;
  if (direction < 0) {
    direction = direction + 360;
  }

  if (direction > 180) {
    direction = direction - 360;
  }

  return direction;
}


//
// unitで指定された距離ごとにセンサデータをリサンプリングする
//
function resampling(data, wheelSize, unit)
{
  var Interval = 1.0/52.0;
  var resampled = [[],[],[],[],[],[],[],[],[],[]];

  var ax = 0;
  var ay = 0;
  var az = 0;
  var pitch = 0;
  var roll = 0;
  var yaw = 0;
  var rpm = 0;
  var vbat = 0;
  var imot = 0;

  //J 全データをなめる
  var distance = 0;
  var index = 0;
  var cnt = 0;
  for (var i=0 ; i<data[0].length ; ++i) {
    var delta_mm = wheelSize * Math.PI * (data[IDX_RPM][i] / 60.0) * Interval;

    if ((delta_mm + distance) > index * unit) {
      //J 未処理の距離
      var fragment = (index * unit) - distance;
      var remaining_delta = delta_mm;
      //J 1回のイテレーションでUnit以上進むケースを考慮
      do {
        cnt++;
        var remaining_rate = (fragment / delta_mm);

        ax    += data[IDX_AX][i];
        ay    += data[IDX_AY][i];
        az    += data[IDX_AZ][i];
        pitch += data[IDX_PITCH][i] * remaining_rate * 1/52;
        roll  += data[IDX_ROLL][i]  * remaining_rate * 1/52;
        yaw   += data[IDX_YAW][i]   * remaining_rate * 1/52;
        rpm   += data[IDX_RPM][i];
        vbat  += data[IDX_VBAT][i];
        imot  += data[IDX_IMOT][i];

        resampled[IDX_AX].push(ax/cnt)
        resampled[IDX_AY].push(ay/cnt)
        resampled[IDX_AZ].push(az/cnt)
        resampled[IDX_PITCH].push(pitch)
        resampled[IDX_ROLL].push(roll)
        resampled[IDX_YAW].push(yaw)
        resampled[IDX_RPM].push(rpm/cnt)
        resampled[IDX_VBAT].push(vbat/cnt)
        resampled[IDX_IMOT].push(imot/cnt)

        ax    = 0;
        ay    = 0;
        az    = 0;
        pitch = 0;
        roll  = 0;
        yaw   = 0;
        rpm   = 0;
        vbat  = 0;
        imot  = 0;

        index++;
        cnt = 0;
	remaining_delta -= fragment;

        fragment = unit;
      } while (remaining_delta > unit)

      var fragment_rate = (remaining_delta) / delta_mm;
      ax    = data[IDX_AX][i];
      ay    = data[IDX_AY][i];
      az    = data[IDX_AZ][i];
      pitch = data[IDX_PITCH][i] * fragment_rate * 1/52;
      roll  = data[IDX_ROLL][i]  * fragment_rate * 1/52;
      yaw   = data[IDX_YAW][i]   * fragment_rate * 1/52;
      rpm   = data[IDX_RPM][i];
      vbat  = data[IDX_VBAT][i];
      imot  = data[IDX_IMOT][i];
      cnt++;
    }
    else {
      ax    += data[IDX_AX][i];
      ay    += data[IDX_AY][i];
      az    += data[IDX_AZ][i];
      pitch += data[IDX_PITCH][i] * 1/52;
      roll  += data[IDX_ROLL][i] * 1/52;
      yaw   += data[IDX_YAW][i] * 1/52;
      rpm   += data[IDX_RPM][i];
      vbat  += data[IDX_VBAT][i];
      imot  += data[IDX_IMOT][i];
      cnt++;
    }
    distance = delta_mm + distance;
  }

  return resampled;
}

//
// 移動距離とYaw軸、Pitch軸の角度変化から特徴量を出します
//
function featureValue(delta_mm, yaw_deg, pitch_deg, threshold)
{
  var feature = 0;
  var radius = 0

  if (Math.abs(yaw_deg) != 0) {
    radius = (delta_mm / (yaw_deg / 360)) / (2 * Math.PI);
  }

  if (pitch_deg > 3.0) {
    feature = FEATURE_UPDOWN;
  }
  else if (threshold.center_to_outer_left <= radius && radius < 800) {
    feature = FEATURE_OUT_LEFT;
  }
  else if (threshold.inter_to_center_left <= radius && radius < threshold.center_to_outer_left) {
    feature = FEATURE_CENTER_LEFT;
  }
  else if (300 <= radius && radius < threshold.inter_to_center_left) {
    feature = FEATURE_IN_LEFT;
  }
  else if (threshold.inter_to_center_right <= radius && radius < -300) {
    feature = FEATURE_IN_RIGHT;
  }
  else if (threshold.center_to_outer_right <= radius && radius < threshold.inter_to_center_right) {
    feature = FEATURE_CENTER_RIGHT;
  }
  else if (-800 <= radius && radius < threshold.center_to_outer_right) {
    feature = FEATURE_OUT_RIGHT;
  }
  else {
    feature = FEATURE_STRAIGHT;
  }

  return feature;
}


//
// 特定コースにおける状態空間を作ります
//
function createStateSpaceVector(data, unit, coeff, wheelSize, threshold)
{
  var Interval = 1.0/52.0;
  var stateSpaceVec = []

  var posX = 0;
  var posY = 0;
  var lap  = 0;

  var distance = 0;
  var direction = 0;

  var yaw = 0;
  var pitch = 0;
  var rpm = 0;

  var index = 0;
  var cnt = 0;

  for (var i=0 ; i<data[0].length ; ++i) {
    var delta_mm = wheelSize * Math.PI * (data[IDX_RPM][i] / 60.0) * Interval;

    yaw_corrected = (data[IDX_YAW][i] < 0) ? (data[IDX_YAW][i] * coeff.right) : (data[IDX_YAW][i] * coeff.left)
    direction += getValidAngularVelocity_degree(yaw_corrected) * Interval; //mdegree/sec -> degree
    direction = round180(direction)
    if (posX < 0 && (posX + delta_mm * Math.cos(direction * Math.PI/180.0)) >= 0) {
      lap++;
      direction = 0;
      if (lap >= 3) {
        break;
      }
    }
    posX += delta_mm * Math.cos(direction * Math.PI/180.0);
    posY += delta_mm * Math.sin(direction * Math.PI/180.0);

    //J 状態空間単位毎にパッキング
    if ((delta_mm + distance) > index * unit) {
      var fragment = (index * unit) - distance;
      var remaining_delta = delta_mm;

      //J 1回のイテレーションでUnit以上進むケースを考慮
      do {
        cnt++;
        var remaining_rate = (fragment / delta_mm);

        yaw   += data[IDX_YAW][i] * remaining_rate * 1/52;
        pitch += data[IDX_PITCH][i] * remaining_rate * 1/52;
        rpm   += data[IDX_RPM][i];

        var feature = featureValue(unit, yaw / 1000.0, pitch / 1000.0, threshold);

        //J 状態空間の1点としてPush
        stateSpaceVec.push(new StateInfo(posX, posY, lap, feature));

        pitch = 0;
        yaw   = 0;
        rpm   = 0;

        index++;
        cnt = 0;
	remaining_delta -= fragment;

        fragment = unit;
      } while (remaining_delta > unit)

      var fragment_rate = (remaining_delta) / delta_mm;
      yaw   = data[IDX_YAW][i] * fragment_rate * 1/52;
      pitch = data[IDX_PITCH][i] * fragment_rate * 1/52;
      rpm   = data[IDX_RPM][i];
      cnt++;
    }
    else {
      yaw   += data[IDX_YAW][i] * 1/52;
      pitch += data[IDX_PITCH][i] * 1/52;
      rpm   += data[IDX_RPM][i];
      cnt++;
    }
    distance = delta_mm + distance;
  }


  return stateSpaceVec;
}


//
// マシンの回転半径のヒストグラムを生成
//
function createFeatureValueHistgram(resampled_data, unit, min_range, max_range, step)
{
  var distribution = new Array((max_range-min_range)/step);
  distribution.fill(0);

  for (var i=0 ; i<resampled_data[0].length ; ++i) {
    var degree = resampled_data[IDX_YAW][i] / 1000.0;
    if (Math.abs(degree) < 1) {
      continue;
    }
    
    var radius = (unit / (degree / 360)) / (2 * Math.PI);

    var bin = Math.round((radius - min_range) / step);
    if (bin >= distribution.length) {
      bin = distribution.length-1;
    }
    else if (bin < 0) {
      bin = 0;
    }
    distribution[bin]++;
  }

  //J ３レーン分のピークがどこかに立つはずなのでこれを探す
  //J ピークの位置と理論値の比を求める


  return distribution;
}

//
// 配列の中で最小の値の位置を返します。探索が配列の中央から始まるので、最小の値が複数ある場合には中央に近い部分が選ばれます
//
function searchMinIndex(arr)
{
  var min = Math.min(...arr);

  var minIndex = -1;
  var i = 0;
  var center_of_arr = Math.round(arr.length/2);
  for (i=0 ; i<center_of_arr ; ++i) {
    if ((center_of_arr + i < arr.length) && (min == arr[center_of_arr + i])) {
      return center_of_arr + i;
    }
    if ((center_of_arr - i >= 0) && (min == arr[center_of_arr - i])) {
      return center_of_arr - i;
    }
  }

  return -1;
}

//
// マシン回転半径によるラップの切り分け用の閾値を返します
//
function decideThresholdOfFeatures(distribution, step)
{
  //J 左回りの場合の中央値の間で最小の値を採用する
  var bin_center = Math.round(distribution.length/2 + (Jcjc_CenterRadius / step));
  var bin_outer  = Math.round(distribution.length/2 + (Jcjc_OuterRadius / step));
  var bin_inner  = Math.round(distribution.length/2 + (Jcjc_InnerRadius / step));

  var sub_arr = distribution.slice(bin_inner, bin_center);
  var threshold_inter_to_center_left = searchMinIndex(sub_arr) + bin_inner;

  sub_arr = distribution.slice(bin_center, bin_outer);
  var threshold_center_to_outer_left = searchMinIndex(sub_arr) + bin_center;

  //J 右回りの場合の中央値の間で最小の値を採用する
  var bin_center = Math.round(distribution.length/2 - (Jcjc_CenterRadius / step));
  var bin_outer  = Math.round(distribution.length/2 - (Jcjc_OuterRadius / step));
  var bin_inner  = Math.round(distribution.length/2 - (Jcjc_InnerRadius / step));

  var sub_arr = distribution.slice(bin_center, bin_inner);
  var threshold_inter_to_center_right = searchMinIndex(sub_arr) + bin_center;

  sub_arr = distribution.slice(bin_outer, bin_center);
  var threshold_center_to_outer_right = searchMinIndex(sub_arr) + bin_outer;

  //このリテラルの形をここで定義して本当にいいんだろうか
  var threshold = {
    inter_to_center_left:  (threshold_inter_to_center_left - Math.round(distribution.length/2))* step,
    center_to_outer_left:  (threshold_center_to_outer_left - Math.round(distribution.length/2))* step,
    inter_to_center_right: (threshold_inter_to_center_right - Math.round(distribution.length/2))* step,
    center_to_outer_right: (threshold_center_to_outer_right - Math.round(distribution.length/2))* step,
  }

  return threshold;
}


//
//J マップ情報と走行時のログから、各状態での平均速度、平均Dutyを算出します
//
function analizeDriveRecord(ssv, log, unit_mm, interval, wheelSize, threshold)
{
  var average_velocity = new Array(ssv.length);
  var average_duty = new Array(ssv.length);
  var data_cnt = new Array(ssv.length);

  for (var i=0 ; i<ssv.length ; ++i) {
    average_velocity[i] = 0.0;
    average_duty[i] = 0.0;
    data_cnt[i] = 0;
  }


  initializeSimulater(ssv, unit_mm, wheelSize, interval, threshold);

  for (var i=0 ; i<log[0].length ; ++i) {
    var position = updateSimulater(i, log);

    average_velocity[position]  += log[IDX_RPM][i];
    average_duty[position] += log[IDX_DUTY][i];
    data_cnt[position]++;
  }

  for (var i=0 ; i<ssv.length ; ++i) {
    if (data_cnt[i] == 0) {
      average_velocity[i] = NaN;
      average_duty[i]     = NaN;
      continue;
    }

    average_velocity[i]  /= data_cnt[i];
    var delta_mm = wheelSize * Math.PI * (average_velocity[i] / 60.0) * interval;
    average_velocity[i] = (delta_mm / (1000.0 * 1000.0)) / (interval / 3600.0);

    average_duty[i] /= data_cnt[i];
    average_duty[i] = 100.0 * (average_duty[i] / 255.0); 
  }

  return [average_velocity, average_duty];
}

