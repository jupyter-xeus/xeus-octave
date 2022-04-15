# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.224.3/containers/python-3-miniconda/.devcontainer/base.Dockerfile

FROM mcr.microsoft.com/vscode/devcontainers/miniconda:0-3

# [Choice] Node.js version: none, lts/*, 16, 14, 12, 10
ARG NODE_VERSION="16"
RUN if [ "${NODE_VERSION}" != "none" ]; then su vscode -c "umask 0002 && . /usr/local/share/nvm/nvm.sh && nvm install ${NODE_VERSION} 2>&1"; fi

COPY environment-dev.yml apt.txt /tmp/

RUN umask 0002 && \
    /opt/conda/bin/conda env update -n base -f /tmp/environment-dev.yml && \
    /opt/conda/bin/conda install cxx-compiler pkg-config -c conda-forge -y

# [Optional] Uncomment this section to install additional OS packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && xargs apt-get -y install --no-install-recommends < /tmp/apt.txt
