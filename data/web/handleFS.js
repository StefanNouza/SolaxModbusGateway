// https://jsfiddle.net/tobiasfaust/uc1jfpgb/

var DirJson;


window.addEventListener('load', initHandleFS, false);
function initHandleFS() {
  requestListDir();
}

// ***********************************
// Ajax Request to update  
// ***********************************
function requestListDir() {
	var data = {};
  data['action'] = "handlefiles";
  data['subAction'] = "listDir"
  //ajax_send(JSON.stringify(data));
  
  var http = null;
  if (window.XMLHttpRequest)  { http =new XMLHttpRequest(); }
  else                        { http =new ActiveXObject("Microsoft.XMLHTTP"); }
  
  if(!http){ alert("AJAX is not supported."); return; }
 
  var url = 'ajax';
  var params = 'json=' + JSON.stringify(data);
  
  http.open('POST', url, true);
  http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
  http.onreadystatechange = function() { //Call a function when the state changes.
    if(http.readyState == 4 && http.status == 200) {
      DirJson = JSON.parse(http.responseText);
      listFiles("/");
    } 
  }
  http.send(params);
}

// ***********************************
// show content of fetched file 
// ***********************************
function setContent(string, filename) {
    if (filename.endsWith("json")) {
        obj = document.getElementById('content').value = JSON.stringify(JSON.parse(string), null, 2);
    } else {
        obj = document.getElementById('content').value = string;
    }
  }

// ***********************************
// fetch file from host
// ***********************************
function fetchFile(file) {
	obj = document.getElementById('content').value = "loading "+file+"...";
  
  fetch(file)
  .then(response => response.text())
  .then(textString => setContent(textString, file));
}

// ***********************************
// show directory structure 
// ***********************************
function listFiles(path) {
  var table = document.querySelector('#files'),
      row = document.querySelector('#NewRow'),
      tr_tpl, DirJsonLocal;
  
  // cleanup table
  table.replaceChildren();
  
  // get the right part
  for(let i = 0; i < DirJson.length; i++) { 
  	if (DirJson[i].path == path) {
    	DirJsonLocal = DirJson[i]
    }
  }
  
  // show path information
  document.getElementById('path').innerHTML = path;
  
  // set "back" item if not root
  if (path != '/') {
    tr_tpl = document.importNode(row.content, true);
    cells = tr_tpl.querySelectorAll("td");
      cells.forEach(function (item, index) {
        var text = item.innerHTML;
        var oc = "listFiles('" + getParentPath(path) + "')"
        text = text.replaceAll("{file}", '..');
        item.innerHTML = text;
        item.setAttribute('onClick', oc);
    });
    table.appendChild(tr_tpl);
	}
  
  // show files
  DirJsonLocal.content.forEach(function (file) {
  	// template "laden" (lies: klonen)
    tr_tpl = document.importNode(row.content, true);
		cells = tr_tpl.querySelectorAll("td");
    cells.forEach(function (item, index) {
      var text = item.innerHTML;
      var oc;
      
      if(file.isDir == 0) {
        oc = item.getAttribute('onClick');
        var newPath = DirJsonLocal.path + "/" + file.name;
        if (newPath.startsWith("//")) {newPath = newPath.substring(1)}
	      oc = oc.replaceAll("{fullpath}", newPath);
        text = text.replaceAll("{file}", file.name);
      } else if(file.isDir == 1) {
      	var newPath = DirJsonLocal.path + "/" + file.name;
        if (newPath.startsWith("//")) {newPath = newPath.substring(1)}
      	oc = "listFiles('" + newPath + "')"
        text = text.replaceAll("{file}", file.name + "/");
      }
      
      
      item.innerHTML = text;
      item.setAttribute('onClick', oc);
    });
		table.appendChild(tr_tpl);
	})
 }

listFiles('/');

// ***********************************
// returns parent path: '/regs/web' -> '/regs' 
// ***********************************
function getParentPath(path) {
	var ParentPath, PathArray;
  
  PathArray = path.split('/')
  PathArray.pop()
  if (PathArray.length == 1) { ParentPath = '/' }
  else { ParentPath = PathArray.join('/')}
  return ParentPath
}