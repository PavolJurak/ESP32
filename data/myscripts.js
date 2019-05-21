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
  var baseUrl = 'control/blind/calibration?';
  var blind = document.getElementById("selectBlind").value;
  var value = document.getElementById("blindValue").value;
  var action = document.getElementById("selectAction").value;
}

function refreshEepromValues(url) {
  if (url != null) {
      if (window.ActiveXObject) {
          httpRequest = new ActiveXObject("Microsoft.XMLHTTP");
      } else {
          httpRequest = new XMLHttpRequest();
      }

      httpRequest.open("GET", url, true)

      httpRequest.onload = function () {
          var code = httpRequest.status;
          if (this.readyState == 4 && this.status == 200) {
              addMessage("REFRESH SUCCESFULL", "succesfull");
              var data = JSON.parse(this.responseText);
              document.getElementById("vCloseSun").value = data.CloseSun;
              document.getElementById("vCloseNight").value = data.CloseNight;
              document.getElementById("vOpenLow").value = data.OpenLow;
              document.getElementById("vOpenMiddle").value = data.OpenMiddle;
              document.getElementById("vOpenHight").value = data.OpenHight;
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


function getBlindsPosition() {

    var url = "/control/blind";
    var leftBlindRange = document.getElementById("LeftBlind");
    var rightBlindRange = document.getElementById("RightBlind");

    if (window.ActiveXObject) {
        httpRequest = new ActiveXobject("Microsoft.XMLHTTP");
    } else {
        httpRequest = new XMLHttpRequest();
    }
    httpRequest.open("GET", url, true);

    httpRequest.onload = function () {
        var code = httpRequest.status;
        if (this.readyState == 4 && this.status == 200) {
            var text = httpRequest.responseText;
            var json_obj = JSON.parse(text);
            leftBlindRange.value = json_obj.left;
            rightBlindRange.value = json_obj.right;
        }
        console.log('DONE', code);
    };

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

        httpRequest.open("GET", url, true)

        httpRequest.onload = function () {
            var code = httpRequest.status;
            if (this.readyState == 4 && this.status == 200) {
                addMessage("OK", "succesfull");
                getBlindsPosition();
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
