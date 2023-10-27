FROM colmap/colmap:latest

ARG PYTHON_VERSION=3.8
RUN apt-get update -y
RUN apt-get update -y
RUN apt-get install -y unzip wget software-properties-common
RUN add-apt-repository ppa:deadsnakes/ppa && \
    apt-get -y update && \
    apt-get install -y python${PYTHON_VERSION}
RUN apt-get install -y python${PYTHON_VERSION}-distutils
RUN wget https://bootstrap.pypa.io/get-pip.py && python${PYTHON_VERSION} get-pip.py
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python${PYTHON_VERSION} 1
RUN apt-get install python${PYTHON_VERSION}-dev -y
