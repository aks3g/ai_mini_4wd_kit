/*
 * strategy_editor_canvas_ctrl.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
/*-----------------------------------------------------------------------------
 *J グラフのラベル
 */
var TrainigSensorName   = ["Position", "Policy", "Updated Policy", "Velocity", "Accel-X", "Accel-Y", "Accel-Z", "Pitch", "Roll", "Yaw", "rpm", "V-BATT", "I-MOT"];
var TrainingSensorUnit  = ["-",        "km/h",     "km/h",     "km/h", "mG", "mG", "mG", "mdegree/sec", "mdegree/sec", "mdegree/sec", "rpm", "mV", "mA"];

/*-----------------------------------------------------------------------------
 *J グラフ描画に必要な情報
 */
var GraphTypeLine = 0;
var GraphTypeBar  = 1;

var LineColors = ["maroon",    "darkblue",   "darkgreen",  "indigo",    "crimson",   "darkcyan" ]
var LineShadow = ["indianred", "dodgerblue", "aquamarine", "slateblue", "lightpink", "turquoise"]

var FeatureColorTable = ["#000000", "#ff0000", "#191970", "#0000cd", "#00bfff", "#2f4f4f", "#228b22", "#7fffd4", "#000000"]


/*-----------------------------------------------------------------------------
 *J グラフ描画に必要な情報
 */
var FeatureColorTable = ["#ffffff", "#ff0000", "#8a2be2", "#0000cd", "#00bfff", "#adff2f", "#228b22", "#7fffd4", "#ffffff"]

/*-----------------------------------------------------------------------------
 *
 */
//J XY座標を表す構造体的なもの
Position = function(x, y) {
  this.x = x;
  this.y = y;
}

UsageGuide = function (name, color, shadow) {
  this.name = name;
  this.color = color;
  this.shadow = shadow;
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

//-----------------------------------------------------------------------------
// 選択されているセンサデータのみグラフとして描画する
//
function drawLabeledLineGraph(canvas, title, data, min_val, max_val, xaxis_sub_ruled_interval, yaxis_sub_ruled_interval, col, sha, label)
{
  var ctx2d = canvas.getContext("2d");

  var height = 500;
  var width = data[0].length ;

  var margin_left   = 50;
  var margin_right  = 10;
  var margin_top    = 5;
  var margin_bottom = 5;

  //J widthはデータ数に依存、heightはデータ種別数に依存
  canvas.setAttribute("width",  (width  + margin_left + margin_right ).toString(10));
  canvas.setAttribute("height", (height + margin_top  + margin_bottom).toString(10));

  //J 下地は透明色にする
  ctx2d.fillStyle = "rgba(0, 0, 0, 1)";
  ctx2d.fillRect(0, 0, canvas.clientWidth, canvas.clientHeight);

  _drawLineGraph(
    ctx2d, 
    data[0], 
    margin_left,
    margin_top,
    width,
    height,
    xaxis_sub_ruled_interval,
    yaxis_sub_ruled_interval,
    min_val,
    max_val,
    title,
    col[0],
    sha[0],
    "white"
    );

  for (let i=1 ; i<data.length ; ++i) {
    _drawLineGraphBody(
      ctx2d, 
      data[i], 
      margin_left,
      margin_top,
      width,
      height,
      min_val,
      max_val,
      col[i],
      sha[i],
      );
   }

  //J 各データのラベルを出す
  let line_h = 10
  let label_h = label.length * (line_h + margin_top) + margin_bottom
  let label_w = 0;

  for (let i=0 ; i<label.length ; ++i) {
    if (label_w < ctx2d.measureText(label[i]).width) {
      label_w = ctx2d.measureText(label[i]).width
    }
  }

  let bar_w = 20
  let margin_w = 5
  label_w = margin_w + bar_w + margin_w + label_w + margin_w

  let label_x = canvas.clientWidth - label_w - 3*margin_w
  let label_y = margin_bottom + 2.5 * margin_top

  ctx2d.fillStyle='black';
  ctx2d.fillRect(label_x, 2 * margin_top, label_w, label_h);

  ctx2d.beginPath();
  ctx2d.strokeStyle='white';
  ctx2d.lineWidth = 1;
  ctx2d.strokeRect(canvas.clientWidth - label_w - 3*margin_w, 2 * margin_top, label_w, label_h);
  ctx2d.stroke()

  ctx2d.fillStyle='white';
  for (let i=0 ; i<label.length ; ++i) {
    ctx2d.beginPath();
    ctx2d.strokeStyle=col[i];
    ctx2d.moveTo(label_x + margin_w,         i*(line_h + margin_top) + label_y)
    ctx2d.lineTo(label_x + margin_w + bar_w, i*(line_h + margin_top) + label_y)
    ctx2d.stroke()

    ctx2d.fillText(label[i], label_x + margin_w + bar_w + margin_w, i*(line_h + margin_top) + label_y + 3)

    console.log(label[i], col[i])

  }

  return
}

//
// RAWセンサデータの再描画
//
function updateRawDataGraph(raw_data_canvas, info_canvas, data)
{
// drawRawDataGraph(raw_data_canvas, data, 52*5, TrainigSensorName, TrainingSensorUnit);
  drawInitInfoLayerOfRawDataGraph(raw_data_canvas, info_canvas);
}

//
// Info Layerのサイズをグラフサイズに追従する
//
function drawInitInfoLayerOfRawDataGraph(raw_data_canvas, info_canvas)
{
  // Info Canvasのサイズを追従させる
  var w = raw_data_canvas.getAttribute("width");
  var h = raw_data_canvas.getAttribute("height");
  info_canvas.setAttribute("width", w);
  info_canvas.setAttribute("height", h);
}


//-----------------------------------------------------------------------------
//J 比較用ラインの描画
//
function drawHighLightLine(canvas, x)
{
  var h = canvas.getAttribute("height");
  var w = canvas.getAttribute("width");

  var margin_left   = 50;
  var margin_right  = 10;

  if (margin_left < x && x < (w - margin_right)) {
    _drawLine(canvas, x, 0, 0, h, "white");
  }
}

//
//J Raw Sensor Graphの座標からデータのIndexを逆参照する
//
function drawGetDataIndex(x, w)
{
  var margin_left 　= 50;
  var margin_right = 10;

  if (!(margin_left < x && x < (w - margin_right))) {
    return -1;
  }

  var data_idx = x - margin_left;

  return data_idx
}

//
//J Graphに補助線とLabelを描画
//
function drawDataPointerAndLabel(canvas, data, selecter, x)
{
  var height = 150;
  var margin_top = 5;
  var margin_bottom = 5;
  var w = canvas.getAttribute("width");

  var data_idx = drawGetDataIndex(x, w);
  if (data_idx < 0) return;

  var j = 0;
  var max_velocity = Math.max(Math.max(...data[1]), Math.max(...data[2]), Math.max(...data[3]))

  for (var i=0 ; i<4 ; ++i) {
    if (i == 0){
      var scale = height / (Math.max(...data[i]) - 0);
      var y = j*(height + margin_top + margin_bottom) + height - ((data[i][data_idx] - 0) * scale);
      _drawPointer(canvas, x, y, 5);
      _drawLabel(canvas, x+5, y, data[i][data_idx].toString(), "white")
      j++;
    }
    else {
      var scale = height / (max_velocity - 0)
      var y = j*(height + margin_top + margin_bottom) + height - ((data[i][data_idx] - 0) * scale);
      _drawPointer(canvas, x, y, 5);
      _drawLabel(canvas, x+5, y, data[i][data_idx].toString(), "white")
    }
  }
  j++;

  for (var i=0 ; i<9 ; ++i) {
    if (selecter.sensors[i].checked) {
      var scale = height / (Math.max(...data[i+4]) - Math.min(...data[i+4]))
      var y = j*(height + margin_top + margin_bottom) + height - ((data[i+4][data_idx] - Math.min(...data[i+4])) * scale);
      _drawPointer(canvas, x, y, 5);
      _drawLabel(canvas, x+5, y, data[i+4][data_idx].toString(), "white")
      j++;
    }
  }

}

//
//J 時間間隔を表示する
//
function drawTimeCursor(canvas, p1, p2, scale)
{
  var delta = Math.abs(p1-p2);
  var start = (p1 > p2) ? p2 : p1;

  _drawLine(canvas, start, 50, delta, 0, "white");

  _drawLine(canvas, start, 50, 5, 5, "white");
  _drawLine(canvas, start, 50, 5,-5, "white");

  _drawLine(canvas, start + delta, 50, -5, 5, "white");
  _drawLine(canvas, start + delta, 50, -5,-5, "white");

  var delta_time = Math.round((delta * scale) * Math.pow(10, 4) ) / Math.pow(10, 4); 

  _drawLabel(canvas, start + delta + 20, 50, delta_time.toString(), "white")
}

//-----------------------------------------------------------------------------
//J 対象のCanvasの内容をすべて消去
//
function drawClearAll(canvas)
{
  _drawClearAll(canvas);
}

