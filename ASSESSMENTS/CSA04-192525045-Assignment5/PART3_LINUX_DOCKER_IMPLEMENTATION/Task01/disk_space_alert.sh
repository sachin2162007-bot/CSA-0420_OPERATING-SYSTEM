#!/usr/bin/env bash
# Disk-space alert: report every mounted file system whose use is at or above a threshold.
# Usage: disk_space_alert.sh [threshold_percent]   (default 85, or $DISK_ALERT_THRESHOLD)
# Exit status: 0 = all file systems below threshold, 1 = at least one alert, 2 = bad argument.
set -euo pipefail

threshold="${1:-${DISK_ALERT_THRESHOLD:-85}}"
case "$threshold" in
    ''|*[!0-9]*) echo "threshold must be a whole number (percent)" >&2; exit 2 ;;
esac

alerts=$(df -P -x tmpfs -x devtmpfs -x squashfs -x overlay 2>/dev/null |
    awk -v t="$threshold" -v now="$(date '+%F %T')" '
        NR > 1 {
            use = $5; sub(/%/, "", use)
            if (use + 0 >= t) printf "%s ALERT: %s (%s) is %s%% full (threshold %s%%)\n", now, $6, $1, use, t
        }')

if [ -n "$alerts" ]; then
    echo "$alerts"
    # Also send to syslog/journal when the logger command exists (cron has no terminal).
    if command -v logger >/dev/null 2>&1; then
        echo "$alerts" | logger -t disk_space_alert -p user.warning || true
    fi
    exit 1
fi
echo "$(date '+%F %T') OK: all file systems below ${threshold}%"
