# CommanderLink – MEMORY MODEL

## Ziel
Beschreibung des Speicher- und Cache-Modells von CommanderLink.

---

## 1. Cacheline-Primat

- 1 Cacheline = 64 Byte
- Jede Hot-Struktur passt exakt in eine Cacheline
- Kein Objekt überschreitet eine Cacheline ohne Notwendigkeit

---

## 2. Hot / Warm / Cold Trennung

### Hot
- Wird häufig geschrieben
- Atomics erlaubt
- Eigene Cacheline

### Warm
- Periodisch aktualisiert
- Keine Atomics

### Cold
- Statisch / selten geändert
- Strings / Metadaten

---

## 3. False Sharing Verbot

- Keine zwei schreibenden Cores auf derselben Cacheline
- Layout erzwingt Trennung

---

## 4. Zero-Transformation-Prinzip

- Daten werden exakt so verarbeitet, wie sie im Register liegen
- Keine zusätzlichen Kopien
- Keine Umformatierung
- Keine Shifts im Fast-Path

---

## 5. Register-Geometrie

- AVX-512: 64 Byte = 1 ZMM
- CL-Atom = Register-Atom
- Cacheline == Registerbreite

---

## 6. Speicher als Staudamm

- RAM darf aggressiv als Puffer genutzt werden
- Credits bestätigen RAM-Eingang, nicht Disk-Commit
- Disk-I/O entkoppelt vom Netz

---

## 7. Merksatz

> **Die CPU darf niemals auf Speicher warten.**

