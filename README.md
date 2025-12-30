THE PHYSICAL NIGHTMARE
https://img.shields.io/badge/C++-17-blue
https://img.shields.io/badge/Raylib-4.5-red
https://img.shields.io/badge/CMake-3.16-green
https://img.shields.io/badge/License-MIT-yellow
https://img.shields.io/badge/Platform-Windows%2520%7C%2520Linux%2520%7C%2520macOS-lightgrey
https://img.shields.io/badge/Architecture-ECS-purple
A physics-based action platformer built with Raylib using Entity-Component-System architecture. Battle through 7 unique zones with 4 dynamic skills in a hand-crafted nightmare world.
🎯 Features
🏗 Architecture
Entity-Component-System (ECS) design for scalable game logic
Decoupled systems: Rendering, Physics, Combat, AI operate independently
Data-oriented design for optimal performance
🎮 Gameplay
7 Unique Zones: Each with distinct mechanics and challenges
4 Skill System: Plasma, Shield, Bomb, Ultimate with combo mechanics
Physics-Based Movement: Custom physics system with momentum and collisions
Progressive Difficulty: Each zone introduces new enemies and mechanics
🔧 Technical
Cross-platform (Windows/Linux/macOS)
CMake build system with external dependency management
JSON configuration for game data
Modular level system for easy expansion
📁 Project Structure
text
src/
├── core/                 # Game core systems
│   ├── Game.cpp/hpp     # Main game loop and state management
│   └── World.cpp/hpp    # World/level management
├── systems/             # ECS Systems
│   ├── CombatSystem.cpp/hpp    # Damage, health, combat logic
│   ├── PlayerSystem.cpp/hpp    # Player input and control
│   ├── RenderSystem.cpp/hpp    # Raylib rendering backend
│   └── (More systems...)
├── levels/              # Zone implementations
│   ├── level1/          # Level 1: The Physical Nightmare
│   │   ├── Level1_Newton.cpp/hpp        # Zone 0: Newton's Cradle
│   │   ├── Level1_Zone0_Spawn.cpp/hpp   # Spawn area tutorial
│   │   ├── Level1_Zone1_Slope.cpp/hpp   # Sloping terrain challenges
│   │   ├── Level1_Zone2_Cave.cpp/hpp    # Dark cave exploration
│   │   ├── Level1_Zone3_Fall.cpp/hpp    # Gravity manipulation
│   │   ├── Level1_Zone4_Hidden.cpp/hpp  # Secret area
│   │   ├── Level1_Zone5_Boss.cpp/hpp    # Boss arena
│   │   └── Level1_Zone6_Exit.cpp/hpp    # Level exit
│   └── ZonePlatform.hpp # Base platform class
├── skills/              # Player abilities
│   ├── Skill.hpp        # Base skill interface
│   ├── SkillZ_Plasma.cpp/hpp    # Primary attack
│   ├── SkillX_Shield.cpp/hpp    # Defensive ability
│   ├── SkillC_Bomb.cpp/hpp      # Area damage
│   └── SkillV_Ultimate.cpp/hpp  # Ultimate ability
└── utils/               # Utilities
    ├── PhysicsSystem.cpp/hpp    # Custom physics engine
    └── Stubs.hpp        # Development utilities
🛠 Build Instructions
Prerequisites
CMake 3.16+ (Download)
Raylib 4.5+ (Download)
C++17 Compiler (GCC, Clang, or MSVC)
Windows
bash
# Method 1: Use setup script
setup.bat
# Method 2: Manual build
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make
Linux/macOS
bash
mkdir build && cd build
cmake ..
make -j4
Run the Game
bash
cd build
./THE_PHYSICAL_NIGHTMARE
🎮 Controls
Key	Action
← → ↑ ↓	Movement
Z	Plasma Skill (Primary)
X	Shield Skill (Defense)
C	Bomb Skill (Area)
V	Ultimate Skill (Special)
Space	Jump / Confirm
Shift	Sprint / Dash
ESC	Pause Menu / Quit
F1	Debug Mode
🔥 Skill System
Skill	Type	Effect	Cooldown
Plasma (Z)	Projectile	Fires energy bolts	0.5s
Shield (X)	Defensive	Creates protective barrier	3.0s
Bomb (C)	Area	Explosive delayed damage	5.0s
Ultimate (V)	Special	Screen-clearing attack	15.0s
📊 Development Roadmap
✅ Completed
ECS Architecture foundation
Core game systems (Input, Render, Physics)
Level 1 with 7 zones
4-skill combat system
CMake build system
🚧 In Progress
Enemy AI system
Particle effects
Sound system
Save/load game state
📋 Planned
Level 2: Digital Labyrinth
Multiplayer co-op
Level editor
Steam Workshop integration
🤝 Contributing
Contributions are welcome! Here's how:
Fork the repository
Create a branch (git checkout -b feature/awesome-feature)
Commit changes (git commit -m 'Add awesome feature')
Push to branch (git push origin feature/awesome-feature)
Open a Pull Request
Code Style
Follow existing C++ conventions
Use descriptive variable names
Comment complex algorithms
Keep functions focused (single responsibility)
📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
text
MIT License
Copyright (c) 2025 minhvnmc2010
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
👥 Credits
Development
Technologies Used
Raylib: Game framework
nlohmann/json: JSON parsing
xxHash: Fast hashing
CMake: Build system
Special Thanks
Ramon Santamaria (Raylib creator)
GitHub community for tools and support
Playtesters and contributors
📈 Repository Stats
https://img.shields.io/github/last-commit/minh2010ls12/the-physical-nightmare
https://img.shields.io/github/languages/code-size/minh2010ls12/the-physical-nightmare
https://img.shields.io/github/issues/minh2010ls12/the-physical-nightmare
https://img.shields.io/github/stars/minh2010ls12/the-physical-nightmare?style=social
🔗 Links
Repository: https://github.com/minh2010ls12/the-physical-nightmare
Issues: https://github.com/minh2010ls12/the-physical-nightmare/issues
Releases: https://github.com/minh2010ls12/the-physical-nightmare/releases
Wiki: (Coming soon)
🌟 Support
If you enjoy this project:
⭐ Star the repository
🐛 Report bugs in Issues
💡 Suggest features
🔄 Share with other developers
"In the nightmare of physics, only skill survives." 🎮
README last updated: December 2025
*Game version: 0.0.0001-alpha*
