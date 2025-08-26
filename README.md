# LumionDatasmithTest

A simple Unreal Engine 5.6 (Windows 64-bit, DX12) application demonstrating **runtime Datasmith integration**.  
It connects to a CAD source via Direct Link, synchronizes geometry in real-time, updates lights from metadata,  
and exposes basic runtime UI controls for toggling rendering features.

---

## Setup

- **Engine:** Unreal Engine 5.6  
- **Platform:** Windows 64-bit  
- **Graphics API:** DirectX 12  
- Enable the **Datasmith Runtime** and **Direct Link** plugins in your project.  
- Build and run in **Development Editor** or package for **Win64**.

---

## Code Structure

### Subsystems & Core
- **`UCADSyncSubsystem`**  
  Facade for CAD sync. Manages DirectLink connection (`UCADDirectLinkSession`) and actor registry (`UCADActorRegistry`).  
  Provides connect, re-sync, light update, and connection status functions.

- **`UCADDirectLinkSession`**  
  Handles Datasmith session lifecycle. Manages connection state, proxy initialization, and anchor actor.

- **`UCADActorRegistry`**  
  Tracks and processes spawned CAD actors. Binds to world tick/spawn events, applies light data via `UCADLightManager`.

- **`UCADLightManager`**  
  Processes light proxy actors. Metadata is parsed on a worker thread into plain `LightDescription` structs,  
  then lights are spawned on the Game Thread.

### Utilities
- **`CADDatasmithInspect` (namespace)**  
  Helper functions for reading Datasmith metadata and tags, parsing values, and logging results.

### UI
- **`ARuntimeUIActor`**  
  Anchors the runtime UI system in the level. Spawns `URuntimeSettingsWidget`.

- **`URuntimeSettingsWidget`**  
  UMG widget with runtime controls:
  - Toggle ray tracing / shadows  
  - Connect / re-sync Datasmith  
  - Update lights  
  - Display connection status  

---

## Flow & Dependencies

```mermaid
flowchart TD

  subgraph Core[Core Systems]
    A[UCADSyncSubsystem] --> B[UCADDirectLinkSession]
    A --> C[UCADActorRegistry]
    C --> D[UCADLightManager]
  end

  subgraph Utilities[Utility Functions]
    E{{CADDatasmithInspect}}
  end

  subgraph UI[UI Layer]
    F[ARuntimeUIActor] --> G[URuntimeSettingsWidget]
    G -->|Calls APIs| A
  end

  B -->|Manages| H[(ADatasmithRuntimeActor)]
  D -->|Parses Metadata| E