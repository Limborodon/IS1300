# STM32 Traffic & Pedestrian Control System

A dual-task, non-blocking traffic management system implemented on an **STM32 NUCLEO-L476RG** microcontroller. This project features custom bit-banged drivers, concurrent state machines for vehicle and pedestrian control, hardware debouncing, and strict safety state validation.

## Features

* **Concurrent State Machines:**
  * **Task 1 (Pedestrians):** Manages crosswalk request queues and signal transitions.
  * **Task 2 (Vehicles):** Manages horizontal and vertical lane signals and vehicle throughput.
* **Safety Rules & Fail-Safes:**
  * Pedestrian signals are hardware-gated to ensure a **WALK** state occurs *only* when vertical vehicle lanes are fully locked in a **RED** state.
* **Custom Driver Architecture:**
  * Custom bit-banged 24-bit output driver (`Shift_Out_24`) controlling three daisy-chained 74HC595 shift registers.
* **Software Debouncing:**
  * Integrated 5 ms delay filter to eliminate mechanical switch bounce on car lane sensors and pedestrian request buttons.
* **Modular Hardware Test Suite:**
  * Dedicated test driver (`test.c`) to visually verify shift register serial communication, LED mapping, and button/switch inputs.

## Hardware Requirements

| Component | Quantity | Description |
| :--- | :--- | :--- |
| **Microcontroller** | 1 | STM32 Nucleo-64 (NUCLEO-L476RG) |
| **Shift Registers** | 3 | 8-bit 74HC595 ICs (daisy-chained for 24-bit output) |
| **Display Shield** | 1 | 18-LED Traffic Light Shield |
| **Car Sensors** | 4 | Toggle/SPST switches simulating lane vehicle detection |
| **Pedestrian Buttons** | 2 | Push buttons configured with internal pull-up resistors |

## Project Structure

* `traffic_control.c` — Main application loop and top-level concurrent state machine execution.
* `traffic_control_functions.c` — Implementation of bit-banged SPI/shift drivers, debouncing logic, pin IO initialization, and state transition functions.
* `traffic_control.h` — Pin definitions, state enumerations, bitmasks, and function prototypes.
* `test.c` — Independent testing harness for verifying LED mapping, serial output, and input filtering.

## Getting Started

### Prerequisites
* **IDE:** STM32CubeIDE, Keil uVision, or IAR Embedded Workbench.
* **Compiler:** `arm-none-eabi-gcc`
* **Hardware Tooling:** ST-LINK/V2 (integrated on the Nucleo board)

### Building and Flashing
1. Clone or download this repository into your STM32CubeIDE workspace.
2. Open the project in **STM32CubeIDE**.
3. Connect your **NUCLEO-L476RG** board via USB.
4. Ensure target settings match `STM32L476RG`.
5. Build the project (**Ctrl + B**).
6. Click **Run / Debug** (**F11**) to flash the target device.

## Verification & Testing

To run the onboard hardware test suite:
1. Include `test.c` in your build target and call `run_hardware_tests()` from your main initialization function.
2. Verify that all 18 LEDs toggle sequentially across the 3 shift registers.
3. Actuate each car sensor and pedestrian button to confirm debounced state detection via the debug console or LED indicators.
