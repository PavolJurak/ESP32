function clearMessage() {

    var p = document.getElementById("message");
    var md = document.getElementById("message-div");

    p.innerHTML = "";
    md.classList.remove('alert-danger');
    md.classList.remove('alert-success');

}

function addMessage(text, type) {

    var p = document.getElementById("message");
    var md = document.getElementById("message-div");

    p.innerHTML = text;
    if (type == "succesfull") {
        md.classList.add('alert-success');
    }
    if (type == "error") {
        md.classList.add('alert-danger');
    }

    (function () {
        setTimeout(clearMessage, 1500);
    }())

}

function sendCalibrationRequest() {

  var blind = document.getElementById("selectBlind").value;
  var value = document.getElementById("blindValue").value;
  var action = document.getElementById("selectAction").value;

  if (value != "") {
    var url = '/control/blind/calibration?blind=' + blind + '&value=' + value + '&action=' + action;
    doRequest(url);
  } else {
    addMessage("Value is empty!", "error");
  }

}

function getEepromValues() {

  var url = 'eeprom/values';
  if (window.ActiveXObject) {
      httpRequest = new ActiveXObject("Microsoft.XMLHTTP");
  } else {
      httpRequest = new XMLHttpRequest();
  }
  httpRequest.open("GET", url, true);

  httpRequest.onload = function () {
      if (this.readyState == 4 && this.status == 200) {
          addMessage("REFRESH SUCCESFULL", "succesfull");
          var data = this.responseText;
          console.log("Done", code);
          return data;
      }
      if (this.status != 200) {
          addMessage("<strong>Error:</strong> request with status code " + code, "error");
      }
  }
  httpRequest.send(null);

}

function refreshEepromValues() {
  var data = JSON.parse(getEepromValues());
  if (data != "") {
    document.getElementById("vLeftBlindPosition").value = data.LeftBlindPosition;
    document.getElementById("vRightBlindPosition").value = data.RightBlindPosition;
    document.getElementById("vCloseSun").value = data.CloseSun;
    document.getElementById("vCloseNight").value = data.CloseNight;
    document.getElementById("vOpenLow").value = data.OpenLow;
    document.getElementById("vOpenMiddle").value = data.OpenMiddle;
    document.getElementById("vOpenHight").value = data.OpenHight;
  } else {
    addMessage("<strong>Error:</strong> eeprom data are empty");
  }
}


function getBlindsPosition() {

    var leftBlindRange = document.getElementById("LeftBlind");
    var rightBlindRange = document.getElementById("RightBlind");

    var data = JSON.parse(getEepromValues());
    leftBlindRange.value = data.LeftBlindPosition;
    rightBlindRange.value = data.RightBlindPosition;
}

function map(x, in_min, in_max, out_min, out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

function setBlindPercent(blind, value) {

    var valuePosition;
    if (blind == 'Left') {
        valuePosition = document.getElementById("leftBlindPosition");
    }
    if (blind == 'Right') {
        valuePosition = document.getElementById("rightBlindPosition");
    }
    //calculate to percent
    var percent = map(value, 5, 175, 0, 100);
    valuePosition.innerText = Math.round(percent)+"%";

}

function doRequest(url) {

    if (url != null) {
        if (window.ActiveXObject) {
            httpRequest = new ActiveXObject("Microsoft.XMLHTTP");
        } else {
            httpRequest = new XMLHttpRequest();
        }

        httpRequest.open("GET", url, true);

        httpRequest.onload = function () {
            var code = httpRequest.status;
            if (this.readyState == 4 && this.status == 200) {
                addMessage("OK", "succesfull");
                if (url.includes("/control/blind")) {
                  getBlindsPosition();
                }
            }
            if (this.status != 200) {
                addMessage("<strong>Error:</strong> request with status code " + code, "error");
            }
            console.log("Done", code);
        }

        httpRequest.send(null);
    } else {
        alert("Url is empty");
    }

}
