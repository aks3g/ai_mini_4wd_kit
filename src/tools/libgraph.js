/*
 * libgraph.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
/*-----------------------------------------------------------------------------
 * グラフの描画処理
 *---------------------------------------------------------------------------*/
//
// 折れ線グラフの罫線を描画
//
function _drawGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min, max, label)
{
  var scale = h / (max - min);
  var y0 = y + scale * max;

  ctx2d.beginPath();

  ctx2d.lineWidth = 0.5;
  ctx2d.strokeRect(x, y, w, h);

  if (min < 0) {
    ctx2d.moveTo(x,   y0);
    ctx2d.lineTo(x+w, y0);
  }

  for (var i=sub_x + x ; i<w+x ; i += sub_x) {
    ctx2d.moveTo(i, y);
    ctx2d.lineTo(i, y+h);
  }
  ctx2d.stroke();
  ctx2d.lineWidth = 1.0;



  //J Rangeを出す
  ctx2d.textAlign="right";
  //J 暫定対策（恒久対策ってするんだろうか…）
  if (max >= 1) {
    ctx2d.strokeText(Math.round(max).toString(), x - 3, y + 10);
  }
  else {
    ctx2d.strokeText((Math.round(max*1000)/1000).toString(), x - 3, y + 10);
  }

  ctx2d.strokeText(Math.round(min).toString(), x - 10, y + h);
  if (min < 0) {
    ctx2d.strokeText("0", x - 10, y0);
  }
  ctx2d.textAlign="start";

  //J ラベルを出す
  ctx2d.save()
  {
    //J 中心軸を文字中心に変更する
    ctx2d.translate(20, y + h/2);
    ctx2d.rotate(-90 * Math.PI/180);
    ctx2d.textAlign="center";
    ctx2d.strokeText(label, 0, 0);
  }
  ctx2d.restore();
}


//
// 折れ線グラフ本体の描画
//
function _drawLineGraphBody(ctx2d, arr, x, y, w, h, min, max)
{
  var scale = h / (max - min);

  var y0 = y + scale * max;
  var x0 = x;

  ctx2d.strokeStyle="maroon";
  ctx2d.shadowBlur = 2;
  ctx2d.shadowColor = "indianred"
  ctx2d.beginPath();
  ctx2d.moveTo(x0, y0 - arr[0]*scale);

  var i=1;
  for (i=1 ; i<arr.length ; ++i) {
    ctx2d.lineTo(x0+i, y0 - arr[i]*scale);
  }
  ctx2d.stroke();
  ctx2d.strokeStyle="black";
  ctx2d.shadowBlur = 0;
}

//
// 折れ線グラフの描画
//
function _drawLineGraph(ctx2d, arr, x, y, w, h, sub_x, sub_y, min, max, label)
{
  _drawGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min, max, label);
  _drawLineGraphBody(ctx2d, arr, x, y, w, h, min, max)
}


//
// 棒グラフの罫線を描画
//
function _drawBarGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min_range, max_range,label)
{
  var margin_y = 100;

  ctx2d.strokeStyle = "black";
  ctx2d.beginPath()
  ctx2d.lineWidth = 1;
  ctx2d.strokeRect(x, y, w, h);

  ctx2d.textAlign="center";
  ctx2d.strokeText(label, x+w/2, y+h + margin_y*2/3);

  ctx2d.stroke();

  var x0 = w * min_range/(min_range + max_range);

  //J 軸
  ctx2d.beginPath()
  for (var i = min_range ; i<=max_range ; i+=sub_x * 100) {
    var posX = w * (i - min_range)/(max_range - min_range) + x;
    ctx2d.moveTo(posX, y);
    ctx2d.lineTo(posX, y + h);

    ctx2d.textAlign="center";
    ctx2d.strokeText(i.toString(), posX, y+h + margin_y/4);
  }
  ctx2d.stroke();
}

//
// 棒グラフ本体の描画
//
function _drawBarGraphBody(ctx2d, arr, x, y, w, h)
{
  var scale = h / Math.max(...arr);
  var bin_width = w / arr.length;

  ctx2d.fillStyle = "maroon";
  for (var i=0 ; i<arr.length ; ++i) {
    ctx2d.fillRect(x + i*bin_width - bin_width/2, y+h - scale*arr[i], bin_width, scale*arr[i]);
  }
}

//
// 棒グラフの描画
//
function _drawBarGraph(ctx2d, arr, x, y, w, h, sub_x, sub_y, min, max, label)
{
  _drawBarGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min, max, label);
  _drawBarGraphBody(ctx2d, arr, x, y, w, h);
}

//
// グラフ描画の一番入り口になる関数
//
function _drawGraph(graph_type, ctx2d, arr, x, y, w, h, sub_x, sub_y, min, max, label)
{
  if (graph_type == GraphTypeLine) {
    _drawLineGraph(ctx2d, arr, x, y, w, h, sub_x, sub_y, min, max, label)
  }
  else if (graph_type == GraphTypeBar) {
    _drawBarGraph(ctx2d, arr, x, y, w, h, sub_x, sub_y, min, max, label)
  }
}
