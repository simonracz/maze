FROM ubuntu:22.04

ARG USER_ID
ARG GROUP_ID

RUN apt -y update && \
    apt -y upgrade && \
    apt -y install clang

RUN addgroup --gid $GROUP_ID user && \
    adduser --disabled-password --gecos '' --uid $USER_ID --gid $GROUP_ID user

USER user

COPY --chown=user:user . /app
WORKDIR /app

RUN clang++ -std=c++17 -O2 -o maze maze.cpp

WORKDIR /app/tmp
ENTRYPOINT [ "/app/maze" ]