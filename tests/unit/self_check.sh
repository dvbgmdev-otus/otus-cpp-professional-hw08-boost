#!/usr/bin/env bash
set -eEuo pipefail
trap 'echo "[ERROR] ${BASH_SOURCE[0]}:${LINENO}: \"${BASH_COMMAND}\" failed" >&2' ERR

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
# shellcheck disable=SC1091
source "$SCRIPT_DIR/../../scripts/lib/config.sh"
# shellcheck disable=SC1091
source "$LIB_DIR/logging.sh"

BINARY="$BIN_DIR/bayan"
TMP_DIR=""

cleanup() {
    if [[ -n "$TMP_DIR" ]]; then
        rm -rf "$TMP_DIR"
    fi
}

check_output() {
    local name="$1"
    local expected="$2"
    local actual="$3"

    if [[ "$actual" != "$expected" ]]; then
        log_error "Test failed: $name"
        log_error "Expected: [$expected]"
        log_error "Actual:   [$actual]"
        exit 1
    fi

    log_ok "Passed: $name"
}

check_contains() {
    local name="$1"
    local needle="$2"
    local actual="$3"

    if [[ "$actual" != *"$needle"* ]]; then
        log_error "Test failed: $name"
        log_error "Expected output to contain: [$needle]"
        log_error "Actual: [$actual]"
        exit 1
    fi

    log_ok "Passed: $name"
}

check_output_lines_unordered() {
    local name="$1"
    local expected="$2"
    local actual="$3"

    local sorted_expected
    sorted_expected="$(printf '%s\n' "$expected" | sort)"

    local sorted_actual
    sorted_actual="$(printf '%s\n' "$actual" | sort)"

    check_output "$name" "$sorted_expected" "$sorted_actual"
}

main() {
    if [[ ! -x "$BINARY" ]]; then
        log_error "Binary not found or not executable: $BINARY"
        exit 1
    fi

    local output
    output="$("$BINARY" --help)"

    check_contains "help output contains include option" "--include" "$output"

    TMP_DIR="$(mktemp -d)"
    trap cleanup EXIT

    printf 'same content\n' > "$TMP_DIR/first.txt"
    printf 'same content\n' > "$TMP_DIR/second.txt"
    printf 'other content\n' > "$TMP_DIR/third.txt"

    output="$("$BINARY" \
        --include "$TMP_DIR" \
        --level 0 \
        --block-size 4 \
        --hash crc32)"

    local expected
    expected="$TMP_DIR/first.txt
$TMP_DIR/second.txt"

    check_output_lines_unordered "duplicate files output" "$expected" "$output"

    log_ok "Self-check passed"
}

main "$@"
