# LAW_29_NAMING.md
## CommanderLink – Benennungs-, Nummern- & Artefakt-Identitäts-Gesetz

### Status
VERBINDLICH · NORMATIV · FREIGABEPFLICHTIG

---

## 0. Zweck

Dieses Gesetz definiert verbindlich:

- Benennung von Dateien, Verzeichnissen und Artefakten
- Nummern- und Versionsdisziplin
- Identität von Manifest/Runtime/Common/Tools/Source
- Regeln für Erweiterung ohne Drift

Ziel: keine Mehrdeutigkeit, keine Nebenwahrheiten, keine „Ordnung nach Gefühl“.

---

## 1. Dateinamen sind Verträge

Ein Dateiname muss Rolle und Ebene erkennen lassen.

Folgerungen:
- Mehrdeutige Namen sind verboten.
- Sammelnamen sind verboten.
- Umbenennung ist ein Zustandswechsel.

---

## 2. Gesetzesdateien

Gesetze folgen strikt:

- `LAW_XX_<THEMA>.md`
- `XX` ist zweistellig, aufsteigend, ohne Lücken im aktiven Stand.
- `<THEMA>` ist großgeschrieben, ASCII, `_` als Trenner.

Beispiele:
- `LAW_00_PHYSICS.md`
- `LAW_26_LAYOUT.md`

---

## 3. Manifest-Artefakte

Manifest-Dateien folgen strikt:

- Präfix `clm_` (CommanderLink Manifest)
- Suffix nach Rolle: `_terms`, `_base`, `_policy`
- Keine Runtime-Inhalte.

Beispiele (normativ):
- `clm_terms_*.h`
- `clm_base_*.h`
- `clm_policy_*.h`

Ein Manifest-Artefakt ist ungültig, wenn es Laufzeit-Wahrheit enthält.

---

## 4. Runtime-Artefakte

Runtime-Dateien folgen strikt:

- Präfix `clr_` (CommanderLink Runtime)
- Suffix nach Rolle: `_abi`, `_shm`, `_state`

Beispiele (normativ):
- `clr_abi_*.h`
- `clr_shm_*.h`
- `clr_state_*.h`

Runtime-Artefakte enthalten nur Laufzeit-Wahrheit, keine Policy.

---

## 5. Common-Artefakte

Common-Dateien folgen strikt:

- Präfix `clc_` (CommanderLink Common)
- Mechanik-orientierte Namen
- Keine Policy, keine Runtime-Zustände

Beispiele:
- `clc_time_*.h`
- `clc_bits_*.h`

---

## 6. Tools

Tools folgen strikt:

- Präfix `clt_` (CommanderLink Tool)
- Name beschreibt Ausgabe, nicht Methode.

Beispiele:
- `clt_abi_report`
- `clt_shm_dump`
- `clt_validator_*`

---

## 7. Source-Module

Source-Module folgen strikt:

- Verzeichnis = Rolle (`hal`, `core`, `flow`, `link`, `oracle`)
- Dateien = eindeutige Funktion, kein „utils“.

Beispiele:
- `hal_sensors.c`
- `link_backend_udp.c`

Sammeldateien sind verboten.

---

## 8. Keine Doppelwahrheit

Für jede Definition gilt:

- exakt ein Ort der Wahrheit
- alle anderen Orte referenzieren

Folgerungen:
- Duplikate sind verboten.
- „Kopie zur Sicherheit“ ist verboten.
- Divergenz ist ein Fehlerzustand.

---

## 9. Versionierung von Ständen

Ein aktiver Stand ist eindeutig:

- Ein LAW-Stand
- Ein Manifest-Stand
- Ein Runtime-Stand

Folgerungen:
- „latest“ ist verboten.
- Versionen sind explizit.
- Mischstände sind unzulässig.

---

## 10. Erweiterungsregeln

Neue Artefakte sind nur erlaubt, wenn:

- ihre Ebene klar ist (LAW/Manifest/Runtime/Common/Tools/Src)
- ihr Name Rolle ausdrückt
- sie keine bestehende Wahrheit duplizieren

---

## 11. Verbotene Namen

Explizit verboten sind:

- `misc`, `tmp`, `old`, `backup`, `test2`, `final_final`
- `helpers`, `common_utils`, `stuff`, `notes`
- `include/` als Sammelbecken
- unnummerierte LAW-Dateien

---

## 12. Schlussformel

Benennung ist Kontrolle.

Wer Namen verwässert,
erzeugt doppelte Wahrheit.

Mit diesem Gesetz sind Identitäten und Namen fixiert.
Der nächste Schritt ist das erste Manifest-Paket
unter `manifest/terms/`.

