/*
 * strategy_editor_canvas_ctrl.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
/*-----------------------------------------------------------------------------
 *J グラフのラベル
 */
var TrainigSensorName   = ["Position", "-", "Policy", "Updated Policy", "Velocity", "Accel-X", "Accel-Y", "Accel-Z", "Pitch", "Roll", "Yaw", "rpm", "V-BATT", "I-MOT"];
var TrainingSensorUnit  = ["-",      , "-", "km/h",     "km/h",     "km/h", "mG", "mG", "mG", "mdegree/sec", "mdegree/sec", "mdegree/sec", "rpm", "mV", "mA"];

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
var FeatureColorTable5 = ["#000000", "#ff0000", "#000080", "#0000cd", "#1e90ff", "#87cefa", "#b0e0e6", "#006400", "#008000", "#2e8b57", "#66cdaa", "#98fb98", "#ffffff"]

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

/*-----------------------------------------------------------------------------
 * 状態空間の描画
 */
function drawStateSpaceVector(canvas, clientWidth, clientHeight, vec, position, focused_lap, lanes)
{
  var AlphaMap = ["ff", "ff", "ff", "ff", "ff", "ff", "ff", "ff", "ff", "ff"];
  if (0 <= position && position < vec.length) {
    var highlite_lap = vec[position].lap;

    AlphaMap = ["20", "20", "20", "20", "20", "20", "20", "20", "20", "20"];
    AlphaMap[highlite_lap] = "ff";
  }
  else if (focused_lap != NaN) {
    AlphaMap = ["20", "20", "20", "20", "20", "20", "20", "20", "20", "20"];
    AlphaMap[focused_lap] = "ff";
  }

  //J palet を用意する
  var palette = FeatureColorTable;
  if (lanes != 3) {
    console.log('5 lanes')
    palette = FeatureColorTable5;
  }

  //J mapのサイズを取得する
  var map_size = checkRectSize(vec);
  var margin_x = 50;
  var margin_y = 100;

  //J キャンパスのサイズを決定
  var scaleW = (clientWidth - margin_x)  / map_size.w;
  var scaleH = (clientHeight - margin_y) / map_size.h;

  var scale = (scaleW > scaleH) ? scaleH : scaleW;

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
    ctx2d.beginPath()
    ctx2d.strokeStyle = palette[vec[i].feature] + AlphaMap[vec[i].lap];
    ctx2d.fillStyle = (i == position) ? "#ff00ffff" : "#00000000"; //TODO
    ctx2d.arc(vec[i].x * scale + org.x, canvas_size.h - (vec[i].y * scale + org.y), 10, 0, Math.PI*2, false);
    ctx2d.fill();
    ctx2d.stroke();
  }

  //J 尺のリファレンスを置く
  var sizeOf500mm = scale * 500;
  ctx2d.lineWidth = 1;
  ctx2d.strokeStyle = "#eeeeee"
  ctx2d.beginPath();
  ctx2d.moveTo(canvas_size.w - 20, canvas_size.h - 20);
  ctx2d.lineTo(canvas_size.w - 20 - sizeOf500mm, canvas_size.h - 20);

  ctx2d.moveTo(canvas_size.w - 20 - sizeOf500mm, canvas_size.h - 10);
  ctx2d.lineTo(canvas_size.w - 20 - sizeOf500mm, canvas_size.h - 30);

  ctx2d.moveTo(canvas_size.w - 20, canvas_size.h - 10);
  ctx2d.lineTo(canvas_size.w - 20, canvas_size.h - 30);

  ctx2d.textAlign="center";
  ctx2d.fillStyle="white";
  ctx2d.fillText("500mm", canvas_size.w - 20 - sizeOf500mm/2, canvas_size.h - 10);

  ctx2d.stroke();

  return;
}

function   _drawEvaluationValue(ctx2d, data, x, y, w, h)
{
  console.log(data) 
  ctx2d.lineWidht = 1;
  ctx2d.strokeStyle="#ff0000"

  for (var i=0 ; i<data.length ; ++i) {
    if (data[i] != 0) {
      ctx2d.beginPath();
      ctx2d.moveTo(x+i, y)
      ctx2d.lineTo(x+i, y+h);
      ctx2d.stroke();
    }
  }
}


//-----------------------------------------------------------------------------
// 選択されているセンサデータのみグラフとして描画する
//
function drawSensorDataGraph(canvas, data, xaxis_sub_ruled_interval, sensor_name, sensor_unit, selecter)
{
  // センサデータを選択するCheck Boxのうち、チェックされているものの数を調べる
  // numは最低2個
  function checkValidSensorGraphNum(select_form)
  {
    var num = 2;
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

  var max_velocity = Math.max(Math.max(...data[1]), Math.max(...data[2]), Math.max(...data[3]))
  var j = 0;

  // Position with Evaluation value
  label = sensor_name[0] + "[" + sensor_unit[0] + "]";
  _drawLineGraph(
   ctx2d, 
    data[0], 
    margin_left,
    j*(height + margin_top + margin_bottom) + margin_top,
    width,
    height,
    xaxis_sub_ruled_interval,
    0,
    0,
    Math.max(...data[0]),
    label,
    "gold",
    "khaki",
    "white"
    );
  _drawEvaluationValue(
    ctx2d,
    data[1],
    margin_left,
    j*(height + margin_top + margin_bottom) + margin_top,
    width,
    height
  )

  j++;

  // Policy
  label = sensor_name[2] + "[" + sensor_unit[2] + "]";

  _drawLineGraph(
    ctx2d, 
    data[2], 
    margin_left,
    j*(height + margin_top + margin_bottom) + margin_top,
    width,
    height,
    xaxis_sub_ruled_interval,
    0,
    0,
    max_velocity,
    label,
    "gold",
    "khaki",
    "white"
    );

//j++;

  // Next Policy
  _drawLineGraph(
    ctx2d, 
    data[3], 
    margin_left,
    j*(height + margin_top + margin_bottom) + margin_top,
    width,
    height,
    xaxis_sub_ruled_interval,
    0,
    0,
    max_velocity,
    "",
    "orangered",
    "khaki",
    "white"
    );
// j++;

  // Velocity
  _drawLineGraph(
    ctx2d, 
    data[4], 
    margin_left,
    j*(height + margin_top + margin_bottom) + margin_top,
    width,
    height,
    xaxis_sub_ruled_interval,
    0,
    0,
    max_velocity,
    "",
    "lawngreen",
    "khaki",
    "white"
    );

  var usageGuideList = [new UsageGuide(sensor_name[2], "orangered", "khaki"), new UsageGuide(sensor_name[3], "lawngreen", "khaki"), new UsageGuide(sensor_name[4], "gold", "khaki")]
  _drawUsageGuide(ctx2d, margin_left, j*(height + margin_top + margin_bottom) + margin_top, width, height, usageGuideList);

  j++;

  for (var i=0 ; i<9 ; ++i) {
    if (selecter.sensors[i].checked) {
      label = sensor_name[i+5] + "[" + sensor_unit[i+6] + "]";
      _drawLineGraph(
        ctx2d, 
        data[i+5], 
        margin_left,
        j*(height + margin_top + margin_bottom) + margin_top,
        width,
        height,
        xaxis_sub_ruled_interval,
        0,
        Math.min(...data[i+5]),
        Math.max(...data[i+5]),
        label,
        "gold",
        "khaki",
        "white"
        );
      j++;
    }
  }
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
function updateRawDataGraph(raw_data_canvas, info_canvas, data)
{
  drawRawDataGraph(raw_data_canvas, data, 52*5, TrainigSensorName, TrainingSensorUnit);
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
      var scale = height / (Math.max(...data[i+5]) - Math.min(...data[i+5]))
      var y = j*(height + margin_top + margin_bottom) + height - ((data[i+5][data_idx] - Math.min(...data[i+5])) * scale);
      _drawPointer(canvas, x, y, 5);
      _drawLabel(canvas, x+5, y, data[i+5][data_idx].toString(), "white")
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

