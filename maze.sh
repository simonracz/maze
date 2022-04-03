#!/bin/sh

IMAGE=maze

docker run -it -v $PWD:/app/shared --rm $IMAGE $@