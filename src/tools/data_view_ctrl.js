/*
 * data_view_ctrl.js
 *
 * Created: 2022/08/20
 * Copyright 2022 Kiyotaka Akasaka. All rights reserved.
 */
/*-------------------------------------------------------------------------------------------------
 * 各種変数/定数定義
 */
var DATA_SET={}


/*-----------------------------------------------------------------------------
 * HTML Form まわりの挙動を記載
 */
window.onload = function() {
}

//J Re-size時には描画をやり直す
window.onresize = function () {
}



//-----------------------------------------------------------------------------
// HTML UI Callbacks

//
//J Learning / Mapボタンが押されたときの挙動
//
function onFileLoadButtonClick()
{
  document.getElementById("data_file").click();
  document.getElementById("data_file").addEventListener('change', handleDataFileSelect, false);
}


//-----------------------------------------------------------------------------
//J File Selecterがクリックされたときの挙動
//
function handleDataFileSelect(evt)
{
  var file = evt.target.files[0];

  //J ファイル名の表示
  data_file.innerHTML = file.name;

  //J ファイルの内容をパースする
  loadAndParseDataFile(file);
}

//
//J Map Dataの読み込み処理
//
function loadAndParseDataFile(file)
{
  var reader = new FileReader();

  console.log(file.name)

  document.getElementById('data_file_name').innerHTML = file.name

  //J ファイルの中身を読み込む
  reader.onload = function (evt) {
    //J 1行に区切って処理する
    var lines = reader.result.split(/\r\n|\r|\n/);
    var latest = ''
    for (var i=0 ; i<lines.length ; ++i) {
      if (lines[i].indexOf("#") == 0) {
        continue; // skip
      }
      else if (lines[i].indexOf("$") == 0) {
	token = lines[i].substr(1).trim().split(/\t/)
        DATA_SET[token[0]] = new Array(token.length-1)
        latest = token[0]
        for (let y = 1; y < token.length ; ++y) {
          DATA_SET[latest][y-1] = ([token[y],[]])
        }
      }
      else if (latest != '') {
        token = lines[i].trim().split(/\t/)
        if (token.length != DATA_SET[latest].length) {
          continue
        }
        for (let y = 0; y < token.length ; ++y) {
          DATA_SET[latest][y][1].push(token[y])
        }
      }
    }

    updateDataset(DATA_SET)

    return;
  }

  reader.readAsText(file);
}

function updateDataset(dataset)
{
  html = ''

  var i=0;
  var init_key = '';
  for (let key in dataset) {
    if (i==0) { html += '<input class="data_list_sel" type="radio" name="data_set" onchange="changeDataset(value, DATA_SET)" value="'+ key +'" checked>' + key; init_key = key; }
    else      { html += '<input class="data_list_sel" type="radio" name="data_set" onchange="changeDataset(value, DATA_SET)" value="'+ key +'">' + key }

    i++;
  }

  document.getElementById('data_list').innerHTML = html
  changeDataset(init_key, DATA_SET)

  return;
}

var CURRENT_SET=''
var CURRENT_ITEM=''
function changeDataset(set, dataset)
{
  CURRENT_SET = set;
  var html=''

  html+= '<select name="data_item_select" onchange="changeDataitem(value, CURRENT_SET, DATA_SET)">'
  html+= '<option value="all">all</option>'
  for (let i=0 ; i<dataset[set].length ; ++i) {
    html += '<option value="'+ dataset[set][i][0] +'">' + dataset[set][i][0] + '</option>'
  }
  html += '</select>'

  let min_arr = []
  let max_arr = []
  for (let i=0 ; i<dataset[set].length ; ++i) {
    min_arr.push(Math.min(...dataset[set][i][1]))
    max_arr.push(Math.max(...dataset[set][i][1]))
  }

  html += '<div> <input type="range" id="range_min" name="range_min" onchange="changeDataitem(CURRENT_ITEM, CURRENT_SET, DATA_SET)" value="'+ Math.round(Math.min(...min_arr)).toString() +'" min="'+ Math.round(Math.min(...min_arr)).toString() +'" max="'+ Math.round(Math.max(...max_arr)).toString() +'"><label id="label_min" for="range_min">Min</label></div>'
  html += '<div> <input type="range" id="range_max" name="range_max" onchange="changeDataitem(CURRENT_ITEM, CURRENT_SET, DATA_SET)" value="'+ Math.round(Math.max(...max_arr)).toString() +'" min="'+ Math.round(Math.min(...min_arr)).toString() +'" max="'+ Math.round(Math.max(...max_arr)).toString() +'"><label id="label_max"for="range_max">Max</label></div>'

  document.getElementById('data_item').innerHTML = html

  changeDataitem('all',set, dataset)
}

function changeDataitem(item, set, dataset)
{
  CURRENT_ITEM = item

  var canvas = document.getElementById('main_graph')
  dat = []
  let COL = ['#f5deb3', '#ffff00', '#ffd700', '#ffa500', '#f4a460', '#ff8c00', '#daa520', '#cd853f', '#b8860b', '#d2691e', '#a0522d']
  let SHA = ['#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c', '#f0e68c']

  let col = []
  let sha = []
  let label = []

  let rsvd = -1
  for (let i=0 ; i<dataset[set].length ; ++i) {
    if (item == dataset[set][i][0]) {
      rsvd = i;
    }
    else {
      label.push(dataset[set][i][0])
      dat.push(dataset[set][i][1])
      col.push(COL[i])
      sha.push(SHA[i])
    }
  }

  if (rsvd != -1) {
    label.push(dataset[set][rsvd][0])
    dat.push(dataset[set][rsvd][1])

    for (let i=0 ; i<col.length ; ++i) {
      col[i] = '#696969'
      sha[i] = '#a9a9a9'
    }

    col.push(COL[rsvd])
    sha.push(SHA[rsvd])
  }

  let range_max = parseInt(document.getElementById('range_max').value)
  let range_min = parseInt(document.getElementById('range_min').value)
  sub_y = Math.round((range_max - range_min) / 5)
  if (sub_y == 0) sub_y=1

  document.getElementById('label_min').innerHTML = 'MIN '+range_min.toString()
  document.getElementById('label_max').innerHTML = 'MAX '+range_max.toString()


  console.log(range_min, range_max,sub_y)

  drawLabeledLineGraph(canvas, set, dat, range_min, range_max, 52, sub_y, col, sha, label)


  return;
}