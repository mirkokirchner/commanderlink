#!/bin/sh

# ==========================================
# CONFIGURATION & PHYSICS SETUP
# ==========================================
OUTFILE="projectdump.txt"
SCRIPT_NAME=$(basename "$0")

# Auto-Backend-Selection: Check I/O Capabilities
# We prepare the file creation (truncation)
: > "$OUTFILE"

echo "----------------------------------------"
echo "Initializing Dump... (Target: $OUTFILE)"
echo "----------------------------------------"

# ==========================================
# FUNCTION: PROCESS FILES (The Core Loop)
# ==========================================
# We use a process substitution loop to keep variable scope and optimize I/O stream
# 'find' is used for raw filesystem access, skipping .git and the output file itself.

find . -type f \
    -not -path '*/.git/*' \
    -not -name "$OUTFILE" \
    -not -name "$SCRIPT_NAME" \
    -not -name "*.DS_Store" \
    | sort | while read -r file; do

    # ABSOLUTE SAFETY CHECK:
    # Check if file is binary. If it is, we skip content printing to avoid 
    # corrupting the text dump with non-printable characters.
    # Uses grep logic to detect NUL bytes.
    if grep -Iq . "$file"; then
        echo "Processing: $file"
        
        # Write Header
        echo "" >> "$OUTFILE"
        echo ">>> START FILE: $file" >> "$OUTFILE"
        
        # Stream Content
        cat "$file" >> "$OUTFILE"
        
        # Write Footer
        echo "" >> "$OUTFILE"
        echo "<<< END FILE: $file" >> "$OUTFILE"
    else
        # Handle Binary Files gracefully
        echo "Skipping binary: $file"
        echo "" >> "$OUTFILE"
        echo ">>> START FILE: $file (BINARY SKIPPED)" >> "$OUTFILE"
        echo "[Binary Content Hidden]" >> "$OUTFILE"
        echo "<<< END FILE: $file" >> "$OUTFILE"
    fi

done

# ==========================================
# FUNCTION: GENERATE TREE
# ==========================================
echo "" >> "$OUTFILE"
echo "========================================" >> "$OUTFILE"
echo " PROJECT STRUCTURE TREE" >> "$OUTFILE"
echo "========================================" >> "$OUTFILE"

# Auto-Backend-Selection for Tree Generation
if command -v tree &> /dev/null; then
    # Turbo Backend: Use native optimized binary
    tree -a -I '.git|projectdump.txt' >> "$OUTFILE"
else
    # Standard Backend: Mathematical calculation of tree structure via sed
    find . -print | sed -e 's;[^/]*/;|____;g;s;____|; |;g' >> "$OUTFILE"
fi

echo "Done. All data saved to $OUTFILE"
