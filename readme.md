# Streaming from system devices

This code is a project to Digital Video Applications Laboratory - LAVID. 
The purpose of this code is create a 4K streamer using a 4k capture device from INOGENI, but it works for any system device.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

What things you need to install the software and how to install them

```
G++ or compatible installed.
FFMPEG API
```

To install FFMPEG API on ubuntu 16.04:

```
sudo add-apt-repository ppa:jonathonf/ffmpeg-3
sudo apt update
sudo apt update && sudo apt install ffmpeg libav-tools x264 x265
sudo apt install libavdevice-dev
```

### Compile and Run

To run the code in LINUX, in the directoy file:

```
makefile
```

And next:

```
./Streaming [ip address] [port]
```

## Built With

* [Visual Studio Code](https://code.visualstudio.com/) - Used for code editting.

## Authors

* **Leoberto Soares** - [leossoaress](https://github.com/leossoaress)
* **Lincoln David** - [Lincoln](http://lavid.ufpb.br/index.php/2015/10/23/lincoln-david/)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details