# CommanderLink SPEC: Execution Context 64B (Host)

## 1. Status
Normativ. Dieses Dokument definiert das einzige zulässige Host-Objekt für die Fastpath-Verarbeitung: `cl_exec_ctx_64_t`.

## 2. Physikalische Wahrheit
- Der Execution Context ist exakt 64 Byte groß (eine Cacheline).
- `raw[64]` ist die Normfläche. Feldsicht ist sekundär.
- Host-Order ist intern verbindlich.
- Wire-Endianness ist ausschließlich im Wire-Protokoll relevant.

## 3. Verbote
- Keine Pointer im Execution Context.
- Kein `packed`.
- Keine compilerabhängigen Layoutannahmen.
- Keine variablen Längenstrukturen im Context.

## 4. Ingest/Egress
- Ingest expandiert Wire-Header + lokale Metadaten deterministisch in `cl_exec_ctx_64_t`.
- Egress darf ausschließlich aus `cl_exec_ctx_64_t` plus separater Payload-Region erzeugen.
- Der Context ist nie der Payload selbst, sondern die Verarbeitungseinheit/Metadaten.

## 5. SIMD-Prinzip
- SIMD-first: Fastpath arbeitet primär auf `HOT` (16B) und den festen Kopf-Feldern.
- SIMD-Breite (SSE4/AVX2/AVX-512) ist eine Policy/Gate-Entscheidung; der Context bleibt invariant.

## 6. PHV / Zero-Transformation
- `payload_quantum`, `headroom`, `tailroom` sind deterministische Parameter.
- Zero-Transformation ist der Default: Keine impliziten Kopien/Reshapes.
- Transformationen (z.B. LZ4/FEC) sind explizit und müssen über Flags/Policy sichtbar sein.

## 7. Audit/Forensik
- AUX-Felder sind minimal und dürfen den Fastpath nicht destabilisieren.
- Detaillierte Forensik ist außerhalb des 64B-Contexts zu führen (History/Forensics-Segmente).

