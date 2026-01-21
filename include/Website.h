#ifndef WEBSITE_H
#define WEBSITE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Impact Monitor</title>
  <style>
    body { font-family: sans-serif; text-align: center; background-color: #121212; color: #e0e0e0; margin: 0; padding: 15px; }
    .card { background: #1e1e1e; padding: 15px; margin: 10px auto; max-width: 400px; border-radius: 12px; border: 1px solid #333; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }
    h2 { color: #00d4ff; margin-bottom: 15px; }
    .value { font-size: 1.5rem; font-weight: bold; color: #ffcc00; }
    .data-row { display: flex; justify-content: space-around; margin: 10px 0; font-family: monospace; }
    .btn { background: #00d4ff; border: none; color: #121212; padding: 12px 24px; border-radius: 8px; font-weight: bold; cursor: pointer; width: 100%; }
    .btn:active { background: #008fb3; }
    #logbox { text-align: left; height: 180px; overflow-y: auto; background: #000; color: #00ff00; padding: 12px; font-family: 'Courier New', monospace; font-size: 0.85rem; border-radius: 8px; margin-top: 15px; line-height: 1.4; border: 1px solid #444; }
  </style>
</head>
<body>
  <h2>Impact Dashboard</h2>
  
  <div class="card">
    <p>Temperatuur: <span id="temp" class="value">--</span> &deg;C</p>
    <div class="data-row">
      <span>X: <span id="ax">--</span></span>
      <span>Y: <span id="ay">--</span></span>
      <span>Z: <span id="az">--</span></span>
    </div>
  </div>

  <div class="card">
    <button class="btn" onclick="toggleLed()">LED AAN/UIT</button>
  </div>

  <div class="card">
    <strong>Systeem Log</strong>
    <div id="logbox">Wachten op verbinding...</div>
  </div>

<script>
  function toggleLed() {
    fetch("/toggle");
  }

  function updateData() {
    fetch('/data')
      .then(response => response.json())
      .then(json => {
        document.getElementById('temp').innerHTML = json.temp.toFixed(1);
        document.getElementById('ax').innerHTML = json.ax.toFixed(1);
        document.getElementById('ay').innerHTML = json.ay.toFixed(1);
        document.getElementById('az').innerHTML = json.az.toFixed(1);
        document.getElementById('logbox').innerHTML = json.log;
      })
      .catch(err => console.log("Verbindingsfout"));
  }

  // Update elke seconde (houdt de ESP8266 stabiel en de website snel)
  setInterval(updateData, 1000);
</script>
</body>
</html>
)rawliteral";
#endif