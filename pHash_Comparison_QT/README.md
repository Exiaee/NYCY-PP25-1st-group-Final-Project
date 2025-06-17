# pHash Comparison QT

A Qt-based GUI application for image similarity search using perceptual hashing (pHash). The application allows users to search for visually similar images using different processing methods (Sequential, OpenMP, pThread, etc.).

## Features
- Oepn and load SQLite database
- Image loading and preprocessing
- Compute perceptual hash (pHash)
- Search for similar images by setting a Hamming distance threshold.
- Supports multiple search modes:
  - Sequential
  - OpenMP
  - PThread
- Qt GUI for image selection, preview, and search results

## Dependencies

- Qt 5 or Qt 6 (QtWidgets, QtSql, QtConcurrent)
- SQLite3
- C++17 or later
- OpenMP support for parallel speedup
- Python 3.x + OpenCV (for CV-based pHash)

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
pHash_Comparison_QT/
├── main.cpp                 # Entry point
├── mainwindow.ui            # Main GUI layout
├── newwindow.ui             # Secondary window layout
├── comparedialog.ui         # Compare dialog GUI
├── mainwindow.cpp/h         # GUI logic
├── newwindow.cpp/h          # Secondary window logic
├── imagephash.cpp/h         # pHash algorithm from compute_phash.py and Hamming distance comparison
├── comparedialog.cpp/h      # comparedialog
├── database.cpp/h           # SQLite logic
├── compute_phash.py         # Compute pHash from loaded image using Python OpenCV
└── QtImageSQL_Advanced.pro  # Qt project file
```

## How to Use

1. Launch the application.
2. Click **Open DB** to load the SQLite database.
3. Click **Show DB Items** to view the top five entries of the database.
4. Choose the comparison method (Sequential/OpenMP/PThread) and set the Hamming Distance threshold.
5. Click **Find Similar** to select a query image and compare it against the database.
6. Matching results will be displayed in the Image List area.
7. The selected image and its pHash will be shown in a secondary window.
![image](https://github.com/user-attachments/assets/2a8d0c9d-a094-44c7-9ebe-193b0e9f7434)


### Use Python for pHash Computation

If you are using Python with OpenCV for perceptual hash computation (e.g., via ImagePHash::computeHashCV()),
make sure to configure the Python executable path manually in **imagephash.cpp**:

```cpp
QString pythonExe = "C:/Users/USER/anaconda3/python.exe";  // Modify to your Python path
```

Ensure the following Python packages are installed:

```bash
pip install opencv-python numpy
```

You can change the Python path in the application source to match your Anaconda or system-installed Python executable.

## Sample Dataset and the pHash database

You may download sample images from:

- [https://www.kaggle.com/datasets/sunghoshim/ukbench100](https://www.kaggle.com/datasets/sunghoshim/ukbench100)
  
You may download the pHash database from:
- https://drive.google.com/file/d/1jT6Gti9X4EMNYoNbfHK5qNgrk0QcF416/view?usp=drive_link

## License

This project is for educational or research use only.\
Please cite or give credit if you reuse or modify.

