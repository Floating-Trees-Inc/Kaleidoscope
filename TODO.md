## Road to editor
- Merge Seraph back in
- Render graph editor
- Node inspector
- File dialog

## SERAPH PORT
- R3D_Bloom
- R3D_MotionVectorViz
- R3D_CascadedShadowMaps
- R3D_Tonemap
- R3D_IBLReflections
- R3D_IBLIrradiance
- R3D_RTSoftShadows
- R3D_ConfidenceMaskNormal
- R3D_ConfidenceMaskReflections
- R3D_SVGFMask
- R3D_SVGFLuminance

## Renderer3D
- R3D_ClusterLightCull
- R3D_HZB
- R3D_SSR
- R3D_GlossySSR
- R3D_RTReflections
- R3D_GlossyRTReflections
- R3D_SSAO
- R3D_RTAO
- R3D_SSGI
- R3D_DDGI
- R3D_EyeAdaptation
- R3D_ColorCorrect
- R3D_MeshShaderGBuffer
- R3D_GPUDrivenGBuffer
- R3D_Upscaler(FSR/DLSS/MetalFX)
- R3D_ReSTIRRadiance
- R3D_GPUParticles
- R3D_CPUParticles
- R3D_ScriptedPass
- R3D_VisibilityBuffer
- R3D_Pathtracer
- R3D_SSShadows
- R3D_XeGTAO
- R3D_ShadowAtlas
- R3D_TAA
- R3D_FXAA
- R3D_MotionBlur
- R3D_BokehDOF

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

## KernelGPU
- GPU timestamps
- BLAS compaction
- BLAS refit for skinned meshes (figure out VB copy and whatnot)
- RenderDoc/PIX integration
- CopyBufferRange
- Get GPU info (name, architecture, support levels)

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
- UICompositor
- EditorCore
- KernelGPU-Metal4
- KernelGPU-PS5
- KernelGPU-NS2
