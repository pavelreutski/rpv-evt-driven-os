# Event driven real time operating system

![Status](https://img.shields.io/badge/status-research-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Architecture](https://img.shields.io/badge/arch-cross--platform-lightgrey)

> An experimental event-driven kernel for deterministic and reactive operating system research.

⚠️ **Disclaimer:** This project is a research prototype and is not production-ready.

---

## Table of Contents

- [Abstract](#abstract)
- [Research Motivation](#research-motivation)
- [Design Principles](#design-principles)
- [System Architecture](#system-architecture)
- [Event Model](#event-model)
- [Implementation Details](#implementation-details)
- [Build Instructions](#build-instructions)
- [Experimental Evaluation](#experimental-evaluation)
- [Formalization](#formalization)
- [Repository Structure](#repository-structure)
- [Research Questions](#research-questions)
- [Limitations](#limitations)
- [Related Work](#related-work)
- [Citation](#citation)
- [License](#license)

---

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

- **Non-Blocking Handlers**  
  Kernel components must not block.

- **Deterministic Dispatch**  
  Event ordering policies are explicitly defined.

- **Minimal Shared State**  
  Communication occurs via structured message passing.

- **Layered Modularity**  
  Subsystems are independently replaceable.

---

## System Architecture

### Core Subsystems

- Event Queue (bounded FIFO or lock-free)
- Dispatcher
- Cooperative Scheduler
- Interrupt Adaptation Layer
- Timer Subsystem
- Memory Allocator
- IPC Mechanism

---

## Event Model

The kernel follows a structured event lifecycle:

1. Event Creation  
2. Event Enqueue  
3. Dispatch Resolution  
4. Handler Execution  
5. Optional Event Emission  

Events are immutable once enqueued.

The system may operate as:

- Single-threaded deterministic loop  
- Priority-ordered event processing  
- Experimental multi-queue configuration  

---

## Implementation Details

| Component | Description |
|-----------|------------|
| Language  | C (minimal assembly for bootstrapping) |
| Architectures | x86_64, ARMv8, RISC-V |
| Build System | CMake |
| Artifacts | Set of cmake object libraries composed into an interface one  |

---

## Build Instructions

### Clone Repository

```bash
git clone git@github.com:pavelreutski/rpv-evt-driven-os.git
cd rpv-evt-driven-os
