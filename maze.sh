#!/bin/sh

IMAGE=maze

docker run -it -v $PWD:/app/tmp --rm $IMAGE $@