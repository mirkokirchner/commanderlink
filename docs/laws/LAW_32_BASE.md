# LAW_32_BASE.md
## CommanderLink – Basisannahmen-, Klassen- & Quantisierungs-Gesetz (Manifest/base)

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert den **Inhalt und die Grenzen**
der Manifest-Ebene `manifest/base/`.

`base/` ist der Ort, an dem **qualitative Klassen**
festgelegt werden, ohne physikalische Zahlen festzuschreiben.

Es ist die **Brücke** zwischen reinen Begriffen (`terms`)
und späterer physikalischer Konkretisierung (Runtime).

---

## 1. `base` ist keine Physik

`manifest/base/` enthält **keine** physikalischen Werte.

Verboten sind:
- Byte-Größen
- Cacheline-Zahlen
- Hertz, Sekunden, Watt
- MTU-Werte
- Limits, die gemessen werden müssen

Physik gehört ausschließlich in Runtime/ABI.

---

## 2. `base` definiert Klassen, nicht Werte

Erlaubt sind ausschließlich **diskrete Klassen**.

Beispiele (qualitativ):
- Größe: SMALL / MEDIUM / LARGE
- Nähe: LOCAL / NEAR / FAR
- Intensität: LOW / NORMAL / HIGH
- Kritikalität: OPTIONAL / IMPORTANT / CRITICAL

Diese Klassen besitzen **keine numerische Bedeutung** im Manifest.

---

## 3. Klassen sind total geordnet

Für jede Klassendefinition gilt:

- vollständige Ordnung
- keine Lücken
- keine Mehrdeutigkeit

Beispiel:
```

LOW < NORMAL < HIGH

```

Teilordnungen oder unordentliche Mengen sind verboten.

---

## 4. Klassen sind abgeschlossen

Eine Klassenmenge ist vollständig.

Folgerungen:
- Keine „sonstigen“ Klassen
- Keine Erweiterung zur Laufzeit
- Neue Klassen erfordern Manifest-Revision

---

## 5. Klassen sind interpretierbar, nicht messbar

Die Abbildung einer Klasse auf Physik erfolgt:

- ausschließlich in Runtime
- zustandsabhängig
- messbasiert

Manifest kennt **keine** Abbildungsfunktion.

---

## 6. Klassen sind kontextgebunden

Eine Klasse ist nur im definierten Kontext gültig.

Folgerungen:
- Keine Wiederverwendung von Klassen mit anderer Bedeutung
- Keine globalen „Universal-Klassen“
- Kontextwechsel erfordert neue Definition

---

## 7. `base` kennt keine Layouts

Explizit verboten in `base/`:

- Structs
- Offsets
- Alignments
- Größenannahmen
- Speichersegmente

`base` beschreibt **Eigenschaften**, nicht Speicher.

---

## 8. Trennung zu `policy`

`base` beschreibt **was existiert**,
`policy` beschreibt **wie entschieden wird**.

Folgerungen:
- `base` enthält keine Regeln
- `base` enthält keine Grenzwerte
- `policy` darf auf `base` referenzieren, nie umgekehrt

---

## 9. Naming-Regeln für `base/`

Dateien in `manifest/base/`:

- Präfix `clm_base_`
- Inhalt: Enums, symbolische Klassen
- Keine Abhängigkeit von Runtime oder Policy

Beispiele:
- `clm_base_size_class.h`
- `clm_base_criticality.h`

---

## 10. Validitätskriterium

Ein `base`-Artefakt ist gültig, wenn:

- jede Klasse qualitativ ist,
- keine physikalische Zahl enthalten ist,
- jede Klasse total geordnet ist,
- und keine Policy enthalten ist.

---

## 11. Schlussformel

`base` ist die letzte abstrakte Schicht
vor der physikalischen Wahrheit.

Wer hier Zahlen einführt,
vermischt Gesetz und Realität.

Mit diesem Gesetz ist die **zweite Manifest-Ebene**
vollständig definiert.

**Nächster erlaubter Schritt:**
`LAW_33_POLICY.md` – Ableitungsregeln ohne Physik.

