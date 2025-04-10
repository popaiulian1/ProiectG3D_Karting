# ProiectG3D_Karting
A 3D karting simulation developed in C++ with modern OpenGL, allowing players to drive a kart around a custom-built track.

[![Project Video](https://img.shields.io/badge/Watch-Demo_Video-red?style=for-the-badge&logo=youtube)](https://youtu.be/Qg7PQnN6MlU)

[View Project Demo Video](https://youtu.be/Qg7PQnN6MlU)

## Technologies

- **Graphics API**: OpenGL 3.3 Core Profile
- **Languages**: C++
- **Libraries**:
  - GLFW (window creation and input)
  - GLEW (OpenGL extension loading)
  - GLM (mathematics)
  - Assimp (3D model loading)
  - stb_image (texture loading)

## Features

- Interactive 3D environment with drivable kart
- Physically-based kart controls (acceleration, braking, steering)
- Dynamic camera system with multiple views
- Lighting and texturing system
- Model loading with Assimp
- Custom shader implementation

## Controls:
### General Camera Movement:
"R" -> reset camera / unlock camera from kart  
"E" -> lock to kart seat  
"W" -> move forward   
"S" -> move back    
"A" -> move left  
"D" -> move right 

### Kart Movement and Controls: 
#### Movement:
"W" -> accelerate  
"S" -> brake  
"A" -> turn left  
"D" -> turn right  
"SPACE" -> reverse  

#### Controls:
"Q" -> reset kart to start/finish line position

## Development

The project implements a simple physics system for the kart with:
- Forward and reverse acceleration
- Engine braking
- Steering control
- Camera following with adjustable perspectives

## License

This project was developed for educational purposes at university.


