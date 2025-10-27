# Woxel minimal API (header-only)

Initial target: draw a cylindrical column of woxels.

Kept minimal surface (build what we need):

- Namespace: `steeplejack::woxel`.
- `WoxelGrid`: `num_sectors`, `dy` (height), `dz` (radial thickness); `dtheta()` derived on demand.
- `GridIndex`: discrete indices `(i_theta, i_z)` with wrap helpers.
- Helpers are member functions on `WoxelGrid`: `theta_of`, `z_of`, `theta_index_of`, `z_index_of`, `wrap_theta_index`.

Minimal column content:

- `ColumnSpec` (in `woxel/column.h`): `inner_radius`, `layers`.

Deferred (removed for now):

- Radial layers and materials, chunking/world containers, actor state, and continuous coordinates.

These headers are not compiled directly; they are designed to be included by future code as we integrate rendering.
