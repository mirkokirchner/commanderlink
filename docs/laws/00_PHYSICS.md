# LAW_00_PHYSICS.md
## CommanderLink – Gesetz der Physik (Grundgesetz)

**Status:** ENTWURF (normativ nach Freigabe)  
**Geltungsbereich:** Gesamtsystem (Design, Header, Runtime, Tools, Monitor)  
**Priorität:** Höchste – steht über allen weiteren LAWs

---

## 0. Zweck
Dieses Gesetz definiert die **oberste Wahrheit** für CommanderLink.  
Alle Entscheidungen sind **physikalisch deterministisch** zu treffen.  
Software dient der **Ausnutzung der Hardware**, nicht umgekehrt.

---

## 1. Rangordnung der Wahrheit
Bei Konflikten gilt strikt folgende Priorität (absteigend):

1. **Physikalische Realität**
   - Elektrische, thermische, zeitliche Grenzen
   - Cache-/TLB-/NUMA-Effekte, DMA-Geometrie
2. **Mikroarchitektur**
   - Pipeline, SIMD-Breite, Register, Prefetch, Power-States
3. **ABI-Stabilität**
   - Layout, Alignment, Größenverträge, Offsets
4. **Betriebssystem-Koexistenz**
   - Scheduling, Routing, I/O-Pfade
5. **Externe Standards**
   - POSIX, RFCs, Ethernet, TCP/UDP, API-Konventionen

**Ein Standard darf niemals die Physik überstimmen.**

---

## 2. Plattform-Fixierung
- Zielarchitektur: **x86_64**, little-endian
- Hersteller: **Intel & AMD**
- Annahmen:
  - Cacheline = **64 Byte**
  - Registerbreite ≥ **64 Bit**
- Andere Architekturen sind **nicht normativ**, nur optional adaptierbar.

---

## 3. Zero-Transformation-Prinzip
- Keine impliziten Umrechnungen, kein „Parsing-Overhead“.
- Datengeometrie ist **explizit**:
  - Wire → Execution Context → Register ist definiert.
- Transformation ist **verboten**, außer:
  - explizit genehmigt
  - physikalisch notwendig
  - im LAW referenziert

---

## 4. Cacheline-Souveränität
- **64-Byte-Normfläche** ist das atomare Bauelement.
- Alle ABI-relevanten Strukturen:
  - sind Vielfache von 64 Byte
  - enthalten explizites Padding
  - besitzen `raw[64]`-Union oder äquivalente Normfläche
- **False Sharing ist ein Fehler**, kein Optimierungsfall.

---

## 5. Maximierungsprinzip
- **Maximum ist das Minimum.**
- CL nutzt **immer** die maximal physikalisch mögliche Leistung,
  **sofern** alle GATES offen sind.
- Drosselung erfolgt **nur** bei:
  - Thermal / VRM / Power-Gates
  - Interaktivitäts-Gates
  - Sicherheits-/Trust-Gates

---

## 6. Energie & Thermik
- Energieverbrauch ist **kein Makel**, sondern Messgröße.
- Schutzmechanismen:
  - Hardware-gesteuert (Throttling)
  - softwareseitig sichtbar (Gates)
- Software darf **nicht** künstlich bremsen,
  sondern **reaktiv** auf physikalische Grenzen reagieren.

---

## 7. SIMD-Grundsatz
- SIMD ist **Baseline**, kein Sonderfall.
- Leiter:
  - SSE4 → AVX2 → AVX-512
- Nutzung nur, wenn:
  - Effizienzgewinn real
  - Isolation möglich
  - GATES offen
- Sporadischer SIMD-Einsatz in Latenzpfaden ist **verboten**.

---

## 8. Koexistenz statt Dominanz
- CL ist **Symbiont**, kein Eroberer.
- OS bleibt souverän.
- CL darf:
  - Kerne pinnen
  - Ressourcen reservieren
- CL darf **nicht**:
  - Kernel ersetzen
  - Default-Routen kapern
  - OS destabilisieren

---

## 9. Auditierbarkeit
- Jeder physikalische Zustand ist:
  - sichtbar
  - messbar
  - reproduzierbar
- „Best effort“ ist **markierter Ausnahmezustand**, nie Default.

---

## 10. Ableitungsregel
Jedes weitere LAW muss:
- dieses Gesetz **explizit referenzieren**
- keine Regel einführen, die ihm widerspricht
- physikalisch begründet sein

---

## 11. Schlussformel
CommanderLink folgt **keinen Moden**.  
CommanderLink folgt **keinen Dogmen**.  
CommanderLink folgt **der Physik**.

**PHYSICS FIRST. x86_64 FIRST.**

