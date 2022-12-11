/*
 * strategy_editor_ctrl.js
 *
 * Created: 2020/03/04
 * Copyright 2020 Kiyotaka Akasaka. All rights reserved.
 */
/*-------------------------------------------------------------------------------------------------
 * 各種変数/定数定義
 */
var StateSpaceVec = [];
var ThresholdOfCurve = [];

var UNIT_mm = 100;

var TrialDataList = [];
TrialDataInfo = function(id, filename, policy, next_policy, velocity, sensorData) {
  this.id = id;
  this.filename = filename;
  this.sensorData = sensorData;

  var _policy = []
  var _next_policy = []
  for (var i=0 ; i<sensorData[IDX_POS].length ; ++i) {
    _policy.push(policy[sensorData[IDX_POS][i]]);
    _next_policy.push(next_policy[sensorData[IDX_POS][i]])
  }
  this.data = [];
  this.data.push(sensorData[IDX_POS])
  this.data.push(sensorData[IDX_EVA])
  this.data.push(_policy)
  this.data.push(_next_policy)
  this.data.push(velocity)

  for (var i=0 ; i<9 ; ++i) {
    this.data.push(sensorData[i])
  }
}


/*-----------------------------------------------------------------------------
 * HTML Form まわりの挙動を記載
 */
var AuxiliaryLines = [];

window.onload = function() {
  var canvas = document.getElementById("extra_info_canvas");
  canvas.addEventListener('click', canvasOnClick, false);
  canvas.addEventListener('dblclick', canvasOnDoubleClick, false);
  canvas.addEventListener('mousemove', canvasOnMove, false);
  canvas.addEventListener('mouseleave', canvasOnLeave, false);
}

//J Re-size時には描画をやり直す
window.onresize = function () {
  updateStateSpaceVector();
}

//J Canvasクリック時の挙動を入れる（TODO）
function canvasOnClick(evt)
{
  if (TrialDataList.length == 0 || document.getElementById("trial_data_list").value == "") {
    return;
  }

  //J Canvas上の点を求める
  var rect = evt.target.getBoundingClientRect();
  x = Math.floor((evt.clientX - rect.left) / (evt.target.clientWidth / evt.target.width));

  AuxiliaryLines.push(x)

  return;
}

//J Canvas右クリック時の挙動を入れる（TODO）
function canvasOnDoubleClick(evt)
{
  if (TrialDataList.length == 0 || document.getElementById("trial_data_list").value == "") {
    return;
  }

  AuxiliaryLines = []
  drawClearAll(evt.target)

  return;
}

//J Canvas上のマウス移動
function canvasOnMove(evt)
{
  if (TrialDataList.length == 0 || document.getElementById("trial_data_list").value == "") {
    return;
  }

  //J Canvas上の点を求める
  var rect = evt.target.getBoundingClientRect();
  x = Math.floor((evt.clientX - rect.left) / (evt.target.clientWidth / evt.target.width));

  drawClearAll(evt.target);
  drawHighLightLine(evt.target, x);

  //J 参照線とグラフとの交点にポインタを置く
  drawDataPointerAndLabel(evt.target, TrialDataList[document.getElementById("trial_data_list").value].data, document.data_select_form, x);

  //J 記録済みの点も打つ
  AuxiliaryLines.forEach( function(x) {
    drawHighLightLine(evt.target, x);
    drawDataPointerAndLabel(evt.target, TrialDataList[document.getElementById("trial_data_list").value].data, document.data_select_form, x);
  });

  //J 記録済みの点が2点以上あれば、2点間の時間を計測する
  if (AuxiliaryLines.length >= 2) {
    drawTimeCursor(evt.target, AuxiliaryLines[0], AuxiliaryLines[1], 1.0/52.0);
  }

  //J MinimapのUpdate
  data_idx = drawGetDataIndex(x, evt.target.getAttribute("width"));
  updateStateSpaceVector(TrialDataList[document.getElementById("trial_data_list").value].data[0][data_idx]);//TODO 0

  return;
}

//J Canvas上からのマウス離脱
function canvasOnLeave(evt) {
  drawClearAll(evt.target)

  AuxiliaryLines.forEach( function(x) {
    drawHighLightLine(evt.target, x);
    drawDataPointerAndLabel(evt.target, TrialDataList[document.getElementById("trial_data_list").value].data, document.data_select_form, x);
  });

  //J 記録済みの点が2点以上あれば、2点間の時間を計測する
  if (AuxiliaryLines.length >= 2) {
    drawTimeCursor(evt.target, AuxiliaryLines[0], AuxiliaryLines[1], 1.0/52.0);
  }

}


//-----------------------------------------------------------------------------
// HTML UI Callbacks

//
//J Learning / Mapボタンが押されたときの挙動
//
function onMapFileLoadButtonClick()
{
  document.getElementById("sensor_file").click();
  document.getElementById("sensor_file").addEventListener('change', handleMapFileSelect, false);
}

//
//J Save Map ボタンが押されたときの挙動
//
function onSaveMapButtonClick()
{
  if (StateSpaceVec.length == 0) {
    return;
  }

  saveStringAsFile("map.txt", createStateSpaceVectorString(StateSpaceVec));
}

//
//J Simulatorで使用するデータを保存する
//
function onCreateSimDataButtonClick()
{
  if (document.getElementById("trial_data_list").value == "") {
    return;
  }

  saveStringAsFile("sim_data.txt", createResultData(TrialDataList[document.getElementById("trial_data_list").value]));
//  saveStringAsFile("sim_data.txt", createDatasetForSimulator(TrialDataList[document.getElementById("trial_data_list").value].sensorData));
}

//
//J 現在登録されているデータをプロジェクトファイルとして保存
//
function onSaveProjectButtonClick()
{
  
}

//
//J Add Trial Data ボタンが押されたときの挙動
//
function onAddTrialDataButtonClick()
{
  document.getElementById("trial_data_file").click();
  document.getElementById("trial_data_file").addEventListener('change', handleTrialDataFileSelect, false);
}

//
//J ホイールの選択（プルダウンセレクト）が変更になった際の処理
//
function onWheelSelected()
{
  document.getElementById("wheel_size").value = document.getElementById("wheel_select").options[document.getElementById("wheel_select").selectedIndex].value;
}

//
// 試行データの変更
//
function onTrialDataSelecterChanged(select)
{
  var raw_data_canvas = document.getElementById('raw_graph_canvas');
  var info_canvas     = document.getElementById('extra_info_canvas');

  updateRawDataGraph(raw_data_canvas, info_canvas, TrialDataList[document.getElementById("trial_data_list").value].data);
}

//-----------------------------------------------------------------------------
//J Learning/Map File Selecterがクリックされたときの挙動
//
function handleMapFileSelect(evt)
{
  var file = evt.target.files[0];

  //J ファイル名の表示
  sensor_file_name.innerHTML = file.name;

  //J ファイルの内容をパースする
  loadAndParseMapFile(file);
}

//
//J Map Dataの読み込み処理
//
function loadAndParseMapFile(file)
{
  var reader = new FileReader();

  //J ファイルの中身を読み込む
  reader.onload = function (evt) {
    var raw = reader.result;
    sensorData = [[],[],[],[],[],[],[],[],[],[],[]] //J 内容を破棄する

    //J 1行に区切って処理する
    var lines = raw.split(/\r\n|\r|\n/);
    //J 1行目の内容が、# SSVであれば、生データではない
    if (lines[0].indexOf("# SSV") == 0) {
      parseSsvFile(lines);
    }
    else {
      sensorData = parseSensorFile(lines)
      mapUpdatedCallback(sensorData);
    }
    return;
  }

  reader.readAsText(file);
}

//
//J 生のマップ獲得時のMap生成用データを読み込み
//
function parseSensorFile(lines)
{
  var sensor_data = [[],[],[],[],[],[],[],[],[],[],[]]

  lines.forEach( function (line) {
    //J Tab区切りの数字列なのでこれを処理する
    var values = line.split(/\t/)
    if (values.length >= 11) { 
      sensor_data[IDX_AX].push(Number(values[IDX_AX+1]));
      sensor_data[IDX_AY].push(Number(values[IDX_AY+1]));
      sensor_data[IDX_AZ].push(Number(values[IDX_AZ+1]));
      sensor_data[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
      sensor_data[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
      sensor_data[IDX_YAW].push(Number(values[IDX_YAW+1]));
      sensor_data[IDX_RPM].push(Number(values[IDX_RPM+1]));
      sensor_data[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
      sensor_data[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
      sensor_data[IDX_DUTY].push(Number(values[IDX_DUTY+1]));
    }
    else if (values.length >= 10) { 
      sensor_data[IDX_AX].push(Number(values[IDX_AX+1]));
      sensor_data[IDX_AY].push(Number(values[IDX_AY+1]));
      sensor_data[IDX_AZ].push(Number(values[IDX_AZ+1]));
      sensor_data[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
      sensor_data[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
      sensor_data[IDX_YAW].push(Number(values[IDX_YAW+1]));
      sensor_data[IDX_RPM].push(Number(values[IDX_RPM+1]));
      sensor_data[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
      sensor_data[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
    }
  }
  );

  return sensor_data;
}

//
//J 処理済みのSSVの読み込み
//
function parseSsvFile(lines)
{
  StateSpaceVec = [];

  var arr_f = [];
  var arr_x = [];
  var arr_y = [];
  var arr_l = [];

  lines.forEach( function (line) {
    //J strategy は arr_policyという名前のPython形式の配列として記述されている
    if (line.indexOf('arr_s') == 0) {
      //J []で囲まれた範囲を切り出す
      var begin = line.indexOf('[')+1;
      var end = line.indexOf(']');
      var arr_str = line.substring(begin, end).split(/,/);
      arr_str.forEach(function (v) {
        arr_f.push(Number(v));
      });
    }
    else if (line.indexOf('arr_x') == 0) {
      var begin = line.indexOf('[')+1;
      var end = line.indexOf(']');
      var arr_str = line.substring(begin, end).split(/,/);
      arr_str.forEach(function (v) {
        arr_x.push(Number(v));
      });
    }
    else if (line.indexOf('arr_y') == 0) {
      var begin = line.indexOf('[')+1;
      var end = line.indexOf(']');
      var arr_str = line.substring(begin, end).split(/,/);
      arr_str.forEach(function (v) {
        arr_y.push(Number(v));
      });
    }
    else if (line.indexOf('arr_l') == 0) {
      var begin = line.indexOf('[')+1;
      var end = line.indexOf(']');
      var arr_str = line.substring(begin, end).split(/,/);
      arr_str.forEach(function (v) {
        arr_l.push(Number(v));
      });
    }
    else if (line.indexOf('curve_threshold') == 0) {
     ThresholdOfCurve = [];

      var begin = line.indexOf('[')+1;
      var end = line.indexOf(']');
      var arr_str = line.substring(begin, end).split(/,/);

      ThresholdOfCurve = {
        inter_to_center_left  : Number(arr_str[0]),
        center_to_outer_left  : Number(arr_str[1]),
        inter_to_center_right : Number(arr_str[2]),
        center_to_outer_right : Number(arr_str[3])
      }
    }
  });

  for (var i=0 ; i<arr_f.length ; ++i) {
    StateSpaceVec.push(new StateInfo(arr_x[i], arr_y[i], arr_l[i], arr_f[i]));
  }

  updateStateSpaceVector(-1);

  return;
}


//
//J Map Dataが読み込まれた後の処理
//
function mapUpdatedCallback(sensorData)
{
  var wheelSize = document.getElementById("wheel_size").value;

  //J 距離正規化
  var resampledSensorData = resampling(sensorData, wheelSize, UNIT_mm);
  //J 速度 - 距離推定
  var [velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel] = estimateVelocityAndOdometory(sensorData, wheelSize);

  //J 特徴量ヒストグラムと内周、中央周、外周を判断するための閾値を求める
  var is5lane = document.getElementById("five_lane").checked;
  if (is5lane) {
    var lanes = 5;
    var laps  = 10;
    var distribution = createFeatureValueHistgram(resampledSensorData,UNIT_mm, -1000, 1000, 10);
    ThresholdOfCurve = decideThresholdOfFeatures5(distribution, 10);
  }
  else {
    var lanes = 3;
    var laps  = 3;
    var distribution = createFeatureValueHistgram(resampledSensorData,UNIT_mm, -800, 800, 10);
    ThresholdOfCurve = decideThresholdOfFeatures(distribution, 10);
  }

  //J 3周分のデータを利用して、状態空間を作り出す
  StateSpaceVec = createStateSpaceVector(sensorData, UNIT_mm, {left:1.0, right:0.98}, wheelSize, ThresholdOfCurve, lanes, laps);
  updateStateSpaceVector(-1);

  return;
}


//-----------------------------------------------------------------------------
//J 文字列をファイル名を指定して保存（ダウンロード）させます
//
function saveStringAsFile(filename, save_string)
{
  var stream = stringToStream(save_string)
  var download_link = document.createElement("a");
  download_link.href = URL.createObjectURL(
    new Blob([stream.subarray(0, stream.length)], { type: "text/plain" })
  );
  
  download_link.download = filename;
  download_link.style.display = 'none';

  document.body.appendChild(download_link) //  FireFox specification
  download_link.click();
  document.body.removeChild(download_link) //  FireFox specification
}

//
//J 文字列からバイトストリームを生成
//
function stringToStream(str)
{
  //J Byte Streamに変換
  var len = str.length;
  var arr = new Uint8Array(len);
  for (var i = 0; i < len; i++) {
    arr[i] = str[i].charCodeAt(0);
  }

  return arr;
}

//
//J 特徴量マップを文字列化
//
function createStateSpaceVectorString(ssv)
{
  var str_s = "arr_s = [";
  var str_x = "arr_x = [";
  var str_y = "arr_y = [";
  var str_l = "arr_l = [";

  for (var i=0 ; i<ssv.length ; ++i) {
    if (i != 0) {
      str_s += ",";
      str_x += ",";
      str_y += ",";
      str_l += ",";
    }
    str_s += ssv[i].feature.toString();
    str_x += ssv[i].x.toString();
    str_y += ssv[i].y.toString();
    str_l += ssv[i].lap.toString();
  }
  str_s += "]\n";
  str_x += "]\n";
  str_y += "]\n";
  str_l += "]\n";

  if (document.getElementById("five_lane").checked) {
    var str_curve = "curve_threshold = [";
    str_curve += ThresholdOfCurve.c1_to_c2_left.toString() + ",";
    str_curve += ThresholdOfCurve.c2_to_c3_left.toString() + ",";
    str_curve += ThresholdOfCurve.c3_to_c4_left.toString() + ",";
    str_curve += ThresholdOfCurve.c4_to_c5_left.toString() + ",";
    str_curve += ThresholdOfCurve.c1_to_c2_right.toString() + ",";
    str_curve += ThresholdOfCurve.c2_to_c3_right.toString() + ",";
    str_curve += ThresholdOfCurve.c3_to_c4_right.toString() + ",";
    str_curve += ThresholdOfCurve.c4_to_c5_right.toString();
    str_curve += "]\n";
  }
  else {
    var str_curve = "curve_threshold = [";
    str_curve += ThresholdOfCurve.inter_to_center_left.toString() + ",";
    str_curve += ThresholdOfCurve.center_to_outer_left.toString() + ",";
    str_curve += ThresholdOfCurve.inter_to_center_right.toString() + ",";
    str_curve += ThresholdOfCurve.center_to_outer_right.toString();
    str_curve += "]\n";
  }


  return "# SSV\n" + str_s + str_x + str_y + str_l + str_curve;
}

function createResultData(trialDataInfo)
{
  var str=""

  for (var i=0 ; i<trialDataInfo.data[0].length ; ++i) {
    evt = (Math.abs(trialDataInfo.sensorData[IDX_ROLL][i]) > 100000 || trialDataInfo.sensorData[IDX_AZ][i] > 8000) ? 1 : 0;

    str += trialDataInfo.sensorData[IDX_DUTY][i].toString() + "\t" +
           trialDataInfo.data[0][i].toString() + "\t" +
           evt.toString() + "\t" +
           trialDataInfo.data[3][i].toString() + "\n"
  }

  return str
}
//
//J Simulatorで使用するデータをPythonのArray形式の文字列で出力
//
function createDatasetForSimulator(sensorData)
{
  var wheelSize = document.getElementById("wheel_size").value;

  var str_rpm   = "arr_rpm = [";
  var str_yaw   = "arr_yaw = [";
  var str_roll  = "arr_roll = [";
  var str_pitch = "arr_pitch = [";
  var str_v     = "arr_v = [";

  var sep = ","
  for (var i=0 ; i<sensorData[0].length ; ++i) {
    if (i != 0) {
      str_rpm   += ",";
      str_yaw   += ",";
      str_roll  += ",";
      str_pitch += ",";
      str_v     += ",";
    }
    str_rpm   += sensorData[IDX_RPM][i].toString();
    str_yaw   += sensorData[IDX_YAW][i].toString();
    str_roll  += sensorData[IDX_ROLL][i].toString();
    str_pitch += sensorData[IDX_PITCH][i].toString();
    str_v     += (sensorData[IDX_RPM][i] * 60 * wheelSize * Math.PI / 1000000).toString();
  }
  str_rpm   += "]\n";
  str_yaw   += "]\n";
  str_roll  += "]\n";
  str_pitch += "]\n";
  str_v     += "]\n";

  return (str_rpm + str_yaw + str_roll + str_pitch + str_v);
}


//-----------------------------------------------------------------------------
// Trial Data Selecterがクリックされたときの挙動
//
function handleTrialDataFileSelect(evt)
{
  var files = evt.target.files;

  for (var i=0 ; i<files.length ; i++) {
   //J ファイルの内容をパースする
   loadTrialDataFile(files[i], trialDataUpdatedCallback);
  }
}

//
//J Trial Dataの読み込み処理
//
function loadTrialDataFile(file, loaded_cb)
{
  var reader = new FileReader();

  //J ファイルの中身を読み込む
  reader.onload = function (evt) {
    var raw = reader.result;

    //J 1行に区切って処理する
    var lines = raw.split(/\r\n|\r|\n/);
    if (lines[0].indexOf("# Project") == 0) {
      parseProjectFile(lines, loaded_cb);
    }
    else {
      [sensor_data_arr, policy_arr] = parseTrialData(lines);
      loaded_cb(file.name, policy_arr, sensor_data_arr);
    }
  }

  reader.readAsText(file);
}

//
//J Trialデータをパースする
//
function parseTrialData(lines)
{
  var policy_arr = []
  var sensor_data_arr = []

  var policy = [];
  var sensorData = [[],[],[],[],[],[],[],[],[],[],[],[],[]];

  lines.forEach( function (line) {
    //J strategy は arr_policyという名前のPython形式の配列として記述されている
    if (line.indexOf('arr_policy') == 0) {
      //J []で囲まれた範囲を切り出す
      var begin = line.indexOf('[')+1;
      var end = line.indexOf(']');
      policy = line.substring(begin, end).split(/,/)

      //J 方策を保存
      policy_arr.push(policy)
      if (sensorData[0].length != 0) {
        sensor_data_arr.push(sensorData)
      }
      sensorData = [[],[],[],[],[],[],[],[],[],[],[],[],[]];
    }
    else {
      //J Tab区切りの数字列なのでこれを処理する
      var values = line.split(/\t/)
      if (values.length == 14 || values.length == 16 ) { 
        sensorData[IDX_AX].push(Number(values[IDX_AX+1]));
        sensorData[IDX_AY].push(Number(values[IDX_AY+1]));
        sensorData[IDX_AZ].push(Number(values[IDX_AZ+1]));
        sensorData[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
        sensorData[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
        sensorData[IDX_YAW].push(Number(values[IDX_YAW+1]));
        sensorData[IDX_RPM].push(Number(values[IDX_RPM+1]));
        sensorData[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
        sensorData[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
        sensorData[IDX_DUTY].push(Number(values[IDX_DUTY+1]));
        sensorData[IDX_POS].push(Number(values[13]));
        sensorData[IDX_EVA].push(Number(values[15]));
      }
      else if (values.length == 12) { 
        sensorData[IDX_AX].push(Number(values[IDX_AX+1]));
        sensorData[IDX_AY].push(Number(values[IDX_AY+1]));
        sensorData[IDX_AZ].push(Number(values[IDX_AZ+1]));
        sensorData[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
        sensorData[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
        sensorData[IDX_YAW].push(Number(values[IDX_YAW+1]));
        sensorData[IDX_RPM].push(Number(values[IDX_RPM+1]));
        sensorData[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
        sensorData[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
        sensorData[IDX_DUTY].push(Number(values[IDX_DUTY+1]));
        sensorData[IDX_POS].push(Number(values[11]));
      }
      else if (values.length == 11) { 
        sensorData[IDX_AX].push(Number(values[IDX_AX+1]));
        sensorData[IDX_AY].push(Number(values[IDX_AY+1]));
        sensorData[IDX_AZ].push(Number(values[IDX_AZ+1]));
        sensorData[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
        sensorData[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
        sensorData[IDX_YAW].push(Number(values[IDX_YAW+1]));
        sensorData[IDX_RPM].push(Number(values[IDX_RPM+1]));
        sensorData[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
        sensorData[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
        sensorData[IDX_DUTY].push(Number(values[IDX_DUTY+1]));
        sensorData[IDX_POS].push(0);
      }
      else if (values.length >= 10) { 
        sensorData[IDX_AX].push(Number(values[IDX_AX+1]));
        sensorData[IDX_AY].push(Number(values[IDX_AY+1]));
        sensorData[IDX_AZ].push(Number(values[IDX_AZ+1]));
        sensorData[IDX_PITCH].push(Number(values[IDX_PITCH+1]));
        sensorData[IDX_ROLL].push(Number(values[IDX_ROLL+1]));
        sensorData[IDX_YAW].push(Number(values[IDX_YAW+1]));
        sensorData[IDX_RPM].push(Number(values[IDX_RPM+1]));
        sensorData[IDX_VBAT].push(Number(values[IDX_VBAT+1]));
        sensorData[IDX_IMOT].push(Number(values[IDX_IMOT+1]));
        sensorData[IDX_DUTY].push(0);
        sensorData[IDX_POS].push(0);
      }
    }
  });

  console.log(policy_arr)

  return [sensor_data_arr, policy_arr];
}


//
//J Projectファイルをパースする
//
function parseProjectFile(lines)
{
  return;
}


//
//J Trial Dataが読み込まらた後の処理
//
function trialDataUpdatedCallback(file_name, policy_arr, sensor_data_arr)
{
  var wheelSize = document.getElementById("wheel_size").value;

  var trial_data_select = document.getElementById("trial_data_list");

  console.log(policy_arr.length)
  console.log(sensor_data_arr.length)

  for (var i=0 ; i < sensor_data_arr.length ; ++i) {
    var option = document.createElement("option");
    var id = trial_data_select.length;
    option.text = file_name + " - trial " + i.toString() ;
    option.value = id;
  
    trial_data_select.appendChild(option);
  
    var velocity = createVelocityOnEachPosition(sensor_data_arr[i]);
  
    TrialDataList.push(new TrialDataInfo(id, file_name, policy_arr[i], policy_arr[i+1], velocity, sensor_data_arr[i]));
  }


  document.getElementById("trial_data_list").value = (TrialDataList.length-1).toString();
  onTrialDataSelecterChanged(document.getElementById("trial_data_list"))
}

//
//J RPMから時速に変換して返す
//
function createVelocityOnEachPosition(sensorData)
{
  var velocity  = new Array(sensorData[IDX_POS].length);
  var wheelSize = document.getElementById("wheel_size").value;

  for (var i=0 ; i<sensorData[IDX_RPM].length ; ++i) {
    velocity[i] = sensorData[IDX_RPM][i] * 60 * Math.PI * wheelSize / 1000000;
  }

  return velocity
}


//-----------------------------------------------------------------------------
// 特徴量マップの再描画
//
function updateStateSpaceVector(position = -1)
{
  if (StateSpaceVec.length == 0) return;

  var lanes = 3;
  if (document.getElementById("five_lane").checked) {
    lanes = 5;
  }

  drawStateSpaceVector(
    document.getElementById("map_canvas"),
    document.getElementById("minimap_box").clientWidth,
    document.getElementById("minimap_box").clientHeight,
    StateSpaceVec,
    position,
    0,
    lanes);
}

//-----------------------------------------------------------------------------
// Raw Data種別の選択が変更されたときの処理
//
function updateRawDataSelecter()
{
  var raw_data_canvas = document.getElementById('raw_graph_canvas');
  var info_canvas     = document.getElementById('extra_info_canvas');

  updateRawDataGraph(raw_data_canvas, info_canvas, TrialDataList[document.getElementById("trial_data_list").value].data);

  // canvas_graph の高さを描画内容に合わせる
  var h = document.getElementById("raw_graph_canvas").getAttribute("height")
  document.getElementById("graph_box").style.height = h.toString() ;
}

