FROM gcc
COPY . /ProcessScheduler
WORKDIR /ProcessScheduler/
RUN make
ENTRYPOINT ["./scheduler"]