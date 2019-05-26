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
    var url = '/control/calibration?blind=' + blind + '&value=' + value + '&action=' + action;
    doRequest(url);
  } else {
    addMessage("Value is empty!", "error");
  }

}

function refreshEepromValues() {
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
          var jsonData = JSON.parse(data);
          document.getElementById("vLeftBlindPosition").value = jsonData.LeftBlindPosition;
          document.getElementById("vRightBlindPosition").value = jsonData.RightBlindPosition;
          document.getElementById("vCloseSun").value = jsonData.CloseSun;
          document.getElementById("vCloseNight").value = jsonData.CloseNight;
          document.getElementById("vOpenLow").value = jsonData.OpenLow;
          document.getElementById("vOpenMiddle").value = jsonData.OpenMiddle;
          document.getElementById("vOpenHight").value = jsonData.OpenHight;
          console.log("Done", this.status);
      }
      if (this.status != 200) {
          addMessage("<strong>Error:</strong> request with status code " + this.status + "error");
      }
      return jsonData;
  }
  httpRequest.send(null);
}


function refreshRangeElements() {

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
            var jsonData = JSON.parse(data);
            document.getElementById("LeftBlind").value = jsonData.LeftBlindPosition;
            document.getElementById("RightBlind").value = jsonData.RightBlindPosition;
            console.log("Done", this.status);
        }
        if (this.status != 200) {
            addMessage("<strong>Error:</strong> request with status code " + this.status + "error");
        }
    }
    httpRequest.send(null);
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
                  refreshRangeElements();
                }
            }
            if (this.status != 200) {
                addMessage("<strong>Error:</strong> request with status code " + code, "error");
            }
        }

        httpRequest.send(null);
    } else {
        alert("Url is empty");
    }

}
