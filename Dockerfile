FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libsqlite3-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build -DUSE_BUNDLED_SQLITE=ON && cmake --build build

EXPOSE 8080

ENTRYPOINT ["./build/c_fullstack_server"]
CMD ["--db-config", "./server/db.conf"]
