FROM docker.io/hakarlsson/riscv-picolibc

USER root

ADD https://archive.ubuntu.com/ubuntu/pool/main/m/mtools/mtools_4.0.43-1build1_amd64.deb /tmp/mtools.deb
ADD https://archive.ubuntu.com/ubuntu/pool/universe/i/inetutils/inetutils-telnet_2.2-2ubuntu0.2_amd64.deb /tmp/telnet.deb

RUN dpkg -i /tmp/telnet.deb /tmp/mtools.deb \
    && rm -f /tmp/telnet.deb /tmp/mtools.deb
