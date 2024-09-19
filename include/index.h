#include <Arduino.h>
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="de">
  
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <title>ESP32 remote control</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }

      html, body {
        height: 100%;
        font-family: Roboto, sans-serif;
        font-size: 12pt;
        overflow: hidden;
        background-color: #171a1c;
      }

      body {
        display: grid;
        grid-template-columns: 2fr;
        grid-template-rows: 1fr;
        align-items: center;
        justify-items: center;
      }

      #panel {
        display: grid;
        grid-gap: 2em;
        justify-items: center;
        min-width: 300px;
        padding: 1rem 0 2rem 0;
        background-color: #eee;
        background-color: #29363d;
        border-radius: 1rem;
        box-shadow: 0 8px 16px rgba(0, 0, 0, 0.4), inset 0 2px 4px rgba(159, 202, 223, 0.25);
      }

      h1 {
        font-size: 1.5rem;
        text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);
      }
      input {
        width: 100%;
        height: 1.5em;
        padding: 1.05em 0 .95em 0;
        font-family: 'DSEG7 Modern Mini', monospace;
        font-size: 1.2rem;
        text-shadow: 0 1px 2px #000;
        border-radius: 8px;
        border: 0;
        -webkit-box-shadow: inset 0 2px 4px #000, 0 1px 1px rgba(255, 255, 255, 0.3);
        -moz-box-shadow: inset 0 2px 4px #000, 0 1px 1px rgba(255, 255, 255, 0.3);
        box-shadow: inset 0 2px 4px #000, 0 1px 1px rgba(255, 255, 255, 0.3);
        outline: none;
        text-align: center;
      }

      input#time {
        color: #00cccc;
        background-color: #141b1f;
      }

      input#date {
        color: #00cccc;
        background-color: #141b1f;
      }

      .display {
        display: grid;
        grid-gap: .25rem;
      }

      .inset {
        color: #668899;
        text-shadow: -1px -1px 0 #000;
      }      
    </style>
  </head>
  <body>
    <div id="panel">
      <h1 class="inset">ESP32 remote control</h1>
      <div class="display">
        <label class="inset" for="time">Time</label>
        <input type="text" id="time" readonly>
      </div>
      <div class="display">
        <label class="inset" for="date">Date</label>
        <input type="text" id="date" readonly>
      </div>
    </div>
  </body>
  </html>
  <script>
    var time;
    var date;

    window.addEventListener('load', onLoad);

    function onLoad(event) {
        initTime();
    }

    function initTime() {
        time = document.getElementById('time');
        date = document.getElementById('date');
        updateTime();
        // a timing event manager is initialized
        // which must be triggered every second to refresh
        // the display of the current time
        setInterval(updateTime, 1000);
    }

    function updateTime() {
        let now    = new Date();
        let h      = now.getHours();
        let m      = now.getMinutes();
        let s      = now.getSeconds();
        let dd     = now.getDate();
        let mm     = now.getMonth() + 1;
        let yy     = now.getFullYear();
        time.value = `${normalize(h)}:${normalize(m)}:${normalize(s)}`;
        date.value = `${normalize(dd)}.${normalize(mm)}.${yy}`;
    }

    function normalize(digit) {
        return (digit < 10 ? '0' : '') + digit;
    }
  </script>
  )rawliteral";