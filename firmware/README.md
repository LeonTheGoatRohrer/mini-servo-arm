# MiniServoArm per WLAN steuern

Der ESP32-C3 an COM5 stellt einen eigenen WLAN-Zugangspunkt und eine Webseite bereit. Es sind keine weiteren Bibliotheken oder ein externer Router erforderlich.

- WLAN-Name: `MiniServoArm-C3`
- WLAN-Passwort: `<set locally in local_config.h>`
- Bedienseite: `http://192.168.42.1`

Die WLAN-Sendeleistung ist auf 5 dBm begrenzt. Am vorhandenen Board war der Zugangspunkt bei 19,5, 13 und 8,5 dBm unsichtbar; bei 5 dBm empfing der Computer ihn mit 55 % Signal. Die Firmware setzt diesen Wert bei jedem Start. Zum normalen Start nur RESET/EN drücken; BOOT während RESET führt in den Downloadmodus.

Telefon oder Computer mit diesem WLAN verbinden und die Adresse im Browser öffnen. Manche Telefone melden bei diesem WLAN „kein Internet“; die lokale Bedienseite funktioniert trotzdem. Nach dem Start der Firmware bleiben beide GPIOs ohne PWM, bis auf der Seite **Motoren aktivieren** gewählt wird. Dabei erhalten beide Servos zunächst 1500 µs. Ihre tatsächliche Stellung wird nicht zurückgelesen; das Aktivieren kann daher eine Bewegung auslösen.

Der linke Joystick bewegt den unteren Basisservo an GPIO6 nach links/rechts im erprobten Bereich 1450–1550 µs. Der rechte Joystick bewegt den oberen Schulterservo an GPIO7 ebenfalls horizontal nach links/rechts im Bereich 1500–2100 µs. Der untere Endwert 1500 µs entspricht der gemeldeten Armstellung von ungefähr 15° unter der Horizontalen; 2100 µs ist die Hitec-Grenze von +60° Servoweg und damit rechnerisch ungefähr +45° Armwinkel. **90° Armwinkel sind mit dieser mechanischen Nullstellung nicht erreichbar.** Der reale Winkel wird nicht gemessen.

Die Auslenkung des Joysticks regelt die Geschwindigkeit: an beiden Achsen maximal 12 µs pro 20 ms, also 600 µs pro Sekunde (laut Servo-Spezifikation ungefähr 60°/s). Das ist an der Basis zwölfmal und an der Schulter dreimal so schnell wie zuvor. Loslassen hält den letzten Sollwert. Falls Steuerbefehle ausbleiben, setzt die Firmware beide Geschwindigkeiten nach 350 ms auf null. **Stopp & halten** hält sofort; **Zur Ausgangsstellung** fährt beide Achsen auf 1500 µs; **PWM aus** nimmt beiden Servos das Haltemoment.

Die Servos benötigen eine geeignete externe Versorgung (HS-325HB: 4,8–6 V) und eine gemeinsame Masse mit dem ESP32-C3. Die physischen Anschläge und die tatsächlichen Winkel sind laut `../Prototyp/p1/NOTES.md` nicht kalibriert. Die verwendete Servo-Grenze stammt aus der aktuellen [Hitec-Spezifikation](https://www.hitecrcd.co.kr/wp-content/uploads/hitec-products/2019/c/HS-325HB_Specsheet(v2.1)_22L.pdf).
