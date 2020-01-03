FROM ubuntu:16.04

LABEL "com.github.actions.name"="VCVRackPluginBuilder-Combine"
LABEL "com.github.actions.description"="Combines VCV Rack plugin distributions"
LABEL "com.github.actions.icon"="headphones"
LABEL "com.github.actions.color"="purple"

LABEL "repository"="TBD"
LABEL "homepage"="TBD"
LABEL "maintainer"="dewb"

RUN apt-get update
RUN apt-get install -y unzip zip

ADD entrypoint.sh /entrypoint.sh
RUN chmod a+x /entrypoint.sh

ADD lin.zip /lin.zip
ADD win.zip /win.zip

ENTRYPOINT ["/entrypoint.sh"]
