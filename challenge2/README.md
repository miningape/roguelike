# FFMPEG image sequence compiler

## Installing & Running

If you are on Mac / Linux you can use the `run.sh` script to set up the environment, download dependencies (PySide6) and run the application.

Otherwise you will need to install dependencies using `pip install -r requirements.txt` and run the application using `python main.py`. Optionally: use `python -m venv ...` to create a virtual environment.

## Usage

At a high level you can use this program to compile several images (image sequence) together into a video file.

- The list of files to be included are specified in the large text area to the left, they should be specified using their absolute path to avoid confusion, and in the order of appearance in the video. There is a button below the this text area that opens a file dialogue to automate this process.
- To the right, there are several different options you can use to tweak the outputted image such as FPS, encoding, or format. There is also a text area here that allows CLI args to be passed to `ffmpeg` directly.
- In the bottom-left there is a button that opens a "save file" dialogue which allows you to choose the directory where final video is generated to, as well as its name.
- Finally, if you're happy with the settings, press the compile button to generate your video. If something goes wrong an alert will pop up informing you of the error.

## Requirements

- Python 3.7.16
- PySide6
- ffmpeg 4.4.3
