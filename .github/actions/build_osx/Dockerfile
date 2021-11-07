
FROM debian

LABEL "com.github.actions.name"="VCVRackPluginBuilder-OSX"
LABEL "com.github.actions.description"="Builds a VCV Rack plugin for OS X"
LABEL "com.github.actions.icon"="headphones"
LABEL "com.github.actions.color"="purple"

LABEL "repository"="TBD"
LABEL "homepage"="TBD"
LABEL "maintainer"="dewb"

RUN brew install git wget cmake autoconf automake libtool jq python zstd

ADD entrypoint.sh /entrypoint.sh
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
