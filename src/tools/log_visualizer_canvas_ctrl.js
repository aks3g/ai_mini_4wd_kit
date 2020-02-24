/*
 * log_visualizer_canvas_ctrl.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
/*-----------------------------------------------------------------------------
 *J グラフのラベル
 */
var SensorName = ["Accel-X", "Accel-Y", "Accel-Z", "Pitch", "Roll", "Yaw", "rpm", "V-BATT", "I-MOT"];
var SensorUnit = ["mG", "mG", "mG", "mdegree/sec", "mdegree/sec", "mdegree/sec", "rpm", "mV", "mA"];
var ResampledSensorUnit = ["mG", "mG", "mG", "mdegree", "mdegree", "mdegree", "rpm", "mV", "mA"];


/*-----------------------------------------------------------------------------
 *J グラフ描画に必要な情報
 */
var GraphTypeLine = 0;
var GraphTypeBar  = 1;

var LineColors = ["maroon",    "darkblue",   "darkgreen",  "indigo",    "crimson",   "darkcyan" ]
var LineShadow = ["indianred", "dodgerblue", "aquamarine", "slateblue", "lightpink", "turquoise"]

var FeatureColorTable = ["#000000", "#ff0000", "#191970", "#0000cd", "#00bfff", "#2f4f4f", "#228b22", "#7fffd4", "#000000"]


/*-----------------------------------------------------------------------------
 *J センサデータの可視化処理
 */
//
// 選択されているセンサデータのみグラフとして描画する
//
function drawSensorDataGraph(canvas, data, xaxis_sub_ruled_interval, sensor_name, sensor_unit, selecter)
{
  // センサデータを選択するCheck Boxのうち、チェックされているものの数を調べる
  function checkValidSensorGraphNum(select_form)
  {
    var num = 0;
    for (var i=0 ; i<select_form.sensors.length ; ++i) {
      if (select_form.sensors[i].checked) {
        num++;
      }
    }

    return num;
  }


  if (data == NaN || data.length == 0) {
    return;
  }

  var numGraph = checkValidSensorGraphNum(selecter);
  var ctx2d = canvas.getContext("2d");

  var height = 150;
  var width = data[0].length ;

  var margin_left   = 50;
  var margin_right  = 10;
  var margin_top    = 5;
  var margin_bottom = 5;

  //J widthはデータ数に依存、heightはデータ種別数に依存
  canvas.setAttribute("width",  (width +  margin_left + margin_right).toString(10));
  canvas.setAttribute("height", ((numGraph * height) + ((margin_top+margin_bottom) * numGraph)).toString(10));

  //J 下地は透明色にする
  ctx2d.fillStyle = "rgba(0, 0, 0, 0)";
  ctx2d.fillRect(0, 0, canvas.clientWidth, canvas.clientHeight);

  //J グラフがない場合はここで終わる
  if (numGraph == 0) {
    return;
  }

  var i;
  var j = 0;
  for (i=0 ; i<9 ; ++i) {
    if (selecter.sensors[i].checked) {
      label = sensor_name[i] + "[" + sensor_unit[i] + "]";
      _drawGraph(GraphTypeLine, ctx2d, data[i], margin_left, j*(height + margin_top + margin_bottom) + margin_top, width, height, xaxis_sub_ruled_interval, 0, Math.min(...data[i]), Math.max(...data[i]), label);
      j++;
    }
  }
}

//
// 再サンプリングされたセンサデータの描画
//
function drawResampledDataGraph(canvas, data, xaxis_sub_ruled_interval, sensor_name, sensor_unit)
{
  drawSensorDataGraph(canvas, data, xaxis_sub_ruled_interval, sensor_name, sensor_unit, document.resampled_data_select_form);
}

//
// 再サンプリングされたセンサデータの再描画
//
function updateResampledDataGraph(canvas)
{
  drawResampledDataGraph(canvas, ResampledSensorData, 20, SensorName, ResampledSensorUnit);
}


//
// RAWセンサデータの描画
//
function drawRawDataGraph(canvas, data, xaxis_sub_ruled_interval, sensor_name, sensor_unit)
{
  drawSensorDataGraph(canvas, data, xaxis_sub_ruled_interval, sensor_name, sensor_unit, document.data_select_form);
}

//
// RAWセンサデータの再描画
//
function updateRawDataGraph(canvas)
{
  drawRawDataGraph(canvas, SensorData, 52*5, SensorName, SensorUnit);
}


/*-----------------------------------------------------------------------------
 * 回転数、加速度をソースとした移動距離と速度グラフの描画
 */
function drawVelocityAndOdometryGraph(canvas, velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel)
{
  if (velocityArr.length == 0) {
    return;
  }

  var ctx2d = canvas.getContext("2d");
  var graph_margin_y = 10;
  var graph_offset_x = 50
  var width = velocityArr.length;
  var height = 200;

  //J widthはデータ数に依存、heightはデータ種別数に依存
  canvas.setAttribute("width",  (width +  + graph_offset_x + 10).toString(10));
  canvas.setAttribute("height", (4 * height + graph_margin_y * 4).toString(10));

  //J 下地は透明色にする
  ctx2d.fillStyle = "rgba(0, 0, 0, 0)";
  ctx2d.fillRect(0, 0, canvas.clientWidth, canvas.clientHeight);


  //J タコメータベースの推定
  //J 速度グラフを表示
  var label = "Velocity[km/h]"
  _drawGraph(GraphTypeLine, ctx2d, velocityArr, graph_offset_x, 0*(height + graph_margin_y) + graph_margin_y/2, width, height, 52*5, 0, 0, Math.max(...velocityArr), label);

  //J 距離グラフを表示
  var label = "Odometry[mm]"
  _drawGraph(GraphTypeLine, ctx2d, odometryArr, graph_offset_x, 1*(height + graph_margin_y) + graph_margin_y/2, width, height, 52*5, 0, 0, Math.max(...odometryArr), label);


  //J 加速度センサベースの推定
  //J 速度グラフを表示
  var label = "Velocity[km/h] from accel"
  _drawGraph(GraphTypeLine, ctx2d, velocityArrFromAccel, graph_offset_x, 2*(height + graph_margin_y) + graph_margin_y/2, width, height, 52*5, 0, 0, Math.max(...velocityArrFromAccel), label);

  //J 距離グラフを表示
  var label = "Odometry[mm] from accel"
  _drawGraph(GraphTypeLine, ctx2d, odometryArrFromAccel, graph_offset_x, 3*(height + graph_margin_y) + graph_margin_y/2, width, height, 52*5, 0, 0, Math.max(...odometryArrFromAccel), label);
}

//
// 回転数、加速度をソースとした移動距離と速度グラフの再描画
//
function updateVelocityAndOdometryGraph(canvas)
{
  //J 再描画のためには推定からやり直す必要がある
  var [velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel] = estimateVelocityAndOdometory(SensorData);
  drawVelocityAndOdometryGraph(canvas, velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel);

}


/*-----------------------------------------------------------------------------
 * ミニ四駆の位置をXYでプロットする
 */
function drawEstimatedMachinePosition(canvas, clientWidth, xArr, yArr, lapArr)
{
  //J キャンパスのサイズを決定
  var x_range = (Math.max(...xArr) + (-1 * Math.min(...xArr))) * 1.05;
  var y_range = (Math.max(...yArr) + (-1 * Math.min(...yArr))) * 1.05;
//  var scale =  document.getElementById("sensor_visualizer").clientWidth / x_range;
  var scale =  clientWidth / x_range;

  canvas.setAttribute("width",  (x_range * scale).toString(10));
  canvas.setAttribute("height", (y_range * scale).toString(10));

  //J 原点を決定
  var x0 = - Math.min(...xArr) * 1.025;
  var y0 =   Math.max(...yArr) * 1.025;

  var ctx2d = canvas.getContext("2d");
  var lap = Math.max(...lapArr);
  //J XYのプロットを行う
  for (var i=0 ; i<=lap ; ++i) {
    head = searchHeadOfLap(i, lapArr);
    tail = searchTailOfLap(i, lapArr);
    if (head == NaN || tail == NaN) {
      break;
    }

    if (false == document.getElementById("lap"+i.toString()).checked) {
      continue;
    }

    //J 線の情報を更新
    ctx2d.strokeStyle = LineColors[i%6];
    ctx2d.shadowColor = LineColors[i%6];
    ctx2d.shadowBlur  = 5;

    ctx2d.beginPath();
    ctx2d.moveTo((x0 + xArr[head]) * scale, (y0 - yArr[head]) * scale);
    for (var j=head+1 ; j<=tail ; ++j) {
      var x = (x0 + xArr[j]) * scale;
      var y = (y0 - yArr[j]) * scale;
      ctx2d.lineTo(x, y);
    }
    ctx2d.stroke();

    ctx2d.strokeStyle="black";
    ctx2d.shadowBlur = 0;
  }

  //J 尺のリファレンスを置く
  var sizeOf500mm = scale * 500;
  ctx2d.beginPath();
  ctx2d.moveTo(x_range * scale - 20, y_range * scale - 20);
  ctx2d.lineTo(x_range * scale - 20 - sizeOf500mm, y_range * scale - 20);

  ctx2d.moveTo(x_range * scale - 20 - sizeOf500mm, y_range * scale - 10);
  ctx2d.lineTo(x_range * scale - 20 - sizeOf500mm, y_range * scale - 30);

  ctx2d.moveTo(x_range * scale - 20, y_range * scale - 10);
  ctx2d.lineTo(x_range * scale - 20, y_range * scale - 30);

  ctx2d.textAlign="center";
  ctx2d.strokeText("500mm", x_range * scale - 20 - sizeOf500mm/2, y_range * scale - 10);

  ctx2d.stroke();

}

/*-----------------------------------------------------------------------------
 * 状態空間の描画
 */
function drawStateSpaceVector(canvas, clientWidth, vec, position)
{
  //J XY座標を表す構造体的なもの
  Position = function(x, y) {
    this.x = x;
    this.y = y;
  }

  // キャンパス(w x h)内で、原点位置になる場所を求めます
  function originInCanvas2d(w, h, vec)
  {
    var min_x = 0;
    var max_x = 0;
    var min_y = 0;
    var max_y = 0;

    for (var i=0 ; i<vec.length ; ++i) {
      if (vec[i].x > max_x) {
        max_x = vec[i].x;
      }
      if (vec[i].x < min_x) {
        min_x = vec[i].x;
      }

      if (vec[i].y > max_y) {
        max_y = vec[i].y;
      }
      if (vec[i].y < min_y) {
        min_y = vec[i].y;
      }
    }

    var scale = w / (max_x - min_x);

    return new Position(Math.round(scale * -min_x), Math.round(scale * -min_y));
  }

  //J 領域サイズを指定するための構造体的なもの
  RectSize = function(w, h) {
    this.w = w;
    this.h = h;
  }

  //J 状態空間を描画するのに必要なサイズをmm単位で取得します
  function checkRectSize(vec)
  {
    var w, h;
    var min_x = 0;
    var max_x = 0;
    var min_y = 0;
    var max_y = 0;

    for (var i=0 ; i<vec.length ; ++i) {
      if (vec[i].x > max_x) {
        max_x = vec[i].x;
      }
      if (vec[i].x < min_x) {
        min_x = vec[i].x;
      }

      if (vec[i].y > max_y) {
        max_y = vec[i].y;
      }
      if (vec[i].y < min_y) {
        min_y = vec[i].y;
      }
    }

    return new RectSize(max_x-min_x, max_y-min_y);
  }



  var AlphaMap = ["ff", "ff", "ff"];
  if (0 <= position && position < vec.length) {
    var highlite_lap = vec[position].lap;

    AlphaMap = ["20", "20", "20"];
    AlphaMap[highlite_lap] = "ff";
  }

  //J mapのサイズを取得する
  var map_size = checkRectSize(vec);

  //J 様々な情報を描画するのに使うエリアをとっておく
  var margin_x = 50;
  var margin_y = 100;

  //J キャンパスのサイズを決定
  var scale = (clientWidth - margin_x) / map_size.w;

  var canvas_size = new RectSize(map_size.w * scale + margin_x, map_size.h * scale + margin_y);
  canvas.setAttribute("width",  (map_size.w * scale + margin_x).toString(10));
  canvas.setAttribute("height", (map_size.h * scale + margin_y).toString(10));

  //J キャンパス内での原点(0, 0) を決める
  var org = originInCanvas2d((map_size.w * scale), (map_size.h * scale), vec);
  org.x += margin_x/2;
  org.y += margin_y/2;

  var ctx2d = canvas.getContext("2d");
  ctx2d.fillStyle = "rgba(0, 0, 0, 0)";
  ctx2d.fillRect(0, 0, canvas.clientWidth, canvas.clientHeight);

  ctx2d.lineWidth = 1.5
  for (var i=0; i<vec.length ; ++i) {
    if (document.lane_select.lanes[vec[i].lap].checked == false) {
      continue;
    }

    ctx2d.beginPath()
    ctx2d.strokeStyle = FeatureColorTable[vec[i].feature] + AlphaMap[vec[i].lap];
    ctx2d.fillStyle = (i == position) ? "#ff1493ff" : "#00000000"; //TODO
    ctx2d.arc(vec[i].x * scale + org.x, canvas_size.h - (vec[i].y * scale + org.y), 10, 0, Math.PI*2, false);
    ctx2d.fill();
    ctx2d.stroke();
  }

  //J 尺のリファレンスを置く
  var sizeOf500mm = scale * 500;
  ctx2d.lineWidth = 1;
  ctx2d.strokeStyle = "#000000"
  ctx2d.beginPath();
  ctx2d.moveTo(canvas_size.w - 20, canvas_size.h - 20);
  ctx2d.lineTo(canvas_size.w - 20 - sizeOf500mm, canvas_size.h - 20);

  ctx2d.moveTo(canvas_size.w - 20 - sizeOf500mm, canvas_size.h - 10);
  ctx2d.lineTo(canvas_size.w - 20 - sizeOf500mm, canvas_size.h - 30);

  ctx2d.moveTo(canvas_size.w - 20, canvas_size.h - 10);
  ctx2d.lineTo(canvas_size.w - 20, canvas_size.h - 30);

  ctx2d.textAlign="center";
  ctx2d.strokeText("500mm", canvas_size.w - 20 - sizeOf500mm/2, canvas_size.h - 10);

  ctx2d.stroke();

  return;
}

/*-----------------------------------------------------------------------------
 * 観測された回転半径の頻度を表す棒グラフを描画
 */
function drawFeatureDistributionGraph(canvas, arr, x, y, label, min_range, max_range, step, threshold)
{
  var width = arr.length * 4;
  var height = 300;
  var scale = Math.max(...arr);

  var margin_x = 50;
  var margin_y = 100;

  canvas.setAttribute("width",  width + margin_x *2);
  canvas.setAttribute("height", height + margin_y);

  var ctx2d = canvas.getContext("2d");

  _drawGraph(GraphTypeBar, ctx2d, arr, x + margin_x, y + margin_y/2, width, height, step, 0, min_range, max_range, label)

//  drawBarGraphRuledLine(ctx2d, x + margin_x, y + margin_y/2, width, height, margin_y/2, label, min_range, max_range, step);
//  drawBarGraphBody(ctx2d, arr, x + margin_x, y + margin_y/2, width, height);

  //J リファレンスとなる中央、外周、内周の半径をプロットする
  var bin_width = 2;
  var bin_center = width * (Jcjc_CenterRadius-min_range) /(max_range - min_range)
  var bin_outer =  width * (Jcjc_OuterRadius -min_range) /(max_range - min_range)
  var bin_inner =  width * (Jcjc_InnerRadius -min_range) /(max_range - min_range)

  ctx2d.fillStyle = "blue";
  ctx2d.fillRect(x + margin_x + bin_center - bin_width/2,  y + margin_y/2, bin_width, height);
  ctx2d.fillRect(x + margin_x + bin_outer  - bin_width/2,  y + margin_y/2, bin_width, height);
  ctx2d.fillRect(x + margin_x + bin_inner  - bin_width/2,  y + margin_y/2, bin_width, height);

  ctx2d.beginPath();
  ctx2d.textAlign="center";
  ctx2d.strokeText("L/Center", x + margin_x + bin_center - bin_width/2, y + margin_y/3);
  ctx2d.strokeText("L/Outer",  x + margin_x + bin_outer  - bin_width/2, y + margin_y/3);
  ctx2d.strokeText("L/Inner",  x + margin_x + bin_inner  - bin_width/2, y + margin_y/3);
  ctx2d.stroke();

  bin_center = width * (-Jcjc_CenterRadius-min_range) /(max_range - min_range)
  bin_outer =  width * (-Jcjc_OuterRadius -min_range) /(max_range - min_range)
  bin_inner =  width * (-Jcjc_InnerRadius -min_range) /(max_range - min_range)

  ctx2d.fillStyle = "green";
  ctx2d.fillRect(x + margin_x + bin_center - bin_width/2,  y + margin_y/2, bin_width, height);
  ctx2d.fillRect(x + margin_x + bin_outer  - bin_width/2,  y + margin_y/2, bin_width, height);
  ctx2d.fillRect(x + margin_x + bin_inner  - bin_width/2,  y + margin_y/2, bin_width, height);

  ctx2d.beginPath();
  ctx2d.textAlign="center";
  ctx2d.strokeText("R/Center", x + margin_x + bin_center - bin_width/2, y + margin_y/3);
  ctx2d.strokeText("R/Outer",  x + margin_x + bin_outer  - bin_width/2, y + margin_y/3);
  ctx2d.strokeText("R/Inner",  x + margin_x + bin_inner  - bin_width/2, y + margin_y/3);
  ctx2d.stroke();

  //J 閾値を破線でプロットする
  segs = ctx2d.getLineDash()
  ctx2d.lineWidth = 2;
  ctx2d.setLineDash([6,3]);

  //J 左カーブ
  ctx2d.strokeStyle = "midnightblue"
  ctx2d.beginPath();

  bin_threshold = width * (threshold.inter_to_center_left - min_range) /(max_range - min_range)
  ctx2d.moveTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2);
  ctx2d.lineTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2 + height);

  bin_threshold = width * (threshold.center_to_outer_left - min_range) /(max_range - min_range)
  ctx2d.moveTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2);
  ctx2d.lineTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2 + height);

  ctx2d.stroke();

  //J 右カーブ
  ctx2d.strokeStyle = "darkslategray"
  ctx2d.beginPath();

  bin_threshold = width * (threshold.inter_to_center_right - min_range) /(max_range - min_range)
  ctx2d.moveTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2);
  ctx2d.lineTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2 + height);

  bin_threshold = width * (threshold.center_to_outer_right - min_range) /(max_range - min_range)
  ctx2d.moveTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2);
  ctx2d.lineTo(x + margin_x + bin_threshold - bin_width/2,  y + margin_y/2 + height);

  ctx2d.stroke();

  ctx2d.setLineDash(segs)
}



/*-----------------------------------------------------------------------------
 *J 存在確率分布グラフの描画
 */
function drawExistanceGraph(canvas, arr)
{
  var width = arr.length;
  var height = 300;

  var margin_x = 0;
  var margin_y = 50;

  if (width > document.getElementById("panel5").clientWidth) {
    margin_x = 50;
  }
  else {
    margin_x = (document.getElementById("panel5").clientWidth - width)/2;
  }


  canvas.setAttribute("width",  width + margin_x *2);
  canvas.setAttribute("height", height + margin_y);

  var ctx2d = canvas.getContext("2d");

  _drawGraph(GraphTypeLine, ctx2d, arr, margin_x, 0, width, height, 100, 0, 0, Math.max(...arr), "Existance");
}