FROM ubuntu:22.04

ARG USER_ID
ARG GROUP_ID

RUN apt -y update && \
    apt -y upgrade && \
    apt -y install clang

RUN if [ $USER_ID -lt 1000 ]; \
    then export USER_ID=1000; \
    fi && \
    if [ $GROUP_ID -lt 1000 ]; \
    then export GROUP_ID=1000; \
    fi && \
    addgroup --gid $GROUP_ID user && \
    adduser --disabled-password --gecos '' --uid $USER_ID --gid $GROUP_ID user

USER user

COPY --chown=user:user . /app
WORKDIR /app

RUN clang++ -std=c++17 -O2 -o maze maze.cpp

WORKDIR /app/shared
ENTRYPOINT [ "/app/maze" ]