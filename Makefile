IMAGE=maze

export DOCKER_BUILDKIT=0

.PHONY: docker/build
docker/build:
	docker build -t $(IMAGE) --build-arg USER_ID=$(shell id -u) --build-arg GROUP_ID=$(shell id -g) -f Dockerfile .

.PHONY: docker/tag/%
docker/tag/%:
	docker tag $(IMAGE) $(IMAGE):$*

#.PHONY: docker/push/%
#docker/push/%:
#	docker push $(ARTIFACTORY)/$(IMAGE):$*

#.PHONY: docker/run/%
#docker/run/%:
#	docker run -it -v $(shell pwd):/app/shared --rm $(IMAGE) $*