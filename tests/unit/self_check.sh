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

check_command_fails_with_error() {
    local name="$1"
    local expected_error="$2"
    shift 2

    local actual_error
    if actual_error="$("$@" 2>&1 >/dev/null)"; then
        log_error "Test failed: $name"
        log_error "Expected command to fail"
        exit 1
    fi

    check_contains "$name" "$expected_error" "$actual_error"
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

    mkdir -p "$TMP_DIR/include_a/nested/deeper"
    mkdir -p "$TMP_DIR/include_b"
    mkdir -p "$TMP_DIR/excluded"

    # "директории для сканирования (может быть несколько)"
    # "не забыть, что дубликатов может быть больше чем два"
    printf 'same content\n' > "$TMP_DIR/include_a/first.TXT"
    printf 'same content\n' > "$TMP_DIR/include_a/second.txt"
    printf 'same content\n' > "$TMP_DIR/include_a/nested/nested.txt"
    printf 'same content\n' > "$TMP_DIR/include_b/third.txt"

    # "директории для исключения из сканирования (может быть несколько)"
    printf 'same content\n' > "$TMP_DIR/excluded/excluded.txt"

    # "уровень сканирования (один на все директории, 0 - только указанная директория без вложенных)"
    printf 'same content\n' > "$TMP_DIR/include_a/nested/deeper/deeper.txt"

    # "минимальный размер файла, по умолчанию проверяются все файлы больше 1 байта"
    printf 'x' > "$TMP_DIR/include_b/small.txt"

    # "маски имен файлов разрешенных для сравнения (не зависят от регистра)"
    printf 'same content\n' > "$TMP_DIR/include_b/ignored.bin"
    printf 'other content\n' > "$TMP_DIR/include_b/unique.txt"

    # "без ложных срабатываний и пропуска существующих дубликатов"
    printf 'same contenx\n' > "$TMP_DIR/include_b/same_size_other.txt"

    # "размер блока, которым производится чтения файлов, в задании этот размер упоминается как S"
    # "один из имеющихся алгоритмов хэширования (`crc32`, `md5` ...), ... H"
    output="$("$BINARY" \
        --include "$TMP_DIR/include_a" "$TMP_DIR/include_b" "$TMP_DIR/excluded" \
        --exclude "$TMP_DIR/excluded" \
        --level 1 \
        --min-size 2 \
        --mask "*.txt" \
        --block-size 5 \
        --hash md5)"

    # "Результатом работы утилиты должен быть список полных путей файлов с идентичным содержимым"
    # "Идентичные файлы должны подряд, одной группой"
    local expected_lines=(
        "$TMP_DIR/include_a/first.TXT"
        "$TMP_DIR/include_a/second.txt"
        "$TMP_DIR/include_a/nested/nested.txt"
        "$TMP_DIR/include_b/third.txt"
    )

    local expected
    expected="$(printf '%s\n' "${expected_lines[@]}")"

    check_output_lines_unordered "duplicate files output" "$expected" "$output"

    check_command_fails_with_error \
        "unknown hash returns error" \
        "Unknown hash algorithm: sha1" \
        "$BINARY" --hash sha1

    log_ok "Self-check passed"
}

main "$@"
