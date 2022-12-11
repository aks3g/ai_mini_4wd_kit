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
function _drawGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min, max, label, color="black")
{
  var scale = h / (max - min);
  var y0 = y + scale * max;

  ctx2d.beginPath();
  ctx2d.strokeStyle=color;
  ctx2d.fillStyle=color;

  ctx2d.lineWidth = 0.5;
  ctx2d.strokeRect(x, y, w, h);

  if (min < 0) {
    ctx2d.moveTo(x,   y0);
    ctx2d.lineTo(x+w, y0);
  }

  for (var i=sub_x + x ; i<w+x && sub_x!=0; i += sub_x) {
    ctx2d.moveTo(i, y);
    ctx2d.lineTo(i, y+h);
  }

  for (var i = 0 ; i*scale<h && sub_y != 0; i += sub_y) {
    ctx2d.moveTo(x,   h - i*scale + y);
    ctx2d.lineTo(x+w, h - i*scale + y);
  }
  ctx2d.stroke();
  ctx2d.lineWidth = 1.0;

  //J Rangeを出す
  ctx2d.textAlign="right";
  for (var i = 0 ; i*scale<h && sub_y!=0; i += sub_y) {
    ctx2d.fillText(Math.round(i+min).toString(), x - 10, h - i*scale + y);
  }

  ctx2d.fillText(Math.round(min).toString(), x - 10, y + h);
  if (min < 0) {
    ctx2d.fillText("0", x - 10, y0);
  }
  ctx2d.textAlign="start";

  //J ラベルを出す
  ctx2d.save()
  {
    //J 中心軸を文字中心に変更する
    ctx2d.translate(20, y + h/2);
    ctx2d.rotate(-90 * Math.PI/180);
    ctx2d.textAlign="center";
    ctx2d.fillText(label, 0, 0);
  }
  ctx2d.restore();
}


//
// 折れ線グラフ本体の描画
//
function _drawLineGraphBody(ctx2d, arr, x, y, w, h, min, max, lineColor="maroon", shadowColor="indianred")
{
  var scale = h / (max - min);

  var y0 = y + scale * max;
  var x0 = x;

  ctx2d.strokeStyle=lineColor;
  ctx2d.shadowBlur = 2;
  ctx2d.shadowColor = shadowColor
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
function _drawLineGraph(ctx2d, arr, x, y, w, h, sub_x, sub_y, min, max, label, lineColor="maroon", shadowColor="indianred", gridColor="black")
{
  _drawGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min, max, label, gridColor);
  _drawLineGraphBody(ctx2d, arr, x, y, w, h, min, max, lineColor, shadowColor)
}


//
// 棒グラフの罫線を描画
//
function _drawBarGraphRuledLine(ctx2d, x, y, w, h, sub_x, sub_y, min_range, max_range,label)
{
  var margin_y = 100;

  ctx2d.strokeStyle = "black";
  ctx2d.fillStyle = "black"
  ctx2d.beginPath()
  ctx2d.lineWidth = 1;
  ctx2d.strokeRect(x, y, w, h);

  ctx2d.textAlign="center";
  ctx2d.fillText(label, x+w/2, y+h + margin_y*2/3);

  ctx2d.stroke();

  var x0 = w * min_range/(min_range + max_range);

  //J 軸
  ctx2d.beginPath()
  for (var i = min_range ; i<=max_range ; i+=sub_x * 100) {
    var posX = w * (i - min_range)/(max_range - min_range) + x;
    ctx2d.moveTo(posX, y);
    ctx2d.lineTo(posX, y + h);

    ctx2d.textAlign="center";
    ctx2d.fillText(i.toString(), posX, y+h + margin_y/4);
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

//
function _drawLine(canvas, x, y, w, h, color="black")
{
  var ctx2d = canvas.getContext("2d");

  ctx2d.beginPath()
  {
    ctx2d.moveTo(x, y);
    ctx2d.lineTo(x+w, y+h);
  }
  ctx2d.strokeStyle = color
  ctx2d.stroke();
  ctx2d.strokeStyle = "black";
}

function _drawClearAll(canvas)
{
  var ctx2d = canvas.getContext("2d");

  ctx2d.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
}

function _drawPointer(canvas, x, y, r)
{
  var ctx2d = canvas.getContext("2d");

  ctx2d.beginPath();
  {
    ctx2d.fillStyle = "rgba(80,80,80,0.8)"
    ctx2d.strokeStyle = "rgba(255,255,255,0.8)"
    ctx2d.arc( x, y+r, r, 0, 2 * Math.PI, false) ;
  }
  ctx2d.stroke();
}

function _drawLabel(canvas, x, y, label, color="black")
{
  var ctx2d = canvas.getContext("2d");
  var w = ctx2d.measureText(label).width;
  ctx2d.beginPath();
  ctx2d.strokeStyle = color
  {
    ctx2d.fillStyle = "rgba(20,20,20,0.5)"
    ctx2d.fillRect(x-3, y+8, w+5, -20)

    ctx2d.textAlign="left";
    ctx2d.fillStyle=color
    ctx2d.fillText(label, x, y);
  }
  ctx2d.stroke();
  ctx2d.strokeStyle = "black";
}

function _drawUsageGuide(ctx, x, y, w, h, usageGuideList)
{
  // Calc Rect Size
  var margin = 10;
  var height = usageGuideList.length * 10 + 10;
  var width  = 0;
  var text_start = 0;

  ctx.beginPath();
  ctx.strokeStyle = "white"
  {
    for (var i=0 ; i<usageGuideList.length ; ++i) {
      width = (width < ctx.measureText(usageGuideList[i].name).width) ? ctx.measureText(usageGuideList[i].name).width : width;
    }
    text_start = width;
    width += margin + 50;

    // Draw Rect
    ctx.rect(x+w - width - margin, y+h - height - margin, width, height);
  }
  ctx.stroke();

  ctx.fillStyle="white";
  var pad_height = ctx.measureText(usageGuideList[0].name).actualBoundingBoxAscent + ctx.measureText(usageGuideList[0].name).actualBoundingBoxDescent ;
  for (var i=0 ; i<usageGuideList.length ; ++i) {
    ctx.fillText(usageGuideList[i].name, x+w - text_start - margin, y+h - height -margin + pad_height);

    var text_height = ctx.measureText(usageGuideList[i].name).actualBoundingBoxAscent + ctx.measureText(usageGuideList[i].name).actualBoundingBoxDescent;

    ctx.beginPath();
    ctx.strokeStyle = usageGuideList[i].color;
    ctx.shadowBlur = 2;
    ctx.shadowColor = usageGuideList[i].shadow
    ctx.moveTo(x+w - width - margin,      y+h - height -margin + pad_height - text_height/2);
    ctx.lineTo(x+w - width - margin + 50, y+h - height -margin + pad_height - text_height/2);
    ctx.stroke();

    pad_height += text_height + 3;
  }
  
  ctx.strokeStyle = "black";
}