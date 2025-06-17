# QtImageSQL\_Advanced

A Qt-based GUI application for image similarity search using perceptual hashing (pHash). The application allows users to insert images into a SQLite database and search for visually similar images using different parallel processing methods (serial, OpenMP, QThread, etc.).

## Features

- Image loading and preprocessing
- Compute perceptual hash (pHash)
- Oepn and load pHash SQLite database
- Search similar images by Hamming distance
- Supports multiple search modes:
  - Serial
  - OpenMP
  - PThread
- Qt GUI for image selection, preview, and search results

## Dependencies

- Qt 5 or Qt 6 (QtWidgets, QtSql, QtConcurrent)
- SQLite3
- C++17 or later
- (Optional) OpenMP support for parallel speedup
- (Optional) Python 3.x + OpenCV (for CV-based pHash)

## Build Instructions

### Using Qt Creator (GUI)

1. **Open Qt Creator**

   - Launch the Qt Creator IDE.

2. **Open Project**

   - Go to `File` → `Open File or Project...`
   - Select `QtImageSQL_Advanced.pro` in your project directory.

3. **Configure Kit**

   - Choose an appropriate kit (Desktop GCC/Clang/MSVC, depending on your OS).
   - Enable "Shadow Build" (recommended).

4. **Build the Project**

   - Click the **hammer icon (🔨)** or press `Ctrl + B` to compile the project.
   - Wait for `Build finished successfully` message.

5. **Run the Application**

   - Click the **green play button (▶)** or press `Ctrl + R` to launch the GUI.

### Using Command Line (Linux/macOS)

```bash
qmake QtImageSQL_Advanced.pro
make
./QtImageSQL_Advanced
```

### Using Command Line (Windows MinGW)

```bash
qmake QtImageSQL_Advanced.pro
mingw32-make
./release/QtImageSQL_Advanced.exe
```

## Project Structure

```text
QtImageSQL_Advanced/
├── main.cpp                  # Entry point
├── mainwindow.ui            # Main GUI layout
├── newwindow.ui             # Secondary window layout
├── mainwindow.cpp/h         # GUI logic
├── newwindow.cpp/h          # Secondary window logic
├── imagephash.cpp/h         # pHash algorithm
├── database.cpp/h           # SQLite logic
├── compute_phash.py         # Compute pHash from loaded image using Python OpenCV
└── QtImageSQL_Advanced.pro  # Qt project file
```

## How to Use

1. Launch the application.
2. Click **Insert Images** to import images into the database.
3. Click **Find Similar** to select a query image.
4. Choose the comparison method (serial/OpenMP/QThread).
5. Results will be displayed in a scrollable preview area.

### Optional: Use Python for pHash Computation

If using Python OpenCV for pHash (e.g., via `ImagePHash::computeHashCV()`):

Make sure to configure your Python path manually in code:

```cpp
QString pythonExe = "C:/Users/USER/anaconda3/python.exe";  // Modify to your Python path
```

Ensure the following Python packages are installed:

```bash
pip install opencv-python numpy
```

You can change the Python path in the application source to match your Anaconda or system-installed Python executable.

## Sample Dataset

You may download sample images from:

- [https://www.kaggle.com/datasets/sunghoshim/ukbench100](https://www.kaggle.com/datasets/sunghoshim/ukbench100)
  
You may download the pHash database from:
- https://drive.google.com/file/d/1jT6Gti9X4EMNYoNbfHK5qNgrk0QcF416/view?usp=drive_link

## License

This project is for educational or research use only.\
Please cite or give credit if you reuse or modify.

