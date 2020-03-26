/*
 * strategy_editor_canvas_ctrl.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
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

//J RGBAの値からCSS形式のrgbaを出力する
function _rgba(R, G, B, A)
{
  return "#" + ('00' + R.toString(16)).slice(-2) + ('00' + G.toString(16)).slice(-2) + ('00' + B.toString(16)).slice(-2) + ('00' + A.toString(16)).slice(-2);
}

function _rgb(R, G, B)
{
  return "#" + ('00' + R.toString(16)).slice(-2) + ('00' + G.toString(16)).slice(-2) + ('00' + B.toString(16)).slice(-2);
}


function strategy2rgba(strategy, min=0, max=30, alpha="7F")
{
  var r = 0;
  var g = 0;
  var b = 0;

  if (strategy == NaN) {
    return  "#00000000";
  }

  // B - G - R の順で値が高くなる
  if (strategy > max) strategy = max;
  if (strategy < min) strategy = min;

  //J 512 stepで区切る
  var step = (max - min) / 512
  var position = (strategy - min) / (max - min)

  if (strategy == max) {
    r =  255;
    g =  255;
    b =  255;
  }
  else if (strategy  == min) {
    r =  40;
    g =  40;
    b =  40;
  } else if (position > 0.5) {
    position = (position - 0.5) * 2;
    r = Math.round(position * 255);
    g = 255 - r;
  }
  else {
    position = position * 2;
    g = Math.round(position * 255);
    b = 255 - g;
  }

  return _rgb(r, g, b) + alpha;
}


/*-----------------------------------------------------------------------------
 * 状態空間の描画
 */
function drawStateSpaceVector(canvas, clientWidth, clientHeight, vec, position, focused_lap)
{
  var AlphaMap = ["ff", "ff", "ff"];
  if (0 <= position && position < vec.length) {
    var highlite_lap = vec[position].lap;

    AlphaMap = ["20", "20", "20"];
    AlphaMap[highlite_lap] = "ff";
  }
  else if (focused_lap != NaN) {
    AlphaMap = ["20", "20", "20"];
    AlphaMap[focused_lap] = "ff";
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
    ctx2d.strokeStyle = FeatureColorTable[vec[i].feature] + AlphaMap[vec[i].lap];
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
  ctx2d.strokeText("500mm", canvas_size.w - 20 - sizeOf500mm/2, canvas_size.h - 10);

  ctx2d.stroke();

  return;
}

/*-----------------------------------------------------------------------------
 * Strategyをヒートマップで描画  0 - 40km/hの間をヒートマップで表現する
 */
function drawHeatMap(canvas, clientWidth, clientHeight, ssv, strategy, position, focused_lap, min, max, unit_str = "")
{
  var AlphaMap = ["ff", "ff", "ff"];
  if (0 <= position && position < ssv.length) {
    var highlite_lap = ssv[position].lap;

    AlphaMap = ["40", "40", "40"];
    AlphaMap[highlite_lap] = "7f";
  }
  else if (focused_lap != NaN) {
    AlphaMap = ["40", "40", "40"];
    AlphaMap[focused_lap] = "7f";
  }

  //J mapのサイズを取得する
  var map_size = checkRectSize(ssv);
  var margin_x = 50;
  var margin_y = 100;

  //J スケールの決定
  var scaleW = (clientWidth - margin_x)  / map_size.w;
  var scaleH = (clientHeight - margin_y) / map_size.h;

  var scale = (scaleW > scaleH) ? scaleH : scaleW;

  var canvas_size = new RectSize(map_size.w * scale + margin_x, map_size.h * scale + margin_y);
  canvas.setAttribute("width",  (map_size.w * scale + margin_x).toString(10));
  canvas.setAttribute("height", (map_size.h * scale + margin_y).toString(10));

  //J キャンパス内での原点(0, 0) を決める
  var org = originInCanvas2d((map_size.w * scale), (map_size.h * scale), ssv);
  org.x += margin_x/2;
  org.y += margin_y/2;

  var ctx2d = canvas.getContext("2d");
  ctx2d.fillStyle = "rgba(0, 0, 0, 0)";
  ctx2d.fillRect(0, 0, canvas.clientWidth, canvas.clientHeight);

  ctx2d.lineWidth = 1.5
  for (var i=0; i<ssv.length ; ++i) {
    ctx2d.beginPath()
    ctx2d.strokeStyle = "#00000000"
    ctx2d.fillStyle = strategy2rgba(strategy[i], min, max, AlphaMap[ssv[i].lap]);
    ctx2d.arc(ssv[i].x * scale + org.x, canvas_size.h - (ssv[i].y * scale + org.y), 10, 0, Math.PI*2, false);
    ctx2d.fill();
    ctx2d.stroke();
  }

  //J 左下にサンプルを出す
  ctx2d.lineWidth = 1.0;
　for (var i=0 ; i<200 ; ++i) {
    ctx2d.beginPath();
    ctx2d.strokeStyle = strategy2rgba(i, 0, 200, "FF");
    ctx2d.moveTo(i + margin_x, (map_size.h * scale + margin_y));
    ctx2d.lineTo(i + margin_x, (map_size.h * scale + margin_y) - 10);
    ctx2d.stroke();
  }

  ctx2d.lineWidth = 0.5;
  ctx2d.beginPath();
  ctx2d.strokeStyle = "#eeeeee"
  ctx2d.textAlign="center";
  ctx2d.strokeText(min.toString(), margin_x, (map_size.h * scale + margin_y) - 20);
  ctx2d.strokeText(max.toString() + " " +  unit_str, 200 + margin_x, (map_size.h * scale + margin_y) - 20);

  ctx2d.stroke();


  return;

}
