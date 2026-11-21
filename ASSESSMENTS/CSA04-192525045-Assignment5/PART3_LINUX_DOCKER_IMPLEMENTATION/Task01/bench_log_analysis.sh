#!/usr/bin/env bash
# Measure how the log_analysis.sh pipeline scales with log size (supports complexity_analysis.md).
# Uses the same awk | sort | uniq stages as log_analysis.sh on generated logs in the same format.
# Usage: bench_log_analysis.sh   (prints a table; takes about a minute)
set -euo pipefail
tmp=$(mktemp -d); trap 'rm -rf "$tmp"' EXIT

gen() {  # gen <lines> <file> : deterministic synthetic log, 200 distinct client IPs
    awk -v n="$1" 'BEGIN{
        split("200 200 200 200 302 403 404 500", st, " ")
        for (i = 0; i < n; i++)
            printf "10.0.%d.%d - - [08/Oct/2026:10:01:02] \"GET /p%d\" %s %d\n",
                   i % 5, (i * 7) % 200, i % 50, st[i % 8 + 1], (i * 13) % 5000
    }' > "$2"
}
analyse() {  # the pipeline of log_analysis.sh, one pass per question
    awk '{print $7}' "$1" | sort -n | uniq -c | awk '{print $2, $1}' >/dev/null
    awk '{print $1}' "$1" | sort | uniq -c | sort -k1,1nr -k2,2 | head -n 1 >/dev/null
    awk '{s += $8} END {print s}' "$1" >/dev/null
}

printf "%-12s %-12s %s\n" "lines" "file_MB" "seconds(real)"
for n in 10000 100000 1000000; do
    gen "$n" "$tmp/log.$n"
    mb=$(du -m "$tmp/log.$n" | cut -f1)
    s=$( { TIMEFORMAT='%R'; time analyse "$tmp/log.$n"; } 2>&1 )
    printf "%-12s %-12s %s\n" "$n" "$mb" "$s"
done

echo
echo "10 simulated servers, 100000 lines each, on THIS one machine (cores: $(nproc))"
for i in $(seq 1 10); do cp "$tmp/log.100000" "$tmp/srv$i.log"; done
export -f analyse
s=$( { TIMEFORMAT='%R'; time (for i in $(seq 1 10); do analyse "$tmp/srv$i.log"; done); } 2>&1 )
echo "sequential (one after another): ${s}s"
s=$( { TIMEFORMAT='%R'; time (seq 1 10 | xargs -P 10 -I{} bash -c 'analyse "$0/srv{}.log"' "$tmp"); } 2>&1 )
echo "parallel   (xargs -P 10)      : ${s}s"
