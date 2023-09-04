/*
 * strategy_editor_ctrl.js
 *
 * Created: 2020/03/04
 * Copyright 2020 Kiyotaka Akasaka. All rights reserved.
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

var TrialDataList = [];
TrialDataInfo = function(id, filename, policy, average_velocity, average_duty) {
  this.id = id;
  this.filename = filename;
  this.policy = policy;
  this.average_velocity = average_velocity;
  this.average_duty = average_duty;

  this.diff = new Array(policy.length);
  for (var i=0 ; i<this.diff.length ; ++i) {
    if (this.average_velocity[i] == NaN) {
      this.diff[i] = NaN;
    }
    else {
      this.diff[i] = this.average_velocity[i] - this.policy[i];
    }
  }
}


/*-----------------------------------------------------------------------------
 * HTML Form まわりの挙動を記載
 */
window.onload = function() {
  var canvas = document.getElementById("main_canvas");
  canvas.addEventListener('click', canvasOnClick, false);
}

//J Re-size時には描画をやり直す
window.onresize = function () {
  updateMainCanvasAll();
  updateStateSpaceVector();
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
    sensorData = [[],[],[],[],[],[],[],[],[],[],[]] //J 内容を破棄する

    //J 1行に区切って処理する
    var lines = raw.split(/\r\n|\r|\n/);
    //J 1行目の内容が、# SSVであれば、生データではない
    if (lines[0].indexOf("# SSV") == 0) {
      document.getElementById("leftCoeff").disabled = true;
      document.getElementById("rightCoeff").disabled = true;
      parseSsvFile(lines);
      return;
    }
    else {
      document.getElementById("leftCoeff").disabled = false;
      document.getElementById("rightCoeff").disabled = false;
    }

    lines.forEach( function (line) {
      //J Tab区切りの数字列なのでこれを処理する
      var values = line.split(/\t/)
      if (values.length >= 11) { 
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

  var wheelSize = document.getElementById("wheel_size").value;

  //J 距離正規化
  ResampledSensorData = resampling(SensorData, wheelSize, UNIT_mm);
  //J 速度 - 距離推定
  var [velocityArr, odometryArr, velocityArrFromAccel, odometryArrFromAccel] = estimateVelocityAndOdometory(SensorData, wheelSize);

  //J 特徴量ヒストグラムと内周、中央周、外周を判断するための閾値を求める
  var distribution = createFeatureValueHistgram(ResampledSensorData,UNIT_mm, -800, 800, 10);
  ThresholdOfCurve = decideThresholdOfFeatures(distribution, 10);

  //J 3周分のデータを利用して、状態空間を作り出す
  StateSpaceVec = createStateSpaceVector(SensorData, UNIT_mm, {left:1.0, right:1.0}, wheelSize, ThresholdOfCurve);
  updateStateSpaceVector(-1);

  //J 特徴量リストをMapリストに載せる
  updateFeatureList(StateSpaceVec);
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
  updateFeatureList(StateSpaceVec);

  return;
}

//
//J MapリストにSSVをのせる
//
function updateFeatureList(ssv)
{
  //J 特徴量リストをMapリストに載せる
  var map_select = document.getElementById("property_list");
  while(map_select.hasChildNodes()) {
    map_select.removeChild(map_select.lastChild);
  }

  for (var i=0; i<ssv.length ; ++i) {
    var ss = ssv[i];
    var option = document.createElement("option");
    option.text = i.toString() + "- Lap" + ss.lap.toString();
    option.value = i;

    map_select.appendChild(option);
  }
}



//-----------------------------------------------------------------------------
//J Save Map ボタンが押されたときの挙動
//
function onSaveMapButtonClick()
{
  if (StateSpaceVec.length == 0) {
    return;
  }

  var filename = "map.txt"
  var stream = createStateSpaceVectorStream(StateSpaceVec);

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

function createStateSpaceVectorStream(ssv)
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

  var str_curve = "curve_threshold = [";
  str_curve += ThresholdOfCurve.inter_to_center_left.toString() + ",";
  str_curve += ThresholdOfCurve.center_to_outer_left.toString() + ",";
  str_curve += ThresholdOfCurve.inter_to_center_right.toString() + ",";
  str_curve += ThresholdOfCurve.center_to_outer_right.toString();
  str_curve += "]\n";

  str = "# SSV\n" + str_s + str_x + str_y + str_l + str_curve;

  //J Byte Streamに変換
  var len = str.length;
  var arr = new Uint8Array(len);
  for (var i = 0; i < len; i++) {
    arr[i] = str[i].charCodeAt(0);
  }
  return arr;
}

//-----------------------------------------------------------------------------
//J Add Trial Data ボタンが押されたときの挙動
//
function onAddTrialDataButtonClick()
{
  document.getElementById("trial_data_file").click();
  document.getElementById("trial_data_file").addEventListener('change', handleTrialDataFileSelect, false);
}


//
// Test File Selecterがクリックされたときの挙動
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
//J Learining Dataの読み込み処理
//
function loadTrialDataFile(file, loaded_cb)
{
  var reader = new FileReader();

  //J ファイルの中身を読み込む
  reader.onload = function (evt) {
    var raw = reader.result;
    var policy = [];
    var sensorData = [[],[],[],[],[],[],[],[],[],[],[]];

    //J 1行に区切って処理する
    var lines = raw.split(/\r\n|\r|\n/);
    lines.forEach( function (line) {
      //J strategy は arr_policyという名前のPython形式の配列として記述されている
      if (line.indexOf('arr_policy') == 0) {
        //J []で囲まれた範囲を切り出す
        var begin = line.indexOf('[')+1;
        var end = line.indexOf(']');
        policy = line.substring(begin, end).split(/,/)
      }
      else {
        //J Tab区切りの数字列なのでこれを処理する
        var values = line.split(/\t/)
        if (values.length >= 11) { 
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
        }
      }
    });

    //J 読み込んだデータ返す
    loaded_cb(file.name, policy, sensorData);
  }

  reader.readAsText(file);
}


//
//J Test Dataが読み込まらた後の処理
//
function trialDataUpdatedCallback(file_name, policy, sensorData)
{
  var wheelSize = document.getElementById("wheel_size").value;

  var trial_data_select = document.getElementById("trial_data_list");
  var option = document.createElement("option");
  var id = trial_data_select.length;
  option.text = id.toString() + " - " + file_name;
  option.value = id;

  trial_data_select.appendChild(option);

  var [ave_velocity, ave_duty] = analizeDriveRecord(StateSpaceVec, sensorData, UNIT_mm, 1.0/50.0, wheelSize, ThresholdOfCurve);

  TrialDataList.push(new TrialDataInfo(id, file_name, policy, ave_velocity, ave_duty));
}


//-----------------------------------------------------------------------------
//J ホイールの選択（プルダウンセレクト）が変更になった際の処理
//
function onWheelSelected()
{
  document.getElementById("wheel_size").value = document.getElementById("wheel_select").options[document.getElementById("wheel_select").selectedIndex].value;
}

//
// ホイールサイズが直接入力されたとき
//
function onWheelSizeChanged()
{
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

  estimateMachinePosition(SensorData, coeff, document.getElementById("wheel_size").value, "updateEstimatedMachinePosition()");

  StateSpaceVec = createStateSpaceVector(SensorData, UNIT_mm, coeff, document.getElementById("wheel_size").value, ThresholdOfCurve);
  updateStateSpaceVector(-1);
  updateTrialDataHeatmap(-1);
}

//
// 特徴量マップの再描画
//
function updateStateSpaceVector(position = -1)
{
  drawStateSpaceVector(
    document.getElementById("main_canvas"),
    document.getElementById("visual_box").clientWidth,
    document.getElementById("visual_box").clientHeight,
    StateSpaceVec,
    position,
    0);
}



//-----------------------------------------------------------------------------
// 特徴量マップ情報をリストにダンプ
//
function onMapSelecterChanged(select)
{
  var position = select.options[select.selectedIndex].value;

  //J Property view を変更
  document.getElementById("course_feature").value = StateSpaceVec[position].feature;

  //J 描画を更新
  updateStateSpaceVector(position);
  updateTrialDataHeatmap(position);
}

//-----------------------------------------------------------------------------
// 試行データの変更
//
function onTrialDataSelecterChanged(select)
{
  updateMainCanvasAll();
}

function updateTrialDataHeatmap(position)
{
  updateMainCanvasAll(position);
}



//-----------------------------------------------------------------------------
// Main Canvasに出力するレイヤーを変更する
//
function updateMainCanvasAll(position = -1)
{

  function _find_min(arr) {
    var min = arr[10];
    for (var i=11 ; i<arr.length ; ++i) {
      if (arr[i] < min) {
        min = arr[i];
      }
    }

    return Math.round(min);
  }
  function _find_max(arr) {
    var max = arr[10];
    for (var i=11 ; i<arr.length ; ++i) {
      if (arr[i] > max) {
        max = arr[i];
      }
    }

    return Math.round(max);
  }


  var is_cource_state_enabled = document.getElementById("enable_course_state").checked;
  var is_extra_info_enabled   = document.getElementById("enable_extra").checked;
  var extra_info_elements     = document.getElementsByName("extra_info");
  var extra_info_name = "";
  for (var i=0 ; i<extra_info_elements.length ; ++i) {
    if (extra_info_elements[i].checked) {
      extra_info_name = extra_info_elements[i].value;
      break;
    }
  }

  if (is_cource_state_enabled) {
    document.getElementById("main_canvas").style.visibility = "visible";
  }
  else {
    document.getElementById("main_canvas").style.visibility = "hidden";
  }

  if (is_extra_info_enabled) {
    var select = document.getElementById("trial_data_list");
    if (select.selectedIndex < 0) {
      return;
    }
    var selected_val = select.options[select.selectedIndex].value;


    var arr = [];
    var min = 0;
    var max = 100;
    var unit_str = "";
    if (extra_info_name == "strategy") {
      arr = TrialDataList[selected_val].policy;
      min = 10;
      max = 25;
      unit_str = "[km/h]"
    }
    else if (extra_info_name == "velocity") {
      arr = TrialDataList[selected_val].average_velocity;
      min = 10;
      max = 25;
      unit_str = "[km/h]"
    }
    else if (extra_info_name == "duty") {
      arr = TrialDataList[selected_val].average_duty;
      min = -100;
      max = 100;
      unit_str = "[%]"
    }
    else if (extra_info_name == "diff") {
      arr = TrialDataList[selected_val].diff;
      min = _find_min(TrialDataList[selected_val].diff);
      max = _find_max(TrialDataList[selected_val].diff);
      if(min < 0) {
        max = Math.abs(min) > max ? Math.abs(min) : max;
        min = -max;
      }

      unit_str = "[km/h]"
    }
    drawHeatMap(document.getElementById("extra_info_canvas"),
                        document.getElementById("visual_box").clientWidth,
                        document.getElementById("visual_box").clientHeight,
                        StateSpaceVec, 
                        arr,
                        position,
                        0,
                        min,
                        max,
                        unit_str);


    document.getElementById("extra_info_canvas").style.visibility = "visible";
  }
  else {
    document.getElementById("extra_info_canvas").style.visibility = "hidden";
  }


}