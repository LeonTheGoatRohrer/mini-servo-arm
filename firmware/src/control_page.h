#pragma once

#include <pgmspace.h>

static const char CONTROL_PAGE[] PROGMEM = R"HTML(
<!doctype html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
  <title>MiniServoArm · Steuerung</title>
  <style>
    :root { color-scheme: light; font-family: system-ui, -apple-system, Segoe UI, sans-serif; }
    * { box-sizing: border-box; }
    body { margin: 0; min-height: 100vh; background: #e9eee9; color: #172823; }
    .wrap { max-width: 870px; margin: auto; padding: 24px 18px 40px; }
    header { display: flex; align-items: flex-start; justify-content: space-between; gap: 16px; margin-bottom: 22px; }
    h1 { margin: 0 0 4px; font-size: clamp(1.7rem, 5vw, 2.4rem); letter-spacing: -.04em; }
    h2 { margin: 0 0 6px; font-size: 1.15rem; }
    p { margin: 0; line-height: 1.45; }
    .sub { color: #52665d; font-size: .93rem; }
    .badge { flex: none; margin-top: 6px; border-radius: 99px; padding: 8px 12px; background: #d8e0da; color: #375044; font-size: .8rem; font-weight: 700; }
    .badge.on { background: #c6eddb; color: #12603f; }
    .badge.error { background: #f8d8cf; color: #8d3727; }
    .notice { padding: 14px 17px; margin-bottom: 19px; border-left: 4px solid #d48d38; background: #fff7eb; color: #664b2a; border-radius: 12px; font-size: .9rem; }
    .actions { display: flex; flex-wrap: wrap; gap: 9px; margin-bottom: 22px; }
    button { appearance: none; border: 0; border-radius: 12px; padding: 12px 16px; background: #d3dfd8; color: #1a392d; font: inherit; font-weight: 700; cursor: pointer; }
    button:disabled { opacity: .45; cursor: not-allowed; }
    button.primary { background: #176747; color: #fff; }
    button.stop { background: #f1d7d0; color: #862f20; }
    button.off { background: transparent; border: 1px solid #aab8af; color: #43584b; }
    .grid { display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 16px; }
    .card { background: #fff; border: 1px solid #d3dfd7; box-shadow: 0 7px 26px #1637230d; border-radius: 21px; padding: 19px; text-align: center; }
    .card .sub { min-height: 42px; }
    .pad { width: min(230px, 100%); aspect-ratio: 1; border-radius: 50%; margin: 21px auto 18px; position: relative; touch-action: none; user-select: none; background: radial-gradient(circle at center, #f4f7f2 0 16%, #e0eae3 17% 18%, #f4f7f2 19% 53%, #d5e1d8 54% 55%, #edf3ee 56%); border: 2px solid #c7d8cc; box-shadow: inset 0 4px 13px #26523b1a; }
    .pad::before, .pad::after { content: ""; position: absolute; background: #b6c9ba; }
    .pad::before { left: 50%; top: 11%; width: 1px; height: 78%; }
    .pad::after { top: 50%; left: 11%; height: 1px; width: 78%; }
    .thumb { position: absolute; left: 50%; top: 50%; width: 66px; height: 66px; border-radius: 50%; transform: translate(-50%, -50%); background: #176747; border: 5px solid #fff; box-shadow: 0 6px 17px #193e2d55; pointer-events: none; }
    .card.shoulder .thumb { background: #b66a31; }
    .reading { display: flex; justify-content: center; align-items: baseline; gap: 5px; font-size: 1.35rem; font-weight: 750; font-variant-numeric: tabular-nums; }
    .reading small { font-size: .75rem; color: #6c7c72; font-weight: 600; }
    .range { margin-top: 4px; color: #718175; font-size: .78rem; }
    footer { color: #637469; font-size: .8rem; line-height: 1.45; margin-top: 22px; }
    @media (max-width: 600px) { .grid { grid-template-columns: 1fr; } .card { padding: 18px 13px; } .card .sub { min-height: 0; } .pad { width: 180px; margin: 15px auto; } .wrap { padding-top: 18px; } }
  </style>
</head>
<body>
  <main class="wrap">
    <header>
      <div><h1>MiniServoArm</h1><p class="sub">Direktsteuerung über das WLAN des ESP32-C3</p></div>
      <span class="badge" id="status">Verbinde …</span>
    </header>
    <div class="notice">Vor „Motoren aktivieren“ den Bewegungsraum freihalten: Beide Servos erhalten dabei zunächst 1500 µs und können sich zur Ausgangsstellung bewegen. Loslassen hält die aktuelle Position.</div>
    <div class="actions">
      <button id="arm" class="primary">Motoren aktivieren</button>
      <button id="stop" class="stop" disabled>Stopp &amp; halten</button>
      <button id="center" disabled>Zur Ausgangsstellung</button>
      <button id="off" class="off" disabled>PWM aus</button>
    </div>
    <section class="grid" aria-label="Zwei Joysticks">
      <div class="card">
        <h2>Basis · GPIO6</h2>
        <p class="sub">Joystick nach links oder rechts halten</p>
        <div class="pad" id="basePad" role="group" aria-label="Basis-Joystick"><div class="thumb" id="baseThumb"></div></div>
        <div class="reading"><span id="baseValue">1500</span><small>µs</small></div>
        <div class="range">Begrenzt auf 1450–1550 µs</div>
      </div>
      <div class="card shoulder">
        <h2>Schulter · GPIO7</h2>
        <p class="sub">Joystick nach links oder rechts halten</p>
        <div class="pad" id="shoulderPad" role="group" aria-label="Schulter-Joystick"><div class="thumb" id="shoulderThumb"></div></div>
        <div class="reading"><span id="shoulderValue">1500</span><small>µs</small></div>
        <div class="range">Begrenzt auf 1500–2100 µs</div>
      </div>
    </section>
    <footer>Der ESP32-C3 misst keinen Gelenkwinkel. Die Schultergrenze entspricht höchstens ungefähr +45° Armwinkel, wenn die Ausgangsstellung bei −15° liegt. Bei Verbindungsabbruch endet die Bewegung nach spätestens 350 ms; die Servos halten dann den letzten Sollwert.</footer>
  </main>
  <script>
    const statusEl = document.getElementById('status');
    const armBtn = document.getElementById('arm');
    const stopBtn = document.getElementById('stop');
    const centerBtn = document.getElementById('center');
    const offBtn = document.getElementById('off');
    const desired = { b: 0, s: 0 };
    let armed = false;
    let lastSend = 0;
    let stopLatch = false;

    async function post(path, body = '') {
      const response = await fetch(path, {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body,
        cache: 'no-store'
      });
      if (!response.ok) throw new Error(await response.text());
      return response.json();
    }

    function showState(data) {
      armed = data.armed;
      statusEl.textContent = armed ? (data.centering ? 'Fährt zur Mitte' : 'Motoren aktiv') : 'Motoren aus';
      statusEl.className = 'badge' + (armed ? ' on' : '');
      armBtn.disabled = armed;
      stopBtn.disabled = !armed;
      centerBtn.disabled = !armed;
      offBtn.disabled = !armed;
      document.getElementById('baseValue').textContent = data.baseUs;
      document.getElementById('shoulderValue').textContent = data.shoulderUs;
    }

    function connectionError() {
      statusEl.textContent = 'Verbindung verloren';
      statusEl.className = 'badge error';
      armed = false;
      stopLatch = true;
      resetPads();
    }

    async function refresh() {
      try {
        const response = await fetch('/api/status', { cache: 'no-store' });
        if (!response.ok) throw new Error('Status nicht erreichbar');
        showState(await response.json());
      } catch (_) { connectionError(); }
    }

    function sendDrive(force = false) {
      if (!armed) return;
      const now = Date.now();
      if (!force && now - lastSend < 70) return;
      lastSend = now;
      const body = new URLSearchParams({ b: String(desired.b), s: String(desired.s) });
      post('/api/drive', body).catch(connectionError);
    }

    function resetPads() {
      desired.b = 0;
      desired.s = 0;
      document.getElementById('baseThumb').style.transform = 'translate(-50%, -50%)';
      document.getElementById('shoulderThumb').style.transform = 'translate(-50%, -50%)';
    }

    function attachPad(padId, thumbId, axis) {
      const pad = document.getElementById(padId);
      const thumb = document.getElementById(thumbId);
      let pointer = null;
      function update(event) {
        const rect = pad.getBoundingClientRect();
        const radius = rect.width * .35;
        const delta = event.clientX - rect.left - rect.width / 2;
        let value = Math.max(-1, Math.min(1, delta / radius));
        if (Math.abs(value) < .14) value = 0;
        desired[axis] = Math.round(value * 100);
        const dx = value * radius;
        thumb.style.transform = `translate(-50%, -50%) translate(${dx}px, 0px)`;
        sendDrive();
      }
      pad.addEventListener('pointerdown', event => {
        if (!armed) return;
        stopLatch = false;
        pointer = event.pointerId;
        pad.setPointerCapture(pointer);
        update(event);
      });
      pad.addEventListener('pointermove', event => {
        if (event.pointerId === pointer && !stopLatch) update(event);
      });
      function release(event) {
        if (event.pointerId !== pointer) return;
        pointer = null;
        desired[axis] = 0;
        thumb.style.transform = 'translate(-50%, -50%)';
        sendDrive(true);
      }
      pad.addEventListener('pointerup', release);
      pad.addEventListener('pointercancel', release);
      pad.addEventListener('lostpointercapture', release);
    }

    attachPad('basePad', 'baseThumb', 'b');
    attachPad('shoulderPad', 'shoulderThumb', 's');
    armBtn.onclick = async () => {
      if (!confirm('Bewegungsraum frei? Beim Aktivieren fahren beide Servos gegebenenfalls zu 1500 µs.')) return;
      try { showState(await post('/api/arm')); } catch (error) { alert(error.message); }
    };
    stopBtn.onclick = async () => {
      stopLatch = true;
      resetPads();
      try { showState(await post('/api/stop')); } catch (_) { connectionError(); }
    };
    centerBtn.onclick = async () => {
      stopLatch = true;
      resetPads();
      try { showState(await post('/api/center')); } catch (_) { connectionError(); }
    };
    offBtn.onclick = async () => {
      if (!confirm('PWM abschalten? Beide Servos verlieren ihr Haltemoment.')) return;
      stopLatch = true;
      resetPads();
      try { showState(await post('/api/off')); } catch (_) { connectionError(); }
    };
    window.addEventListener('blur', () => { resetPads(); sendDrive(true); });
    document.addEventListener('visibilitychange', () => {
      if (document.hidden) { resetPads(); sendDrive(true); }
    });
    setInterval(() => { if (armed && (desired.b || desired.s)) sendDrive(true); }, 100);
    setInterval(refresh, 500);
    refresh();
  </script>
</body>
</html>
)HTML";
