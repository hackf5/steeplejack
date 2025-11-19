# Runtime configuration files

This directory contains runtime configuration such as descriptor set layouts.
Files are copied next to the binaries under `config/` so they can be loaded
without having to know the source tree layout.

- `descriptor_layouts/` — one TOML file per descriptor set layout definition.
