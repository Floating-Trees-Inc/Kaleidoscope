## Road to editor
- Render graph
- Render graph editor
- Merge Seraph back in
- Node inspector
- File dialog

## RHI tests
- Texture2D load
- Texture2D alpha cutout
- Sampler
- Comparison sampler
- Write to mip
- Write to face
- Write to face + mip
- TextureCube sample
- Alpha RayQuery
- Alpha RayPipeline

## Tests when RHI has support
- BLAS compaction
- Copy buffer range
- Dispatch Mesh Indirect + ID
- ASTC
- BC7

## Metal
- Figure out MDI drawID
- Figure out SBT and raytracing pipeline (if i ever use it that is...)

## ToolIm3D
- Mesh shader variant if supported

## KernelGPU
- GPU timestamps
- BLAS compaction
- BLAS refit for skinned meshes
- RenderDoc/PIX/NSight integration
- CopyBufferRange
- Get GPU info (name, architecture, support levels)

## Renderer3D
- Clustered light culling
- Spatiotemporal variance guided filtering
- Screen space reflections
- Screen space + raytraced hybrid reflections
- Screen space ambient occlusion
- Raytraced ambient occlusion
- Screen space global illumination with RT hybrid
- Dynamic diffuse global illumination
- Eye adaptation
- Color correction
- Material batching and global index buffer for GPU-driven culling
- GPU driven meshlet culling
- DLSS/FSR/MetalFX
- ReSTIR DI
- GPU particles

## Long-term
- Remove Effects module
- Reflection/Serialization
- Animation
- Audio module
- Cooker
- KDPackage
- Job system
- Navmesh/AI
- Cinematics
- NetCore
- Renderer2D
- Renderer3D
- UICompositor
- EditorCore
- KernelGPU-Metal4
- KernelGPU-PS5
- KernelGPU-NS2
