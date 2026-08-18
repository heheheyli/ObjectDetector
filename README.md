# Object Detector 

A C++ desktop application that detects objects in images using a pretrained YOLOv8 model, built with MFC and OpenCV. Packaged for Windows deployment with the WiX Toolset.

## Features

- Load an image from your computer
- Run object detection using a pretrained YOLOv8n ONNX model
- View detected objects with bounding boxes, class labels, and confidence scores
- Save the annotated image

## Built With

- **C++ / MFC** — desktop application and interface
- **OpenCV (DNN module)** — image processing and model inference
- **YOLOv8n (ONNX)** — pretrained object detection model
- **vcpkg** — dependency management
- **WiX Toolset** — Windows Installer (.msi) packaging

## Installation

Download the latest `ObjectDetectorSetup.msi` from the [Releases](https://github.com/heheheyli/ObjectDetector/releases) page and run it. The application installs to `C:\Program Files\` and runs standalone.

## Usage

1. Click **Load Image** and select an image.
2. Click **Detect Objects** to run detection.
3. Click **Save** to export the annotated result.

## Project

Developed for Swinburne Unit SWE40006 – Software Deployment and Evolution.
Author: Hayley Nguyen - 105005495@student.swin.edu.au