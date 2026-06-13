# OOPS Final Project — CPU Factory Simulation

Dear ImGui(SDL2 + OpenGL) 기반의 CPU 공장 시뮬레이터입니다.
원판 웨이퍼가 Photolithography → Etcher → IonImplantator → Packager 라인을
거쳐 CPU로 완성되며, 병목/고장 시나리오에서 불량(defect) 발생과 처리량을
관찰할 수 있습니다.

## 사전 준비물 (Prerequisites)

빌드에는 **CMake**와 **SDL2**가 필요합니다. (OpenGL은 macOS 기본 제공)

### macOS
```bash
brew install cmake sdl2
```

### Ubuntu / Debian
```bash
sudo apt update
sudo apt install cmake libsdl2-dev libgl1-mesa-dev
```

> imgui 소스는 이 저장소에 포함되어 있으므로 별도 서브모듈 내려받기는 필요 없습니다.
> (ZIP으로 받아도 그대로 빌드됩니다.)

## 빌드 & 실행 (Build & Run)

저장소 루트에서:

```bash
cmake -S . -B build      # 빌드 구성
cmake --build build -j4  # 컴파일
./build/IMGuiApp         # 실행
```

`build/` 디렉터리는 `.gitignore`에 포함되어 커밋되지 않습니다.

## 프로젝트 구조

```
src/                     # 애플리케이션 소스
  main.cpp               # ImGui 프런트엔드 / 메인 루프
  Bridge.*               # UI ↔ 컨트롤러 연결 계층
  Controller.*           # 시뮬레이션 틱 / 집계 (완성/불량/고장)
  Models/
    Generator.*          # RawWafer 생성기
    Machines/            # 4개 공정 기계 + 내구도/고장
    Products/            # 웨이퍼/CPU 제품 상태 머신
CMakeLists.txt
imgui/                   # Dear ImGui (vendored)
```
