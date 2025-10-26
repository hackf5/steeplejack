# Woxel core data model (header-only for now)

This folder sketches the core types to represent a cylindrical voxel ("woxel") world:

- WoxelSpaceSpec: global parameters (sector count, radius, vertical/radial steps) and conversions.
- WoxelIndex / WoxelCoord: discrete vs continuous coordinates, with wrap helpers.
- ChunkDims / ChunkAddress / WoxelChunk: chunking over (theta, z, radial) and a simple dense cell store.
- WoxelWorld: container for chunks with helpers to map continuous positions to cells and back.
- ActorState: continuous cylindrical pose/velocity with local tangent/normal/up basis.

These headers are not compiled yet; they are designed to be included by future code without requiring changes to the build.
