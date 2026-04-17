#!/usr/bin/env python3
"""Convert test/dictionaries/*.txt into gettext .po files.

Input format (one entry per line):
    keyword|translation

Output format (gettext):

    msgid ""
    msgstr ""
    "Content-Type: text/plain; charset=UTF-8\n"
    "Language: <iso>\n"
    "X-SmartMet-Language-Name: <name>\n"
    "X-SmartMet-Full: <yes|no>\n"

    msgid "<keyword>"
    msgstr "<translation>"

The .po files are written next to the .txt sources so both remain available
during the migration. Once the C++ PO loader ships and is exercised in
production, the .txt files can be removed.
"""

from __future__ import annotations

import pathlib
import sys

SCRIPT_DIR = pathlib.Path(__file__).resolve().parent
DICT_DIR = SCRIPT_DIR.parent / "test" / "dictionaries"

# Only fi, sv, en are considered complete; everything else is partial.
FULLY_SUPPORTED = {"fi", "sv", "en"}

# Human-readable names for the X-SmartMet-Language-Name header.
LANG_NAMES = {
    "ar": "Arabic",
    "da": "Danish",
    "de": "German",
    "en": "English",
    "en-marine": "Marine English",
    "es": "Spanish",
    "et": "Estonian",
    "fi": "Finnish",
    "fr": "French",
    "id": "Indonesian",
    "it": "Italian",
    "ja": "Japanese",
    "ko": "Korean",
    "nl": "Dutch",
    "no": "Norwegian",
    "ru": "Russian",
    "sonera": "Sonera",
    "sv": "Swedish",
    "sw": "Swahili",
    "th": "Thai",
    "vi": "Vietnamese",
    "zh": "Chinese",
}


def po_escape(value: str) -> str:
    """Escape a string for inclusion inside a gettext "..." literal."""
    return value.replace("\\", "\\\\").replace('"', '\\"')


def convert(txt_path: pathlib.Path, po_path: pathlib.Path) -> int:
    iso = txt_path.stem
    name = LANG_NAMES[iso]
    full = "yes" if iso in FULLY_SUPPORTED else "no"

    seen: set[str] = set()
    entries: list[tuple[str, str]] = []
    duplicates = 0
    for lineno, line in enumerate(txt_path.read_text(encoding="utf-8").splitlines(), 1):
        if not line:
            continue
        if "|" not in line:
            sys.exit(f"{txt_path}:{lineno}: missing '|' delimiter")
        keyword, _, translation = line.partition("|")
        if not keyword:
            continue
        if keyword in seen:
            # Matches FileDictionary behaviour: std::map::insert keeps the
            # first occurrence. Skip subsequent duplicates silently-ish.
            duplicates += 1
            continue
        seen.add(keyword)
        entries.append((keyword, translation))
    if duplicates:
        print(
            f"  note: {txt_path.name} had {duplicates} duplicate keyword(s); "
            "kept first occurrence (same as FileDictionary)",
            file=sys.stderr,
        )

    with po_path.open("w", encoding="utf-8") as out:
        out.write(f"# SmartMet textgen dictionary: {name} ({iso})\n")
        out.write(f"# Generated from {txt_path.name} by tools/txt_to_po.py\n")
        out.write("#\n")
        out.write("# Keywords are opaque identifiers and must not be translated.\n")
        out.write("# Tokens in [brackets] are placeholders and must be kept as-is.\n")
        out.write('msgid ""\n')
        out.write('msgstr ""\n')
        out.write('"Project-Id-Version: smartmet-library-textgen\\n"\n')
        out.write('"MIME-Version: 1.0\\n"\n')
        out.write('"Content-Type: text/plain; charset=UTF-8\\n"\n')
        out.write('"Content-Transfer-Encoding: 8bit\\n"\n')
        out.write(f'"Language: {iso}\\n"\n')
        out.write(f'"X-SmartMet-Language-Name: {name}\\n"\n')
        out.write(f'"X-SmartMet-Full: {full}\\n"\n')
        for keyword, translation in entries:
            out.write("\n")
            out.write(f'msgid "{po_escape(keyword)}"\n')
            out.write(f'msgstr "{po_escape(translation)}"\n')

    return len(entries)


def main() -> None:
    txt_files = sorted(p for p in DICT_DIR.glob("*.txt") if p.stem in LANG_NAMES)
    if not txt_files:
        sys.exit(f"no .txt dictionaries found in {DICT_DIR}")

    total = 0
    for txt in txt_files:
        po = txt.with_suffix(".po")
        n = convert(txt, po)
        total += n
        print(f"  {txt.name:16s} -> {po.name:16s}  ({n} entries)")
    print(f"Wrote {len(txt_files)} .po files, {total} entries total")


if __name__ == "__main__":
    main()
