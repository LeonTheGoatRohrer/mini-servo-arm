# Roboterarm – Konzeptprototyp

## Dateien und Bedienung
- `main.kcl`: montierte Baugruppe mit genau zwei Servo-Einbaureferenzen, kein Greifer.
- `baseAngle`: Basisrotation um Z; `shoulderAngle`: Armwinkel über der Horizontalen. Anzeigestellung 0° / 35°.
- `baseAxis` und `shoulderAxis`: bewusst sichtbare Konstruktionslinien, keine Druckkörper.
- `printLayout.kcl`: acht getrennte Kunststoffteile ohne Servos, Hörner, Schrauben, Muttern und Stützachse. Anordnung ist keine zugesicherte Druckbettbelegung; Teile bei Bedarf einzeln slicen.
- Einzelteile liegen in eigenen KCL-Dateien. Wiederholte Grundprofile entstehen über vollständig bemaßte Solver-Funktionen in `parameters.kcl`; deren innere Skizzen sind nicht per Point-and-click editierbar.

## Parameter und Annahmen
- Gehäuse-Nennhülle HS-325HB: Länge 40, Breite 20, Höhe 36,5 mm (zuvor nachgeschlagene Herstellerangabe). Kein detailgetreues Hersteller-CAD.
- `servoHolePitch = 47.8`, `servoHoleRow = 10.2` mm: vorläufig aus der zuvor betrachteten Maßzeichnung übernommen; am vorhandenen Servo inklusive Gummitüllen prüfen.
- Annahmen: Abtrieb 10 mm vom kurzen Gehäuseende; Laschenlänge 53 mm, Dicke 2,5 mm, Lage 10 mm unter Gehäuseoberseite. Kabelausgang, Verzahnung und Gummitüllen nicht modelliert.
- `hornPitch = 16` mm: ausdrücklich erfundenes Beispiel-Lochbild (zwei Löcher), Horn Ø24 mm. Vor echtem Betrieb an vorhandenes Originalhorn anpassen. Keine gedruckte Abtriebsverzahnung.
- `armLength = 75` mm: Konstruktionslänge bis zum Endstegbereich, tatsächliche Adapterfläche 87 mm von der Schulterachse.
- `plateThickness = 4` mm, Armwangen 5 mm. `screwSize = 3`, `screwClearance = 3.4` mm; Druckpassung nicht kalibriert.
- Greiferplatte: 64 × 30 × 4 mm, vier Durchgänge Ø3,4 mm auf `gripperPitchY × gripperPitchZ = 40 × 16` mm. Platte mit langen Beispielschrauben und vereinfachten runden Muttern abnehmbar. Kein genormtes Greiferinterface.
- Separate Stützachse Ø5 mm; Lagerbohrung Ø5,4, Armbohrung Ø5,2 mm. Passung und axiale Sicherung sind noch festzulegen.
- Gleitauflage mit integrierten Abstandshaltern: 0,2 mm nominaler Abstand zum Drehteller. Auflagehöhe durch Unterlegen/Anpassen abstimmen; ersetzt kein ausgelegtes Wälzlager. Ein Stützfuß ist bewusst seitlich versetzt, damit die Servolasche frei bleibt.
- Servoänderungen erfordern neben Gehäuseparametern auch Prüfung der Hüllmaße, Achshöhen und Abstände in `main.kcl`; kein universeller automatischer Servoadapter.

## Mechanik und Anschläge
- Basisservo bleibt fest; sein Horn dreht Teller, beide Schulterhalter, Schulterservo und Arm.
- Schulterservo bleibt im mitdrehenden Träger; sein Horn dreht nur Arm und Adapter. Gegenlager stützt die andere Armwange.
- Anschlag-Vorbereitung, noch keine wirksamen Endanschläge: zwei Ø3,4-Bohrungen am Drehteller bei lokal x=±12, y=-40 mm; zwei am Gegenlager bei lokal x=±9, z=22 mm. Daran lassen sich einstellbare Anschlagwinkel befestigen. Gegenstücke und endgültige Winkel erst nach Servo-Kalibrierung festlegen.
- Geplanter Bereich: Basis ±60°, Schulter −20° bis +100°. Das sind Entwurfsgrenzen, keine freigegebenen PWM-Endpunkte.

## Prüfung und offene Punkte
- Einzelteile separat ausgeführt; alle geprüften Solver-Skizzen vollständig bestimmt. Aussparungen und Bohrungen sind echte Subtraktionen.
- Baugruppe bei 0°/35°, −60°/−20° und +60°/+100° ausgeführt und visuell kontrolliert. Keine offensichtliche Arm-/Halterkollision in diesen Stellungen. Keine kontinuierliche Kollisionssimulation oder gemessene Mindestabstandsprüfung; Kabel und reale Schraubenköpfe können den Bereich reduzieren.
- Geometrische Einbauprüfung führte zur seitlichen Versetzung des rechten Ringstützfußes weg von der Basis-Servolasche.
- Schrauben/Muttern sind beispielhaft, nicht vollständige Einkaufsliste. Servohorn-Zentralschrauben, restliche Befestigungsschrauben, Lager-Axialsicherung und gegebenenfalls Gewindeeinsätze fehlen als Detailgeometrie.
- Schwachstellen: Schulterdrehmoment mit zusätzlichem Greifergewicht, dünne Armstege und Lochränder am Endsteg, Fußübergänge der aufrechten Halter, Verschleiß/Spiel der Drucklager und Reibung des Drehtellers. Keine garantierte Nutzlast oder Funktion.
- Drucklayout ist nicht gesliced oder DFM-zertifiziert. Insbesondere die große Fensterbrücke des Schulterhalters braucht eine geeignete Ausrichtung bzw. Stützstrategie; Schichthaftung und Belastungsrichtung am Arm praktisch prüfen.

## Nächster sinnvoller Schritt
Zuerst einen Halter und die Horn-Anschlusszone als Passprobe drucken; danach Lochbild, Tüllen, Kabelweg, Lagerpassung und Anschläge am echten Servo abstimmen, bevor eine Greifereinheit ergänzt wird.
