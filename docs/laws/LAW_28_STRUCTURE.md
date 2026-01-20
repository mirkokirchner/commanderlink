# LAW_28_STRUCTURE.md
## CommanderLink – Projektverzeichnis- & Artefakt-Struktur-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert die **verbindliche Verzeichnisstruktur**
des CommanderLink-Projekts.

Die Struktur ist kein Ordnungswunsch,
sondern ein **architektonischer Vertrag** zwischen
Gesetzen, Ableitung und Implementierung.

---

## 1. Struktur folgt Ableitung

Die Projektstruktur spiegelt strikt die Ableitungsreihenfolge wider:

1. Gesetze
2. Manifeste (compile-time Wahrheit)
3. Runtime (Laufzeit-Wahrheit)
4. Mechanik
5. Werkzeuge
6. Implementierung
7. Betrieb

Eine Vermischung ist verboten.

---

## 2. Wurzelverzeichnis

Das Projekt besitzt genau ein Wurzelverzeichnis:

```

commanderlink/

```

Alles darunter ist Teil des Systems.
Externe Ablagen sind verboten.

---

## 3. Gesetzesebene (bereits vorhanden)

```

commanderlink/
└── law/
├── LAW_00_PHYSICS.md
├── LAW_01_CPU.md
├── …
└── LAW_28_STRUCTURE.md

```

Regeln:
- `law/` enthält **ausschließlich** normative Markdown-Gesetze.
- Keine Code-Dateien.
- Keine Header.
- Keine Erklärtexte außerhalb der Gesetze.

`law/` ist die oberste Autorität.

---

## 4. Manifest-Ebene (nächster Schritt)

```

commanderlink/
└── manifest/
├── base/
├── terms/
├── policy/
└── manifest.md

```

Normen:
- `manifest/` enthält **compile-time Wahrheit**.
- Keine Runtime-Zustände.
- Keine SHM-Layouts.
- Keine dynamischen Werte.

Unterverzeichnisse:

- `terms/`  
  Begriffe, Rollen, Zustände, Enums (rein semantisch)

- `base/`  
  Grundlegende Festlegungen (Endianness, Größenklassen, Klassen von Blöcken)

- `policy/`  
  Ableitbare Regeln (Budgets, Limits, Profile – ohne Messwerte)

---

## 5. Runtime-Ebene (Wahrheit zur Laufzeit)

```

commanderlink/
└── runtime/
├── abi/
├── shm/
├── state/
└── runtime.md

```

Normen:
- Runtime beschreibt **was ist**, nicht was gelten soll.
- Alle Layouts sind cacheline-exakt.
- Alle Größen sind bewiesen.

Unterverzeichnisse:

- `abi/`  
  ABI-Berichte, Größen-, Offset- und Layout-Beweise

- `shm/`  
  Shared-Memory-Strukturen (physikalische Wahrheit)

- `state/`  
  Zustandsdefinitionen und Übergänge (laufzeitrelevant)

---

## 6. Common-Mechanik

```

commanderlink/
└── common/
├── time/
├── math/
├── bits/
└── common.md

```

Normen:
- Nur mechanische Hilfsmittel.
- Kein Policy-Wissen.
- Kein Runtime-Zustand.

---

## 7. Tools & Verifikation

```

commanderlink/
└── tools/
├── abi-report/
├── shm-dump/
├── validators/
└── tools.md

```

Normen:
- Tools beweisen Gesetze.
- Tools ändern kein Verhalten.
- Tools sind Teil der Verifikation.

---

## 8. Implementierung

```

commanderlink/
└── src/
├── hal/
├── core/
├── flow/
├── link/
├── oracle/
└── src.md

```

Normen:
- Jedes Modul referenziert Gesetze.
- Kein Modul ohne gesetzliche Grundlage.
- Reihenfolge ist bedeutungsvoll.

---

## 9. Monitor & Betrieb

```

commanderlink/
└── monitor/
└── deploy/

```

Normen:
- Monitor ist passiver Leser.
- Deploy enthält keine Logik.
- Betrieb verändert keine Gesetze.

---

## 10. Verbotene Strukturen

Explizit verboten sind:

- `include/` als Sammelbecken
- `utils/` ohne klare Rolle
- `misc/`, `old/`, `tmp/`
- Vermischung von Manifest und Runtime
- Mehrere Wahrheitsorte

---

## 11. Erweiterungsregel

Neue Verzeichnisse sind nur erlaubt, wenn:

- ihre Rolle gesetzlich definiert ist,
- sie eindeutig einer Ebene zugeordnet sind,
- sie keine bestehende Ebene unterlaufen.

---

## 12. Schlussformel

Die Verzeichnisstruktur ist Architektur.

Wer Struktur verwässert,
verletzt Gesetze.

Mit diesem Gesetz ist der Projektkörper definiert.
Der nächste Schritt ist **Manifest/terms/**.

