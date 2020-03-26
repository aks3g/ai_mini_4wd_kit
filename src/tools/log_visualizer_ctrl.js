/*
 * log_visualizer_ctrl.js
 *
 * Created: 2020/02/24
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
/*-------------------------------------------------------------------------------------------------
 * 各種変数/定数定義
 */
var SensorData = [];
var ResampledSensorData = [];
var TestData = [[], [], [], [], [], [], [], [], []];

var StateSpaceVec = [];
var ThresholdOfCurve = [];

var UNIT_mm = 100;

/*-----------------------------------------------------------------------------
 * HTML Form まわりの挙動を記載
 */
window.onload = function() {
  var canvas = document.getElementById("canvas_graph_raw");
  canvas.addEventListener('click', canvasOnClick, false);
}

//J Re-size時には描画をやり直す
window.onresize = function () {
}

//J Canvasクリック時の挙動を入れる（TODO）
function canvasOnClick(evt)
{
  //J Canvas上の点を求める
  var rect = evt.target.getBoundingClientRect();
  x = evt.clientX - rect.left;
  y = evt.clientY - rect.top;

  return;
}



//-----------------------------------------------------------------------------
//J Learning Dataボタンが押されたときの挙動
//
function onFileLoadButtonClick()
{
  document.getElementById("sensor_file").click();
  document.getElementById("sensor_file").addEventListener('change', handleSensorFileSelect, false);
}

//
//J File Selecterがクリックされたときの挙動
//
function handleSensorFileSelect(evt)
{
  var file = evt.target.files[0];

  //J ファイル名の表示
  sensor_file_name.innerHTML = file.name;

  //J ファイルの内容をパースする
  loadAndParseSensorFile(file, sensorDataUpdatedCallback);
}

//
//J Learining Dataの読み込み処理
//
function loadAndParseSensorFile(file, loaded_cb)
{
  var reader = new FileReader();

  //J ファイルの中身を読み込む
  reader.onload = function (evt) {
    var raw = reader.result;
    sensorData = [[],[],[],[],[],[],[],[],[],[]] //J 内容を破棄する

    //J 1行に区切って処理する
    var lines = raw.split(/\r\n|\r|\n/);
    lines.forEach( function (line) {
      //J Tab区切りの数字列なのでこれを処理する
      var values = line.split(/\t/)
      if (values.length >= 10) { 
        sensorData[IDX_AX].push(Number(values[IDX_AX+1]));
        sensorData[IDX_AY].push(Number(values[IDX_AY+1]));
        sensorData[IDX_AZ].push(Number(values[IDX_AZ+1]));
        sensorData[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
        sensorData[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
        sensorData[IDX_YAW].push(Number(values[IDX_YAW+1]));
        sensorData[IDX_RPM].push(Number(values[IDX_RPM+1]));
        sensorData[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
        sensorData[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
      }
    });

    //J 読み込んだデータ返す
    loaded_cb(sensorData);
  }

  reader.readAsText(file);
}


//
//J Learining Dataが読み込まれた後の処理
//
function sensorDataUpdatedCallback(sensorData)
{
  SensorData = sensorData;
  var wheelSize = Number(document.getElementById("wheel_size").value);

  //J 距離正規化
  ResampledSensorData = resampling(SensorData, wheelSize, UNIT_mm);
  //J 速度 - 距離推定
  var [velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel] = estimateVelocityAndOdometory(SensorData, wheelSize);
  //J 描画側にデータを渡す
  drawRawDataGraph(document.getElementById("canvas_graph_raw"), SensorData, 52*5, SensorName, SensorUnit);
  drawResampledDataGraph(document.getElementById("canvas_graph_resampling"), ResampledSensorData, 20, SensorName, ResampledSensorUnit);
  drawVelocityAndOdometryGraph(document.getElementById("canvas_graph_velocity"), velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel);

  //J 特徴量ヒストグラムと内周、中央周、外周を判断するための閾値を求める
  var distribution = createFeatureValueHistgram(ResampledSensorData,UNIT_mm, -800, 800, 10);
  ThresholdOfCurve = decideThresholdOfFeatures(distribution, 10);

  drawFeatureDistributionGraph(document.getElementById("canvas_graph_feature"), distribution, 0, 0, "Feature Value Distribution", -800, 800, 1, ThresholdOfCurve);


  //J XY座標でコースをプロットする
  var num_of_laps = estimateMachinePosition(SensorData, {left:1.0, right:1.0}, wheelSize);
  //J Lapの選択情報も一緒に出力する
  var selecter_html = "";
  for (var i=0 ; i<=num_of_laps ; ++i) {
    selecter_html += "<input type=\"checkbox\" id=\"lap" + i.toString() + "\" checked onChange=" + "\"updateEstimatedMachinePosition()\"" + ">Lap "+ i.toString()
  }
  lap_sel = document.getElementById("lap_selecter").innerHTML = selecter_html;

  drawEstimatedMachinePosition(document.getElementById("canvas_graph_xy_plot"), document.getElementById("sensor_visualizer").clientWidth, TracingContext.xArr, TracingContext.yArr, TracingContext.lapArr);

  //J 3周分のデータを利用して、状態空間を作り出す
  StateSpaceVec = createStateSpaceVector(SensorData, UNIT_mm, {left:1.0, right:1.0}, wheelSize, ThresholdOfCurve);

  drawStateSpaceVector(document.getElementById("canvas_graph_estimate_position"), document.getElementById("sensor_visualizer").clientWidth, StateSpaceVec, -1);
}

//
// ミニ四駆の位置をXYで再プロットする
//
function updateEstimatedMachinePosition(canvas, clientWidth)
{
  drawEstimatedMachinePosition(document.getElementById("canvas_graph_xy_plot"), document.getElementById("sensor_visualizer").clientWidth, TracingContext.xArr, TracingContext.yArr, TracingContext.lapArr);
}


//-----------------------------------------------------------------------------
//J Test DataTボタンが押されたときの挙動
//
function onTestFileLoadButtonClick()
{
  document.getElementById("test_sensor_file").click();
  document.getElementById("test_sensor_file").addEventListener('change', handleTestSensorFileSelect, false);
}

//
// Test File Selecterがクリックされたときの挙動
//
function handleTestSensorFileSelect(evt)
{
  var file = evt.target.files[0];

  //J ファイル名の表示
  test_sensor_file_name.innerHTML = file.name;

  //J ファイルの内容をパースする
 loadAndParseSensorFile(file, testDataUpdatedCallback);
}


//
//J Test Dataが読み込まらた後の処理
//
function testDataUpdatedCallback(test_data)
{
  TestData = test_data;

  document.getElementById("map_range").min = 0;
  document.getElementById("map_range").max = TestData[0].length;
}

//-----------------------------------------------------------------------------
//J ホイールの選択（プルダウンセレクト）が変更になった際の処理
//
function onWheelSelected()
{
  document.getElementById("wheel_size").value = document.getElementById("wheel_select").options[document.getElementById("wheel_select").selectedIndex].value;

  updateVelocityAndOdometryGraph(document.getElementById("canvas_graph_velocity"));
}

//
// ホイールサイズが直接入力されたとき
//
function onWheelSizeChanged()
{
  updateVelocityAndOdometryGraph(document.getElementById("canvas_graph_velocity"));
}

//-----------------------------------------------------------------------------
//J 一定時間ごとに実行されるシミュレータの処理
//
function onSimulaterTimerEvent()
{
  if (TestData[0].length == 0) {
    return;
  }

  var val = document.getElementById("map_range").value;
  val++;

  if (val < document.getElementById("map_range").max) {
    var position = updateSimulater(val, TestData);
    drawStateSpaceVector(document.getElementById("canvas_graph_estimate_position"), document.getElementById("sensor_visualizer").clientWidth, StateSpaceVec, position);
    drawExistanceGraph(document.getElementById("canvas_graph_existance"), getExistanceArray());

    document.getElementById("map_range").value = val;
  }
}


//-----------------------------------------------------------------------------
//J シミュレーションのスタート/ストップ
//
var TimerId = NaN;
function onStartButtonClick()
{
  if (TestData[0].length == 0) {
    return;
  }

  var wheelSize = Number(document.getElementById("wheel_size").value);

  if (isNaN(TimerId)) {
    initializeSimulater(StateSpaceVec, UNIT_mm,  wheelSize, 1/52.0, ThresholdOfCurve);
    TimerId = setInterval(onSimulaterTimerEvent, 1000/52);
    document.getElementById("start_btn").innerHTML = "STOP";
  }
  else {
    clearInterval(TimerId);
    TimerId = NaN
    document.getElementById("start_btn").innerHTML = "START";
  }
}

//
//J シミュレーションのリセット
//
function onResetButtonClick()
{
  clearInterval(TimerId);
  TimerId = NaN
  document.getElementById("start_btn").innerHTML = "START";
  document.getElementById("map_range").value = 0;
}


//-----------------------------------------------------------------------------
// カーブの補正係数のUpdate
//
function onCoeffUndated()
{
  document.getElementById("rightCoeffValue").innerHTML = document.getElementById("rightCoeff").value.toString();
  document.getElementById("leftCoeffValue").innerHTML  = document.getElementById("leftCoeff").value.toString();

  var coeff = {
    right : document.getElementById("rightCoeff").value,
    left  : document.getElementById("leftCoeff").value
  }

  var wheelSize = Number(document.getElementById("wheel_size").value);

  //J XY座標でコースをプロットする
  var num_of_laps = estimateMachinePosition(SensorData, coeff, wheelSize);
  //J Lapの選択情報も一緒に出力する
  var selecter_html = "";
  for (var i=0 ; i<=num_of_laps ; ++i) {
    selecter_html += "<input type=\"checkbox\" id=\"lap" + i.toString() + "\" checked onChange=" + "\"updateEstimatedMachinePosition()\"" + ">Lap "+ i.toString()
  }
  lap_sel = document.getElementById("lap_selecter").innerHTML = selecter_html;

  drawEstimatedMachinePosition(document.getElementById("canvas_graph_xy_plot"), document.getElementById("sensor_visualizer").clientWidth, TracingContext.xArr, TracingContext.yArr, TracingContext.lapArr);
  StateSpaceVec = createStateSpaceVector(SensorData, UNIT_mm, coeff, wheelSize, ThresholdOfCurve);
  updateStateSpaceVector();
}

//-----------------------------------------------------------------------------
// 特徴量マップの再描画
//
function updateStateSpaceVector()
{
  drawStateSpaceVector(document.getElementById("canvas_graph_estimate_position"), document.getElementById("sensor_visualizer").clientWidth, StateSpaceVec, -1);
}
