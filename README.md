# Event driven real time operating system

![Status](https://img.shields.io/badge/status-research-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Architecture](https://img.shields.io/badge/arch-any-lightgrey)

> An experimental event-driven kernel for deterministic and reactive operating system research.

⚠️ **Disclaimer:** This project is a research prototype and is not production-ready.

---

## Table of Contents

- [Documentation](#documentation)
- [Abstract](#abstract)
- [Research Motivation](#research-motivation)
- [Design Principles](#design-principles)
- [Event Model](#event-model)
- [Implementation Details](#implementation-details)
- [Build Instructions](#build-instructions)
- [Clone Repository](#clone-repository)

---

## Documentation (Doxygen style)
[Event Driven Kernel pages](https://pavelreutski.github.io/rpv-evt-driven-os/)

## Abstract

This repository presents an event-driven kernel architecture that replaces conventional preemptive multitasking with a structured event-dispatch model.

Unlike traditional monolithic kernels, this system adopts a **reactive execution paradigm** in which all computation is initiated through explicit event propagation.

The project investigates:

- Deterministic scheduling behavior  
- Reduced synchronization complexity  
- Bounded latency characteristics  
- Modular subsystem isolation  
- Verifiable execution semantics  

The goal is to evaluate whether an event-centric kernel simplifies concurrency reasoning while preserving responsiveness.

---

## Research Motivation

Modern kernel designs commonly rely on:

- Interrupt-driven preemption  
- Shared-memory concurrency  
- Blocking system calls  
- Complex synchronization primitives  

These mechanisms introduce nondeterminism and complicate formal reasoning.

This project explores whether an event-driven execution model can:

1. Reduce concurrency hazards  
2. Improve state transition transparency  
3. Provide predictable scheduling semantics  
4. Facilitate formal modeling and verification  

---

## Design Principles

- **Event-Centric Execution**  
  All system activity is triggered by explicit events.

- **Deterministic Dispatch**  
  Event ordering policies are explicitly defined.

- **Minimal Shared State**  
  Communication occurs via structured message passing.

- **Layered Modularity**  
  Subsystems are independently replaceable.

---

### Core Subsystems

- Event Queue (bounded FIFO or lock-free)
- Dispatcher
- Cooperative Scheduler
- Interrupt Adaptation Layer

---

## Event Model

The kernel follows a structured event lifecycle:

1. Event Creation  
2. Event Enqueue  
3. Event Dispatch  
4. Handler Execution 

Events are immutable once enqueued.

The system may operate as:

- Single-threaded deterministic loop  
- Priority-ordered event processing   

---

## Implementation Details

| Component | Description |
|-----------|------------|
| Language  | C (minimal assembly for bootstrapping) |
| Architectures | Any e.g. x86_64, ARMv8, RISC-V etc. |
| Build System | CMake |
| Artifacts | Set of cmake object libraries composed into an interface one  |

---

## Build Instructions

This is an underlaying midleware component and doesnt build by its own but rather would build as part of a target platform project.
Its "cmake" frendly hence can be integrated into the target project via FetchContent_Declare(...) and FetchContent_MakeAvailable(...).
Or as a 3rd party project via add_subdirectory(...).

### Clone Repository

```bash
git clone git@github.com:pavelreutski/rpv-evt-driven-os.git
cd rpv-evt-driven-os
