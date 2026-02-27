# Contributing

## Development setup

1. Clone the repository.
2. Install CMake and a C compiler.
3. Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

4. Run the server:

```bash
./build/c_fullstack_server --db-config ./server/db.conf
```

## Coding guidelines

- Keep changes focused and minimal.
- Prefer cross-platform C code (Windows + POSIX).
- Do not commit generated artifacts (`build/`, `data/`, binaries).
- Update `README.md` when behavior or flags change.

## Pull request checklist

- Build succeeds locally.
- New runtime/config flags are documented.
- No generated files are included in commits.
- Commit messages are clear and action-oriented.
