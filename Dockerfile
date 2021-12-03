# syntax=docker.io/docker/dockerfile:1.3.1

ARG PACKAGE=${PACKAGE:-"wolf-system"}
ARG REFERENCE=${REFERENCE:-"gcr.io/distroless/static-debian11:nonroot@sha256:bca3c203cdb36f5914ab8568e4c25165643ea9b711b41a8a58b42c80a51ed609"}
ARG VERSION=${VERSION:-"0.1.0"}

FROM docker.io/curlimages/curl:7.80.0 as wolf
ARG BUILDARCH
ARG BUILDOS
ARG BUILDPLATFORM
ARG BUILDVARIANT
ARG TARGETARCH
ARG TARGETOS
ARG TARGETPLATFORM
ARG TARGETVARIANT
ARG ARCHVARIANT=${TARGETARCH}${TARGETVARIANT}
ARG PACKAGE
ARG VERSION
RUN set -eux \
    && curl --location --output /tmp/${PACKAGE} --show-error --silent https://github.com/WolfEngine/wolf/releases/download/v${VERSION}/${PACKAGE}-v${VERSION}-${ARCHVARIANT} \
    && chmod 755 /tmp/${PACKAGE}

# FROM docker.io/curlimages/curl:7.80.0 as linkerd-await
# ARG LINKERD_AWAIT_VERSION=v0.2.4
# RUN curl --location --output /tmp/linkerd-await --show-error --silent https://github.com/linkerd/linkerd-await/releases/download/release%2F${LINKERD_AWAIT_VERSION}/linkerd-await-${LINKERD_AWAIT_VERSION}-amd64 \
#     && chmod 755 /tmp/linkerd-await

FROM ${REFERENCE} as base
ARG PACKAGE
COPY --chown=nonroot:nonroot --from=wolf /tmp/${PACKAGE} /${PACKAGE}
# COPY --chown=nonroot:nonroot --from=linkerd-await /tmp/linkerd-await /linkerd-await

FROM base AS wolf-render

FROM base AS wolf-stream

FROM base AS wolf-system


FROM ${PACKAGE} AS after-condition
USER nonroot:nonroot
# ENTRYPOINT ["/linkerd-await", "--shutdown", "--"]
CMD ["/${PACKAGE}"]