# Transitioning from localized database translations

This guide is for SmartMet deployments that have been running with the
database dictionary backend (`multimysql`, `multipostgresql`,
`multimysqlplusgeonames`, `multipostgresqlplusgeonames`) and have edited
translation rows directly in the database. The classic example is a
country-specific deployment — e.g. Latvia, where local meteorologists
improved the Latvian translations in `textgen.translation_lv` without
sending the fixes upstream.

From the PO-dictionaries release onward, the authoritative source of
translations is a set of gettext `.po` files on disk. The DB backend is
still accepted but is deprecated and will be removed after one release
cycle. This guide shows how to move to `.po` files **without losing your
local edits**.

## Overview

The transition is a three-step process:

1. Export your database translations to a `.po` file.
2. Merge those translations into the upstream `.po` file shipped by
   `smartmet-library-textgen-dictionary-<lang>`.
3. Switch the server configuration from `multipostgresqlplusgeonames` (or
   similar) to `multipoplusgeonames`.

The merge step is non-destructive: your translations stay in place, and
missing entries are backfilled from upstream. You keep your edits, you
gain any new keywords that upstream has added, and you no longer need
the database.

## Prerequisites

* The upstream `.po` file for your language — either from the
  `smartmet-library-textgen-dictionary-<lang>` RPM once installed,
  or from `test/dictionaries/<lang>.po` in the
  [smartmet-library-textgen](https://github.com/fmidev/smartmet-library-textgen)
  source tree.
* `psql` (or `mysql`) access to the production database used by textgen,
  with read permission on the `textgen.translation_<lang>` table.
* The GNU gettext tools (`msginit`, `msgmerge`, `msgcat`, `msgfmt`). On
  RHEL / Fedora: `dnf install gettext`.

The procedure below uses PostgreSQL; for MySQL the SQL query is
effectively identical, only the client invocation differs.

## Step 1. Export database translations to a `.po` file

Run this as a script on a machine with psql access. It dumps the
`translation_<lang>` table as a `.po` fragment preserving every
(keyword, translation) pair.

```bash
LANG_CODE=lv                       # or sonera, en-marine, ...
DB_HOST=hostname.fmi.fi
DB_USER=weatherproof_rw
DB_NAME=weatherproof
OUT=from-db-${LANG_CODE}.po

{
  printf 'msgid ""\n'
  printf 'msgstr ""\n'
  printf '"Content-Type: text/plain; charset=UTF-8\\n"\n'
  printf '"Language: %s\\n"\n\n' "$LANG_CODE"
  psql -h "$DB_HOST" -U "$DB_USER" -d "$DB_NAME" -At -F '|' -c \
    "SELECT keyword, translation FROM textgen.translation_${LANG_CODE//-/_}
     ORDER BY keyword;" |
  awk -F'|' '{
    gsub(/\\/, "\\\\", $1); gsub(/"/, "\\\"", $1);
    gsub(/\\/, "\\\\", $2); gsub(/"/, "\\\"", $2);
    printf "msgid \"%s\"\nmsgstr \"%s\"\n\n", $1, $2
  }'
} > "$OUT"

msgfmt --check --output-file=/dev/null "$OUT" && echo "ok: $OUT"
```

The resulting `from-db-lv.po` is a valid gettext catalog containing
exactly what is in your production database — including any local fixes
you applied directly through SQL.

## Step 2. Merge with the upstream `.po`

Use `msgcat` to combine upstream and local, with **your local
translations winning any conflicts**. This is the step that preserves
the Latvian (or other locale) edits:

```bash
UPSTREAM=/usr/share/smartmet/textgen/dictionaries/${LANG_CODE}.po
#     or textgen/test/dictionaries/${LANG_CODE}.po from the source tree
LOCAL=from-db-${LANG_CODE}.po
MERGED=${LANG_CODE}.po

msgcat --use-first "$LOCAL" "$UPSTREAM" > "$MERGED"
msgfmt --check --output-file=/dev/null "$MERGED" && echo "ok: $MERGED"
```

Notes:

* `--use-first` means: for any `msgid` present in both, the translation
  from the *first* file listed (`$LOCAL`) is kept. Put your local export
  first and upstream second.
* `msgcat` also brings in any keywords that exist only upstream (newly
  added in a recent release), giving you a complete catalog.
* The merged file keeps the original `X-SmartMet-Language-Name` and
  `X-SmartMet-Full` headers from upstream; if you want to mark the
  catalog as partial, edit the header manually.

### Reviewing what changed

Before deploying, it's worth seeing which entries were overridden by the
local database copy. A quick way:

```bash
diff <(msgattrib --translated "$UPSTREAM")  \
     <(msgattrib --translated "$MERGED")    | less
```

Entries that appear only in the merged file are your local edits. This
is also the list you should consider contributing upstream (see
"Contributing your edits back" below).

## Step 3. Switch the server to the PO backend

Install the `.po` file to the directory the plugin reads from:

```bash
sudo install -m 0644 "$MERGED" \
    /usr/share/smartmet/textgen/dictionaries/${LANG_CODE}.po
```

Then edit your textgen plugin config (typically
`/etc/smartmet/plugins/textgen.conf`). Change:

```libconfig
dictionary = "multipostgresqlplusgeonames";
```

to:

```libconfig
dictionary = "multipoplusgeonames";
```

and add the dictionary directory:

```libconfig
podictionaries = "/usr/share/smartmet/textgen/dictionaries";
```

Restart the SmartMet server. On startup the textgen plugin scans
`podictionaries` and discovers every `<lang>.po` present; no central
registry needs updating.

Once you are satisfied the PO path is working, you can drop the
`textgen::host`, `textgen::user`, `textgen::passwd`, and related
database settings from the plugin config. They are no longer consulted.

## Verifying the transition

Two quick sanity checks:

1. The plugin should log deprecation warnings only if some config still
   uses `multimysql` / `multipostgresql`; after the switch those should
   disappear.
2. Generate a sample forecast for your language and compare it to the
   output you were getting with the database backend. Identical output
   means the merge preserved every translation correctly.

A minimal comparison without standing up the full server:

```bash
qdtext -c /etc/smartmet/textgen/qdtext.conf -l ${LANG_CODE} > new.txt
diff old.txt new.txt
```

## Contributing your edits back

Local translation improvements benefit everyone running the same
language. Once the merge is stable in production, please submit your
`.po` changes upstream so other deployments inherit them:

1. Fork
   [smartmet-library-textgen](https://github.com/fmidev/smartmet-library-textgen).
2. Replace `test/dictionaries/<lang>.po` with your merged file.
3. Open a pull request against `develop`. Describe what domain the
   translations come from (national weather service, marine, aviation…)
   and who contributed them.

After the PR is merged, the next
`smartmet-library-textgen-dictionary-<lang>` RPM release will carry
your improvements and future installs will pick them up automatically
— no manual merge step required.

## What happens if I do nothing?

You can leave the `multipostgresqlplusgeonames` setting in place for
now. The database backend continues to work but emits a one-time
`WARNING: textgen dictionary backend '...' is deprecated` log line on
server startup. The classes `PostgreSQLDictionary`, `MySQLDictionary`,
and `DatabaseDictionaries` will be removed after the following release
of `smartmet-library-textgen`; after that release the server will
refuse to start with a database `dictionary` setting and you must have
migrated to `.po` files.
