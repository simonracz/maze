#!/bin/sh

IMAGE=maze

docker run -it -v $PWD:/app/shared --rm --name $IMAGE $IMAGE $@
#docker run -it -v $PWD:/app/shared --rm --init --name $IMAGE $IMAGE $@
