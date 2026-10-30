# jtext_table.sh — read a jText table from POSIX sh, before anything is built (sourced by bsir.sh).
#
#   jtext_rows FILE SECTION
#
# prints the data rows of section SECTION of the jText file FILE, one row per line, its field
# values separated by a TAB, in the order of the section's Fields block. It reads the full profile
# (=== Section: NAME === ... === Data === ... === End Data ===) and skips the header, the Fields
# block and comment lines (# or //). Each row must be a compact row from field 1 (" 1. ^~ a^b^c"),
# whole on one line, with an ASCII formatter (<splice><comment>); a comment on the row (the comment
# character with a space on each side) is dropped. Values are trimmed, as jText trims them. The
# values themselves must not hold a TAB.
#
# Setup/compilers.jtext and Setup/recipes.jtext are written for it: formatter ^~, one row per line,
# no multiline values. Only sh and POSIX awk are used (no gawk-only features).

jtext_rows() {
  awk -v want="$2" '
    function trim(s) { sub(/^[ \t]+/, "", s); sub(/[ \t]+$/, "", s); return s }
    /^[ \t]*=== Section: / {
      name = $0
      sub(/^[ \t]*=== Section: /, "", name); sub(/[ \t]*===[ \t]*$/, "", name)
      insec = (name == want); indata = 0; next
    }
    /^[ \t]*=== End Section ===/ { insec = 0; indata = 0; next }
    !insec { next }
    /^[ \t]*=== Data ===/ { indata = 1; next }
    /^[ \t]*=== End Data ===/ { indata = 0; next }
    !indata { next }
    /^[ \t]*(#|\/\/)/ { next }
    /^[ \t]*1\.[ \t]/ {
      line = $0
      sub(/^[ \t]*1\.[ \t]+/, "", line)
      sp = substr(line, 1, 1); cm = substr(line, 2, 1)
      data = substr(line, 3)
      sub(/^[ \t]+/, "", data)
      if (data == cm || substr(data, 1, 2) == cm " ") data = ""
      else { c = index(data, " " cm " "); if (c > 0) data = substr(data, 1, c - 1) }
      out = ""; first = 1
      while (1) {
        i = index(data, sp)
        v = (i > 0) ? substr(data, 1, i - 1) : data
        out = first ? trim(v) : out "\t" trim(v)
        first = 0
        if (i == 0) break
        data = substr(data, i + 1)
      }
      print out
    }
  ' "$1"
}

#   jtext_fields FILE SECTION
#
# prints the field names of section SECTION, one per line, in field order (its Fields block).
jtext_fields() {
  awk -v want="$2" '
    /^[ \t]*=== Section: / {
      name = $0
      sub(/^[ \t]*=== Section: /, "", name); sub(/[ \t]*===[ \t]*$/, "", name)
      insec = (name == want); inf = 0; next
    }
    /^[ \t]*=== End Section ===/ { insec = 0; inf = 0; next }
    !insec { next }
    /^[ \t]*=== Fields ===/ { inf = 1; next }
    /^[ \t]*=== End Fields ===/ { inf = 0; next }
    inf && /^[ \t]*[0-9]+\.[ \t]/ {
      line = $0
      sub(/^[ \t]*[0-9]+\.[ \t]+/, "", line)
      sp = substr(line, 1, 1)
      data = substr(line, 3)
      sub(/^[ \t]+/, "", data)
      while (substr(data, 1, 1) == sp) data = substr(data, 2)
      i = index(data, sp)
      v = (i > 0) ? substr(data, 1, i - 1) : data
      sub(/^[ \t]+/, "", v); sub(/[ \t]+$/, "", v)
      print v
    }
  ' "$1"
}

#   jtext_select FILE SECTION OUTCOLS [COLUMN VALUE]...
#
# prints OUTCOLS (one column name, or several joined by commas, printed TAB-separated) of every
# row of section SECTION whose COLUMNs hold exactly the VALUEs, in file order. An unknown column
# name is an error (exit 1).
jtext_select() {
  _jt_file=$1; _jt_sec=$2; _jt_out=$3; shift 3
  _jt_names=$(jtext_fields "$_jt_file" "$_jt_sec" | tr '\n' ' ')
  jtext_rows "$_jt_file" "$_jt_sec" | awk -F '\t' -v names="$_jt_names" -v out="$_jt_out" '
    BEGIN {
      n = split(names, nm, " ")
      for (i = 1; i <= n; i++) col[nm[i]] = i
      no = split(out, on, ",")
      for (i = 1; i <= no; i++)
        if (!(on[i] in col)) { print "jtext_select: no column " on[i] | "cat 1>&2"; bad = 1 }
      nc = 0
      for (i = 1; i + 1 < ARGC; i += 2) {
        if (!(ARGV[i] in col)) { print "jtext_select: no column " ARGV[i] | "cat 1>&2"; bad = 1 }
        nc++; ccol[nc] = col[ARGV[i]]; cval[nc] = ARGV[i + 1]
      }
      ARGC = 1
      if (bad) exit 1
    }
    {
      for (k = 1; k <= nc; k++) if ($(ccol[k]) != cval[k]) next
      s = $(col[on[1]])
      for (i = 2; i <= no; i++) s = s "\t" $(col[on[i]])
      print s
    }
  ' "$@"
}
